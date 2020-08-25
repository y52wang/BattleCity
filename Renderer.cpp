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
    SDL_Renderer* r = CGame::Get().Window()->GetRenderer();
    SDL_RenderClear(r);
}

void CRenderer::StopRendering() {
    SDL_Renderer* r = CGame::Get().Window()->GetRenderer();
    SDL_RenderPresent(r);
    SDL_Delay(1000/60);
}

// scr_x:       screen x
// scr_y:       screen y
// CRenderer::DrawSprite 所认为的坐标系，x 向右，y 向上，原点在左下角
// SDL2 所认为的坐标系，x 向右，y 向下，原点在左上角
void CRenderer::DrawSprite(SpriteData& sprite_data, int frame,
    int scr_x, int scr_y, int width, int height,
    COLOR color)
{
    CGame&          g = CGame::Get();
    CWindow*        w = g.Window();
    SDL_Renderer*   r = w->GetRenderer();

    static int offX = g.GameOffsetX();
    static int offY = g.GameOffsetY();
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
