/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")
#pragma comment(lib, "SDL2_mixer")
//#pragma comment(lib, "glew32")

#include <ctime>
#include "Game.h"
#undef main

int main() {
    srand(time(NULL));

    CGame &Game = CGame::Get();
    Game.Window()->Set(640, 480, 32, false);
    Game.Init();

    return 0;
}
