NavalCommandAddDependency(https://github.com/NavalCommand/Gateway.Grpc.git 407ecb7c82692181a71a8189a8a19a53a98d2084)
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
add_executable(NavalCommand.CommandCenter.Test NavalCommand/CommandCenter/IntegrationTest.cpp)
target_link_libraries(NavalCommand.CommandCenter.Test PRIVATE
    NavalCommand.Gateway.Grpc.Client NavalCommand.World NavalCommand.TacticalDisplay
    NavalCommand.Symbology.Nato.ImGui NavalCommand.Gateway.Grpc.Mock GTest::gtest_main)
NavalCommandConfigureTarget(NavalCommand.CommandCenter.Test)
gtest_discover_tests(NavalCommand.CommandCenter.Test DISCOVERY_MODE PRE_TEST)
