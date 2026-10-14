NavalCommandAddDependency(https://github.com/NavalCommand/Gateway.Grpc.git 8c1bfb771182eef0f0fc9ae2054fd908d7fdf138)
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
add_executable(NavalCommand.CommandCenter.Test NavalCommand/CommandCenter/IntegrationTest.cpp)
target_link_libraries(NavalCommand.CommandCenter.Test PRIVATE
    NavalCommand.Gateway.Grpc.Client NavalCommand.World NavalCommand.TacticalDisplay
    NavalCommand.Gateway.Grpc.Mock GTest::gtest_main)
NavalCommandConfigureTarget(NavalCommand.CommandCenter.Test)
gtest_discover_tests(NavalCommand.CommandCenter.Test DISCOVERY_MODE PRE_TEST)
