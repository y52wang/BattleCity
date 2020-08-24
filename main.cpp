/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#ifdef USE_SDL2
#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")
#pragma comment(lib, "SDL2_mixer")
#else
#pragma comment(lib, "SDL")
#pragma comment(lib, "SDLmain")
#pragma comment(lib, "SDL_mixer")
#endif

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
