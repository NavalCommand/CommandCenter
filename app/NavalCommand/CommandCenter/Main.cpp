#include <NavalCommand/Gateway/Grpc/Client/Connection.h>
#include <NavalCommand/Symbology/Nato/ImGui/Renderer.h>
#include <NavalCommand/TacticalDisplay/Display.h>
#include <NavalCommand/World/Engine.h>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>

namespace NavalCommand::CommandCenter
{
	class ControlWorker
	{
		std::mutex m_Mutex;
		std::condition_variable m_Changed;
		std::deque<std::function<void()>> m_Requests;
		bool m_Stopping{};
		std::thread m_Thread;

	public:
		ControlWorker()
			: m_Thread{[this]
					   {
						   for (;;)
						   {
							   std::function<void()> request;
							   {
								   std::unique_lock lock{m_Mutex};
								   m_Changed.wait(lock,
												  [&]
												  {
													  return m_Stopping || !m_Requests.empty();
												  });
								   if (m_Stopping && m_Requests.empty())
								   {
									   return;
								   }
								   request = std::move(m_Requests.front());
								   m_Requests.pop_front();
							   }
							   request();
						   }
					   }}
		{
		}

		~ControlWorker()
		{
			Stop();
		}

		bool Submit(std::function<void()> request)
		{
			std::lock_guard lock{m_Mutex};
			constexpr std::size_t MAXIMUM_PENDING_CONTROLS = 16;
			if (m_Stopping || m_Requests.size() >= MAXIMUM_PENDING_CONTROLS)
			{
				return false;
			}
			m_Requests.push_back(std::move(request));
			m_Changed.notify_one();
			return true;
		}

		void Stop()
		{
			{
				std::lock_guard lock{m_Mutex};
				m_Stopping = true;
				m_Changed.notify_all();
			}
			if (m_Thread.joinable())
			{
				m_Thread.join();
			}
		}
	};
}

int main(int argc, char** argv)
{
	using namespace NavalCommand;
	std::string endpoint = "127.0.0.1:50051";
	std::chrono::milliseconds refresh{250};
	std::optional<std::chrono::milliseconds> duration;
	std::optional<Factions::Api::Id> faction;
	try
	{
		for (int argument = 1; argument < argc; ++argument)
		{
			const std::string option = argv[argument];
			if (argument + 1 >= argc)
			{
				throw std::invalid_argument("missing_option_value");
			}
			const std::string value = argv[++argument];
			if (option == "--endpoint")
			{
				endpoint = value;
			}
			else if (option == "--refresh-ms")
			{
				refresh = std::chrono::milliseconds{std::stoi(value)};
			}
			else if (option == "--duration-ms")
			{
				duration = std::chrono::milliseconds{std::stoi(value)};
			}
			else if (option == "--faction")
			{
				faction = Factions::Api::Id{value};
			}
			else
			{
				throw std::invalid_argument("unknown_option");
			}
		}
		Gateway::Grpc::Client::Connection connection{endpoint};
		World::Engine world{[&](std::optional<Factions::Api::Id> selected) -> Error::Api::Result<World::UpstreamView>
							{
								auto acquired = connection.Acquire(selected);
								if (!acquired)
								{
									return std::unexpected(acquired.error());
								}
								auto view = *acquired;
								return World::UpstreamView{
									view, World::Providers{*view, *view, *view, *view, *view, *view, *view, *view, *view, *view, *view, *view}, view->Session(), view->Revision()};
							},
							refresh};
		world.Select(faction);
		world.Start();
		Symbology::Nato::ImGui::Renderer renderer;
		TacticalDisplay::Display display{renderer};
		CommandCenter::ControlWorker controls;
		const int result = display.Run(
			[&]
			{
				auto view = world.AcquireView();
				auto status = world.GetStatus();
				if (!view)
				{
					return TacticalDisplay::Frame{
						{}, {}, false, status.Failure ? "Simulation unavailable / reconnecting" : "Loading perspective", faction ? faction->Value : "Omniscient"};
				}
				return TacticalDisplay::Frame{view,
											  TacticalDisplay::Providers{*view, *view, *view, *view, *view, *view, *view, *view, *view, *view, *view},
											  status.Stale,
											  status.Stale ? "STALE / connection interrupted" : "Connected / session " + view->Session(),
											  faction ? faction->Value : "Omniscient",
											  view->Session()};
			},
			[&](std::optional<Factions::Api::Id> selected)
			{
				faction = selected;
				world.Select(std::move(selected));
			},
			[&](std::function<void()> request)
			{
				return controls.Submit(std::move(request));
			},
			duration);
		// Drain controls while the display callback state and retained views live.
		controls.Stop();
		world.Stop();
		connection.Stop();
		return result;
	}
	catch (const std::exception& error)
	{
		std::cerr << "CommandCenter: " << error.what() << '\n';
		return 1;
	}
}
