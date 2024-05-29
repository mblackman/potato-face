#include <iostream>

#include "./Game/Game.h"
#include "./General/Logger.h"

int main(int argc, char* argv[]) {
    Logger::Init();

    Game game;

    game.Initialize();
    game.Run();
    game.Destroy();

    return 0;
}
