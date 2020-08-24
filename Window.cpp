/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#include <GL/glew.h>
#include <iostream>
#include <assert.h>
#include "Window.h"
#include "Game.h"

using namespace std;

void CWindow::Init() {
    int gr = GL_NO_ERROR;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
        cout << "- Pomyslnie zainicjalizowano bibliteke SDL.\n";
    else
        cout << "# Nie udalo sie zaladowac biblioteki SDL!\n";


    if(m_scr_fullscreen)
    {
        m_window = SDL_CreateWindow(m_title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            m_scr_width,
            m_scr_height,
            SDL_WINDOW_FULLSCREEN);
    }
    else
    {
        //m_window = SDL_CreateWindow(m_title.c_str(),
        //    SDL_WINDOWPOS_UNDEFINED,
        //    SDL_WINDOWPOS_UNDEFINED,
        //    m_scr_width,
        //    m_scr_height,
        //    SDL_WINDOW_OPENGL);

        int res = SDL_CreateWindowAndRenderer(
            m_scr_width,
            m_scr_height,
            SDL_WINDOW_OPENGL,
            &m_window,
            &m_renderer);
        assert(res==0);
    }

    if(m_window != NULL)
    {
        cout << "- Zainicjalizowano tryb wideo.\n";
    }
    else
    {
        cout << "# Nie zainicjalizowano trybu wideo!";
    }

    SDL_GLContext context = SDL_GL_CreateContext(m_window);

    GLenum err = glewInit();
    assert(GLEW_OK==err);

    glViewport(0, 0, m_scr_width, m_scr_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_scr_width, 0, m_scr_height, -1, 5);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


    SDL_GL_SetSwapInterval(0);

    glClearColor(0.455, 0.455, 0.455, 1);  //szare tło
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.5);  //Jeśli byłby problem z kanałem alpha - zmień na więcej!
}
