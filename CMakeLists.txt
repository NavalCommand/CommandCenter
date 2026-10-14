cmake_minimum_required(VERSION 4.2)
cmake_policy(SET CMP0168 NEW)
project(NavalCommand.CommandCenter VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(ncbuild GIT_REPOSITORY https://github.com/NavalCommand/Build.CMake.git GIT_TAG main)
FetchContent_MakeAvailable(ncbuild)
include("${ncbuild_SOURCE_DIR}/NavalCommand.Build.CMake.cmake")

NavalCommandAddDependency(https://github.com/NavalCommand/Gateway.Grpc.Client.git 4fc239e328a314063b0b7e7c829425d5b9596fbf)
NavalCommandAddDependency(https://github.com/NavalCommand/World.git 660b32c3066d8d99e57ed2e2015bd5afb6fb0fb1)
NavalCommandAddDependency(https://github.com/NavalCommand/TacticalDisplay.git 667b2730d3efc1c675bc117b27fcce3fe4477426)

NavalCommandConfigureModule()
