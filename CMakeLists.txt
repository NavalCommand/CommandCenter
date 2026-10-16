cmake_minimum_required(VERSION 4.2)
cmake_policy(SET CMP0168 NEW)
project(NavalCommand.CommandCenter VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(ncbuild GIT_REPOSITORY https://github.com/NavalCommand/Build.CMake.git GIT_TAG main)
FetchContent_MakeAvailable(ncbuild)
include("${ncbuild_SOURCE_DIR}/NavalCommand.Build.CMake.cmake")

NavalCommandAddDependency(https://github.com/NavalCommand/Gateway.Grpc.Client.git 4960966e505be4aa9861180660e80e439b8c0c3f)
NavalCommandAddDependency(https://github.com/NavalCommand/World.git 66ec0b2e498a378af63f53a90d2d1ddfee63ddbc)
NavalCommandAddDependency(https://github.com/NavalCommand/TacticalDisplay.git d59b31a7298c210e208ceed38101acdaadd71488)
NavalCommandAddDependency(https://github.com/NavalCommand/Symbology.Nato.ImGui.git e257bd69ad91e1845115af35f17fdcec32696c0f)

NavalCommandConfigureModule()
