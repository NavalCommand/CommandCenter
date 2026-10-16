cmake_minimum_required(VERSION 4.2)
cmake_policy(SET CMP0168 NEW)
project(NavalCommand.CommandCenter VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(ncbuild GIT_REPOSITORY https://github.com/NavalCommand/Build.CMake.git GIT_TAG main)
FetchContent_MakeAvailable(ncbuild)
include("${ncbuild_SOURCE_DIR}/NavalCommand.Build.CMake.cmake")

NavalCommandAddDependency(https://github.com/NavalCommand/Gateway.Grpc.Client.git 714a80fb7d7d396a6aeb95fbf0163675a153b3d6)
NavalCommandAddDependency(https://github.com/NavalCommand/World.git 001849a40f1d7b332311f1bd6dc46c9c9d5d4941)
NavalCommandAddDependency(https://github.com/NavalCommand/TacticalDisplay.git 43808b144d2ba49774b2378fbfd6bb460c283d7a)
NavalCommandAddDependency(https://github.com/NavalCommand/Symbology.Nato.ImGui.git 9445e0cfc8cfea11c3f7491ca8c3f152bc55a8a9)

NavalCommandConfigureModule()
