cmake_minimum_required(VERSION 4.2)
cmake_policy(SET CMP0168 NEW)
project(NavalCommand.CommandCenter VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(ncbuild GIT_REPOSITORY https://github.com/NavalCommand/Build.CMake.git GIT_TAG main)
FetchContent_MakeAvailable(ncbuild)
include("${ncbuild_SOURCE_DIR}/NavalCommand.Build.CMake.cmake")

NavalCommandAddDependency(https://github.com/NavalCommand/Gateway.Grpc.Client.git 416536bb028408d4004b837181c32f66a5d72fab)
NavalCommandAddDependency(https://github.com/NavalCommand/World.git c64d2a7ac12d024ce38729fc4314f2a3dd44290a)
NavalCommandAddDependency(https://github.com/NavalCommand/TacticalDisplay.git 4814533616cf6993ed181471ea9ecfd05490cbb4)
NavalCommandAddDependency(https://github.com/NavalCommand/Symbology.Nato.ImGui.git 3ca246c22925f9c64aed587aa8efcea4e866ce69)

NavalCommandConfigureModule()
