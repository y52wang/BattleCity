/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#include <iostream>
#include "Window.h"
#include "Game.h"

using namespace std;

void CWindow::Init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
        cout << "- Pomyslnie zainicjalizowano bibliteke SDL.\n";
    else
        cout << "# Nie udalo sie zaladowac biblioteki SDL!\n";

#ifndef USE_SDL2
    SDL_WM_SetCaption(m_title.c_str(), NULL);
#endif

    if(m_scr_fullscreen)
    {
#ifdef USE_SDL2
        m_window = SDL_CreateWindow(m_title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            m_scr_width,
            m_scr_height,
            SDL_WINDOW_FULLSCREEN | SDL_SWSURFACE);
#else
        m_screen = SDL_SetVideoMode(m_scr_width, m_scr_height, m_scr_bits, SDL_OPENGL | SDL_FULLSCREEN | SDL_SWSURFACE);
#endif
    }
    else
    {
#ifdef USE_SDL2
        m_window = SDL_CreateWindow(m_title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            m_scr_width,
            m_scr_height,
            SDL_SWSURFACE);
#else
        m_screen = SDL_SetVideoMode(m_scr_width, m_scr_height, m_scr_bits, SDL_OPENGL | SDL_HWSURFACE);
#endif
    }

#ifdef USE_SDL2
    if(m_window != NULL)
#else
    if(m_screen != NULL)
#endif
        cout << "- Zainicjalizowano tryb wideo.\n";
    else                    cout << "# Nie zainicjalizowano trybu wideo!";



    glViewport(0, 0, m_scr_width, m_scr_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_scr_width, 0, m_scr_height, -1, 5);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifdef USE_SDL2
    SDL_GL_SetSwapInterval(0);
#else
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);
#endif

    glClearColor(0.455, 0.455, 0.455, 1);       //szare tło
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.5);                //Jeśli byłby problem z kanałem alpha - zmień na więcej!
}
