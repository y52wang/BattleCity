/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#include <SDL2/SDL.h>

#include <assert.h>
#include <iostream>
#include "Game.h"
#include "Renderer.h"
using namespace std;

SDL_Color CRenderer::_red_half      = { 127, 0, 0, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_red           = { 255, 0, 0, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_green_half    = { 0, 127, 0, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_green         = { 0, 255, 0, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_blue_half     = { 0, 0, 127, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_blue          = { 0, 0, 255, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_white_half    = { 127, 127, 127, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_white         = { 255, 255, 255, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_yellow_half   = { 127, 127, 0, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_yellow        = { 255, 255, 0, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_cyan_half     = { 0, 127, 127, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_cyan          = { 0, 255, 255, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_magenta_half  = { 127, 0, 127, SDL_ALPHA_OPAQUE };
SDL_Color CRenderer::_magenta       = { 255, 0, 255, SDL_ALPHA_OPAQUE };

CRenderer::~CRenderer() {
    map<string, Atlas>::iterator iter;
    for(iter = m_atlasses.begin(); iter != m_atlasses.end(); ++iter) {
        SDL_DestroyTexture(iter->second.m_tex);
        iter->second.m_tex = NULL;
        cout << "Zwolniono atlas '" << iter->first << "'\n";
    }
}

void CRenderer::Init() {
    LoadAtlasFromFile("data/sprite/sprite.bmp", "atlas");
}

void CRenderer::LoadAtlasFromFile(const string &fileName, const string &atlasName) {
    //Ladowanie atlasu z pliku i dodawanie jako nowy atlas do mapy m_atlasses

    Atlas NewAtlas;                                                         //Atlas który zostanie dodany
    SDL_Surface* temp_surface = SDL_LoadBMP(fileName.c_str());              //Tymczasowa powierzchnia
    if(!temp_surface) {
        cout << "Nie mozna bylo zaladowac atlasu z pliku '" << fileName << "'!\n";
        exit(1);
    }

    NewAtlas.width  = temp_surface->w;                                      //Szerokość atlasu
    NewAtlas.height = temp_surface->h;                                      //Wysokość atlasu

    if (((NewAtlas.width & (NewAtlas.width - 1)) != 0) ||                   //Sprawdzanie atlasu pod kątem rozmiaru
        ((NewAtlas.height & (NewAtlas.height - 1)) != 0)) {
        cout << "Nieprawidlowy rozmiar (" << fileName << ")!\n";
        exit(1);
    }

    GLenum format;                                                          //Format pliku bmp
    switch (temp_surface->format->BytesPerPixel) {
        case 3:                                                             //Bez przezroczystosci
            format = GL_BGR;
            break;
        case 4:                                                             //Z przezroczystoscia
            format = GL_BGRA;
            break;
        default:                                                            //Nieznany format
            cout << "Nieznany format pliku (" << fileName << ")!\n";
            exit(1);
    }

    SDL_Renderer* r = CGame::Get().Window()->GetRenderer();
    NewAtlas.m_tex =  SDL_CreateTextureFromSurface(r, temp_surface);

    if (temp_surface)                                                       //Zwolnienie pamięci
        SDL_FreeSurface(temp_surface);

    InsertAtlas(atlasName, NewAtlas);
    cout << "- Zaladowano nowy atlas '" << atlasName << "' z pliku '" << fileName << "'.\n";
}

void CRenderer::StartRendering() {
    SDL_Renderer* render = CGame::Get().Window()->GetRenderer();

    SDL_Color clrbk;
    SDL_GetRenderDrawColor(render, &clrbk.r, &clrbk.g, &clrbk.b, &clrbk.a);
    SDL_SetRenderDrawColor(render, 116, 116, 116, SDL_ALPHA_OPAQUE);  // 使用灰色进行 Clear
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, clrbk.r, clrbk.g, clrbk.b, clrbk.a);
}

void CRenderer::StopRendering() {
    SDL_Renderer* render = CGame::Get().Window()->GetRenderer();
    SDL_RenderPresent(render);
    SDL_Delay(1000/60);
}

// scr_x:       screen x
// scr_y:       screen y
// CRenderer::DrawSprite 所认为的坐标系，x 向右，y 向上，原点在左下角
// SDL2 所认为的坐标系，x 向右，y 向下，原点在左上角
// Player.cpp 中 void CPlayer::Draw() 函数中亦有说明！
void CRenderer::DrawSprite(SpriteData& sprite_data, int frame,
    int scr_x, int scr_y, int width, int height,
    COLOR color)
{
    CGame&          g = CGame::Get();
    CWindow*        w = g.Window();
    SDL_Renderer*   r = w->GetRenderer();

    int offX = g.GameOffsetX();
    int offY = g.GameOffsetY();
    scr_x = offX + scr_x;
    scr_y = w->WindowHeight() - (scr_y + height) - offY;

    //if(color != COLOR_NONE) {
    //    double r,g,b;
    //    switch(color) {
    //        case COLOR_BLACK:   r = 0; g = 0; b = 0; break;
    //        case COLOR_RED:     r = 1; g = 0; b = 0; break;
    //        case COLOR_GREEN:   r = 0; g = 1; b = 0; break;
    //        case COLOR_BLUE:    r = 0; g = 0; b = 1; break;
    //    }
    //    glColor3d(r,g,b);
    //}

    const Atlas& atlas = m_atlasses[sprite_data.atlas];

    int left     = sprite_data.left + (sprite_data.width * frame);
    int right    = left + sprite_data.width;
    int bottom   = sprite_data.bottom;
    int top      = bottom - sprite_data.height;

    SDL_Rect src = { left, top, sprite_data.width, sprite_data.height };
    SDL_Rect dst = { scr_x, scr_y, width, height };
    SDL_RenderCopy(r, atlas.m_tex, &src, &dst);

    //if(color != COLOR_NONE) {
    //    glColor3d(1,1,1);
    //}
}

void CRenderer::DrawPoint(int scr_x, int scr_y, SDL_Color clr)
{
    CGame&          g       = CGame::Get();
    CWindow*        w       = g.Window();
    SDL_Renderer*   render  = w->GetRenderer();

    SDL_Color clrbk;
    SDL_GetRenderDrawColor(render, &clrbk.r, &clrbk.g, &clrbk.b, &clrbk.a);
    SDL_SetRenderDrawColor(render, clr.r, clr.g, clr.b, clr.a);

    int offX = g.GameOffsetX();
    int offY = g.GameOffsetY();
    scr_x = offX + scr_x;
    scr_y = w->WindowHeight() - scr_y - offY;

    SDL_RenderDrawPoint(render, scr_x, scr_y);

    SDL_SetRenderDrawColor(render, clrbk.r, clrbk.g, clrbk.b, clrbk.a);
}

void CRenderer::DrawLine(int scr_x0, int scr_y0, int scr_x1, int scr_y1, SDL_Color clr)
{
    CGame&          g       = CGame::Get();
    CWindow*        w       = g.Window();
    SDL_Renderer*   render  = w->GetRenderer();

    SDL_Color clrbk;
    SDL_GetRenderDrawColor(render, &clrbk.r, &clrbk.g, &clrbk.b, &clrbk.a);
    SDL_SetRenderDrawColor(render, clr.r, clr.g, clr.b, clr.a);

    int offX = g.GameOffsetX();
    int offY = g.GameOffsetY();
    scr_x0 = offX + scr_x0;
    scr_y0 = w->WindowHeight() - scr_y0 - offY;
	scr_x1 = offX + scr_x1;
	scr_y1 = w->WindowHeight() - scr_y1 - offY;

    SDL_RenderDrawLine(render, scr_x0, scr_y0, scr_x1, scr_y1);

    SDL_SetRenderDrawColor(render, clrbk.r, clrbk.g, clrbk.b, clrbk.a);
}

void CRenderer::DrawRect(int scr_x, int scr_y, int width, int height, SDL_Color clr)
{
    CGame&          g       = CGame::Get();
    CWindow*        w       = g.Window();
    SDL_Renderer*   render  = w->GetRenderer();

    SDL_Color clrbk;
    SDL_GetRenderDrawColor(render, &clrbk.r, &clrbk.g, &clrbk.b, &clrbk.a);
    SDL_SetRenderDrawColor(render, clr.r, clr.g, clr.b, clr.a);

    int offX = g.GameOffsetX();
    int offY = g.GameOffsetY();
    scr_x = offX + scr_x;
    scr_y = w->WindowHeight() - (scr_y + height) - offY;

    SDL_Rect rect =
    {
        scr_x, scr_y, width, height
    };
    SDL_RenderDrawRect(render, &rect);

    SDL_SetRenderDrawColor(render, clrbk.r, clrbk.g, clrbk.b, clrbk.a);
}

void CRenderer::FillRect(int scr_x, int scr_y, int width, int height, SDL_Color clr)
{
    CGame&          g       = CGame::Get();
    CWindow*        w       = g.Window();
    SDL_Renderer*   render  = w->GetRenderer();

    SDL_Color clrbk;
    SDL_GetRenderDrawColor(render, &clrbk.r, &clrbk.g, &clrbk.b, &clrbk.a);
    SDL_SetRenderDrawColor(render, clr.r, clr.g, clr.b, clr.a);

    int offX = g.GameOffsetX();
    int offY = g.GameOffsetY();
    scr_x = offX + scr_x;
    scr_y = w->WindowHeight() - (scr_y + height) - offY;

    SDL_Rect rect =
    {
        scr_x, scr_y, width, height
    };
    SDL_RenderFillRect(render, &rect);

    SDL_SetRenderDrawColor(render, clrbk.r, clrbk.g, clrbk.b, clrbk.a);
}
