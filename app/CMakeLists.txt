add_executable(NavalCommand.CommandCenter NavalCommand/CommandCenter/Main.cpp)
target_link_libraries(NavalCommand.CommandCenter PRIVATE
    NavalCommand.Gateway.Grpc.Client NavalCommand.World NavalCommand.TacticalDisplay NavalCommand.Symbology.Nato.ImGui)
NavalCommandConfigureTarget(NavalCommand.CommandCenter)
