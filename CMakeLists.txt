cmake_minimum_required(VERSION 4.2)
cmake_policy(SET CMP0168 NEW)
project(NavalCommand.CommandCenter VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(ncbuild GIT_REPOSITORY https://github.com/NavalCommand/Build.CMake.git GIT_TAG main)
FetchContent_MakeAvailable(ncbuild)
include("${ncbuild_SOURCE_DIR}/NavalCommand.Build.CMake.cmake")

NavalCommandAddDependency(https://github.com/NavalCommand/Gateway.Grpc.Client.git 75ce27cf9128e5c2d04e7bb959982dccf2052f1a)
NavalCommandAddDependency(https://github.com/NavalCommand/World.git aafc4acad0147e8fc5242ad4cf8a02ecdb7f1ad7)
NavalCommandAddDependency(https://github.com/NavalCommand/TacticalDisplay.git a835a6a1ccb3741b923c452caa6278cd223f3cf9)

NavalCommandConfigureModule()
