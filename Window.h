/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#endif

#include <GL/gl.h>
#include <string>
using namespace std;

class CWindow {
public:
    void Init();
    void Set(int width, int height, int bits, bool fullscreen) {
        m_scr_width = width;
        m_scr_height = height;
        m_scr_bits = bits;
        m_scr_fullscreen = fullscreen;

        m_title = "Battle City PC 1.1";
    }
    int WindowWidth()           { return m_scr_width; }
    int WindowHeight()          { return m_scr_height; }
    string WindowTitle()        { return m_title; }

#ifdef USE_SDL2
    SDL_Window* GetWindow()     { return m_window; }
#else
    SDL_Surface* GetSurface()   { return m_screen; }
#endif

private:
    int             m_scr_width;
    int             m_scr_height;
    int             m_scr_bits;
    bool            m_scr_fullscreen;
    string          m_title;

#ifdef USE_SDL2
    SDL_Window*     m_window;
#else
    SDL_Surface*    m_screen;
#endif
};

#endif // WINDOW_H_INCLUDED
