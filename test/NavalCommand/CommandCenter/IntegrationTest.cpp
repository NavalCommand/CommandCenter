#include <NavalCommand/Gateway/Grpc/Client/Connection.h>
#include <NavalCommand/Gateway/Grpc/MockProvider.h>
#include <NavalCommand/Gateway/Grpc/Server.h>
#include <NavalCommand/TacticalDisplay/Display.h>
#include <NavalCommand/World/Engine.h>
#include <atomic>
#include <future>
#include <gtest/gtest.h>
#include <thread>

namespace NavalCommand::CommandCenter
{
	using namespace std::chrono_literals;

	template <class Predicate> bool Await(Predicate predicate)
	{
		const auto deadline = std::chrono::steady_clock::now() + 2500ms;
		while (std::chrono::steady_clock::now() < deadline)
		{
			if (predicate())
			{
				return true;
			}
			std::this_thread::sleep_for(20ms);
		}
		return false;
	}

	TEST(CommandCenterIntegration, ScopedPollingControlsReconnectAndRendering)
	{
		Gateway::Grpc::MockProvider provider;
		Gateway::Grpc::Server host{provider};
		ASSERT_TRUE(host.Start("127.0.0.1:0"));
		const auto endpoint = "127.0.0.1:" + std::to_string(host.Port());
		Gateway::Grpc::Client::Connection connection{endpoint, 200ms};
		World::Engine world{[&](std::optional<Factions::Api::Id> faction) -> Error::Api::Result<World::UpstreamView>
							{
								auto acquired = connection.Acquire(faction);
								if (!acquired)
								{
									return std::unexpected(acquired.error());
								}
								auto view = *acquired;
								return World::UpstreamView{
									view, World::Providers{*view, *view, *view, *view, *view, *view, *view, *view, *view, *view, *view, *view}, view->Session(), view->Revision()};
							},
							30ms};
		world.Start();
		std::atomic<int> frames{};
		std::atomic<bool> completed{};
		std::jthread checks{[&]
							{
								ASSERT_TRUE(Await(
									[&]
									{
										return world.AcquireView() != nullptr;
									}));
								auto initial = world.AcquireView();
								ASSERT_EQ(initial->GetUnitInstanceIds()->size(), 2);
								world.Select(Factions::Api::Id{"blue"});
								ASSERT_TRUE(Await(
									[&]
									{
										return world.AcquireView() != nullptr;
									}));
								auto blue = world.AcquireView();
								ASSERT_EQ(blue->GetUnitInstanceIds()->size(), 1);
								EXPECT_FALSE(blue->GetUnitInstance({"truth-red"}));
								ASSERT_TRUE(blue->GetObservationInstance({"contact-1"}));
								EXPECT_EQ(blue->GetObservationInstance({"contact-1"})->ObserverFaction.Value, "blue");
								EXPECT_FALSE(blue->GetObservationInstance({"contact-1"})->Kinematics.Speed.IsKnown());
								ASSERT_TRUE(blue->RequestPause(true));
								ASSERT_TRUE(blue->SetTimeCompression({4}));
								ASSERT_TRUE(Await(
									[&]
									{
										auto current = world.AcquireView();
										return current && *current->IsPaused() && current->GetTimeCompression()->Value == 4;
									}));
								EXPECT_FALSE(*blue->IsPaused()); // Retained revision stays immutable.
								host.Stop();
								ASSERT_TRUE(Await(
									[&]
									{
										return world.GetStatus().Stale;
									}));
								ASSERT_TRUE(world.AcquireView());
								EXPECT_EQ(world.AcquireView()->GetUnitInstanceIds()->size(), 1);
								ASSERT_TRUE(host.Start(endpoint));
								ASSERT_TRUE(Await(
									[&]
									{
										return !world.GetStatus().Stale && world.AcquireView() && !*world.AcquireView()->IsPaused();
									}));
								provider.Reload("replacement-mission");
								ASSERT_TRUE(Await(
									[&]
									{
										auto current = world.AcquireView();
										return current && current->Session() == "replacement-mission";
									}));
								EXPECT_EQ(world.AcquireView()->GetCurrentTime()->Value, 0);
								world.Select(Factions::Api::Id{"red"});
								ASSERT_TRUE(Await(
									[&]
									{
										return world.AcquireView() != nullptr;
									}));
								EXPECT_EQ(world.AcquireView()->GetObservationInstance({"contact-1"})->ObserverFaction.Value, "red");
								completed = true;
							}};
		TacticalDisplay::Display display;
		std::vector<std::future<void>> controls;
		const auto result = display.Run(
			[&]
			{
				++frames;
				auto view = world.AcquireView();
				if (!view)
				{
					return TacticalDisplay::Frame{{}, {}, false, "Loading mock perspective"};
				}
				return TacticalDisplay::Frame{
					view, TacticalDisplay::Providers{*view, *view, *view, *view, *view, *view, *view, *view, *view, *view}, world.GetStatus().Stale, "Mock integration test"};
			},
			[&](std::optional<Factions::Api::Id> faction)
			{
				world.Select(std::move(faction));
			},
			[&](std::function<void()> request)
			{
				controls.push_back(std::async(std::launch::async, std::move(request)));
				return true;
			},
			10s);
		for (auto& control : controls)
		{
			control.get();
		}
		checks.join();
		world.Stop();
		connection.Stop();
		EXPECT_EQ(result, 0);
		EXPECT_GT(frames.load(), 1);
		EXPECT_TRUE(completed.load());
	}
}
