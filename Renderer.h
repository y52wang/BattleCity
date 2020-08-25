/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <SDL2/SDL_opengl.h>

#include <string>
#include <map>
#include "Sprites.h"
using namespace std;

struct Atlas {
    Atlas()
        : m_tex(NULL)
        , width(0), height(0)
    {}
        
    SDL_Texture* m_tex;
    int width, height;
};

class CRenderer {
public:
    ~CRenderer();

    void Init();
    void LoadAtlasFromFile(const string &fileName, const string &atlasName);

    void StartRendering();
    void StopRendering();
    void DrawSprite(SpriteData &sprite_data,
        int frame,
        int scr_x, int scr_y, int width, int height,  // screen_x, screen_y
        COLOR color = COLOR_NONE);

    void DrawPoint(int scr_x, int scr_y, SDL_Color clr);
    void DrawRect(int scr_x, int scr_y, int width, int height, SDL_Color clr);
    void FillRect(int scr_x, int scr_y, int width, int height, SDL_Color clr);

    int AtlasWidth(const string &name) { return m_atlasses[name].width; }
    int AtlasHeight(const string &name) { return m_atlasses[name].height; }

    static SDL_Color    _red_half;
    static SDL_Color    _red;
    static SDL_Color    _green_half;
    static SDL_Color    _green;
    static SDL_Color    _blue_half;
    static SDL_Color    _blue;
    static SDL_Color    _white_half;
    static SDL_Color    _white;
    static SDL_Color    _yellow_half;
    static SDL_Color    _yellow;
    static SDL_Color    _cyan_half;
    static SDL_Color    _cyan;
    static SDL_Color    _magenta_half;
    static SDL_Color    _magenta;

private:
    map<string, Atlas>      m_atlasses;

    void InsertAtlas(const string &name, Atlas atlas)
    { m_atlasses.insert(make_pair(name, atlas)); }
};

#endif // RENDERER_H_INCLUDED
