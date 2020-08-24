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


    if(m_scr_fullscreen) {
        int res = SDL_CreateWindowAndRenderer(
            m_scr_width,
            m_scr_height,
            SDL_WINDOW_FULLSCREEN,
            &m_window,
            &m_renderer);
        assert(res==0);
    } else {
        int res = SDL_CreateWindowAndRenderer(
            m_scr_width,
            m_scr_height,
            SDL_WINDOW_OPENGL,
            &m_window,
            &m_renderer);
        assert(res==0);
    }

    if(m_window != NULL) {
        SDL_SetWindowTitle(m_window, m_title.c_str());
        cout << "- Zainicjalizowano tryb wideo.\n";
    } else {
        cout << "# Nie zainicjalizowano trybu wideo!";
    }

    m_context = SDL_GL_CreateContext(m_window);

    // WangLiang: 下面是否可以不需要，彻底不依赖 opengl
    GLenum err = glewInit();
    assert(err==GLEW_OK);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(0);
}
