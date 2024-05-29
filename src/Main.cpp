#include <iostream>

#include "Game.h"
#include "Logger.h"

int main(int argc, char* argv[]) {
    Logger::Init();

    Game game;

    game.Initialize();
    game.Run();
    game.Destroy();

    return 0;
}
