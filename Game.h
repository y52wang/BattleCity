﻿/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <sstream>
#include "Window.h"
#include "Audio.h"
#include "Level.h"
#include "Sprites.h"
#include "Player.h"
#include "Renderer.h"
#include "Bullets.h"
#include "Editor.h"
#include "Enemy.h"
#include "Enemies.h"
#include "Menu.h"
#include "GameTimer.h"
#include "Items.h"
#include "GUI.h"
#include "Effects.h"
#include "AI/Data.h"
#include "AI/StrategyCnn.h"
#include "AI/StrategyNN.h"
#include "AI/StrategyPJW.h"
#include "AI/StrategyWL.h"

struct Mouse { int x, y; };

class CGame {
public: //Metody
    static CGame &Get() {
        static CGame Game;
        return Game;
    }

    //void Init();
    void Init();                                                //Inicjalizacja
    void ProcessEvents();                                       //Przetworzenie wejścia
    void StartGameplay();                                       //Start rozgrywki
    void SetupPlayerTwo();                                      //Ustawienie gracza 2
    void DeletePlayerTwo();                                     //Usunięcie gracza 2

    bool isEnd()            { return m_end_game; }              //Czy koniec gry
    void EndGame()          { m_end_game = true; }              //Zakończ grę
    void SetGameState       (GAME_STATE state);                 //Ustaw stan gry
    GAME_STATE GameState()  { return m_game_state; }            //Stan gry
    void SetGameLost(bool status)   { m_game_lost = status; }   //Ustaw status przegranej
    bool GameLost()         { return m_game_lost; }             //Czy przegrałeś?

    int TailSize() { return m_tail_size; }                      //Rozmiar kafelki（平铺尺寸）
    int GameOffsetX() { return m_game_offset_scr_x;}            //Offset na osi X czarnego okna (gameplay itp)
    int GameOffsetY() { return m_game_offset_scr_y;}            //-||-

    CWindow*		Window()		{ return m_Window; }        //Zwraca wskaźnik na obiekt gry
    CAudio*			Audio()			{ return m_Audio; }         // -||-
    CLevel*			Level()			{ return m_Level; }         // -||-
    CSprites*		Sprites()		{ return m_Sprites; }       // -||-
    CRenderer*		Renderer()		{ return m_Renderer; }      // -||-
    CPlayer*		Player()		{ return m_Player; }        // -||-
    CPlayer*		PlayerTwo()		{ return m_PlayerTwo; }     // -||-
    CBullets*		Bullets()		{ return m_Bullets; }       // -||-
    CEditor*		Editor()		{ return m_Editor; }        // -||-
    CEnemies*		Enemies()		{ return m_Enemies; }       // -||-
    CMenu*			Menu()			{ return m_Menu; }          // -||-
    CGameTimer*		GameTimer()		{ return m_GameTimer; }     // -||-
    CItems*			Items()			{ return m_Items; }         // -||-
    CGUI*			GUI()			{ return m_GUI; }           // -||-
    CEffects*		Effects()		{ return m_Effects; }       // -||-
	CDataManager*	DataManager()	{ return m_DataMgr; }		//
	CStrategy*		Strategy()		{ return m_Stg; }

private: //Metody
    CGame() {
        m_Window = new CWindow;             m_Audio = new CAudio;
        m_Sprites = new CSprites;           m_Level = new CLevel();
        m_Renderer = new CRenderer;         m_Player = new CPlayer;
        m_PlayerTwo = NULL;                 //Standardowo gracz 2 nie istnieje
        m_Bullets = new CBullets;           m_Editor = new CEditor;
        m_Enemies = new CEnemies;           m_Menu = new CMenu;
        m_GameTimer = new CGameTimer;       m_Items = new CItems;
        m_GUI = new CGUI;                   m_Effects = new CEffects;
		m_DataMgr = new CDataManager;

		m_Stg = new StrategyNN();
		//m_Stg = new StrategyPJW();
		//m_Stg = new StrategyWL<InfluenceMethod9>();
		//m_Stg = new StrategyWL<InfluenceMethodVerHorSquares>();

        m_end_game = false;
        //Rozmiar 1 klocka w budowaniu poziomu
        //横向单位长度
        m_tail_size = 16;

		m_tick_noinput = 0;
    }
    ~CGame() {
        delete m_Window;    m_Window    = NULL;
        delete m_Audio;     m_Audio     = NULL;
        delete m_Level;     m_Level     = NULL;
        delete m_Renderer;  m_Renderer  = NULL;
        delete m_Player;    m_Player    = NULL;
        delete m_PlayerTwo; m_PlayerTwo = NULL;
        delete m_Bullets;   m_Bullets   = NULL;
        delete m_Editor;    m_Editor    = NULL;
        delete m_Enemies;   m_Enemies   = NULL;
        delete m_Menu;      m_Menu      = NULL;
        delete m_GameTimer; m_GameTimer = NULL;
        delete m_Items;     m_Items     = NULL;
        delete m_GUI;       m_GUI       = NULL;
        delete m_Effects;   m_Effects   = NULL;
        delete m_Sprites;   m_Sprites   = NULL;
		delete m_DataMgr;	m_DataMgr	= NULL;
		delete m_Stg;		m_Stg		= NULL;
    }

private:
    //Pola
    int ticks_o, ticks_n;   //Obliczanie czasu
    int fps_count;          //FPSy
    int fps_max;            //Max FPS
    double fps_time;        //Czas liczenia FPSów
    double delta_time;      //Delta czasu

    bool m_game_lost;       //Czy przegrales?
    bool m_end_game;        //Zakończyć grę
    int m_tail_size;        //Rozmiar pojedyńczego klocka w œwiecie w pikselach (odnosi siê do budowy poziomu)

    GAME_STATE m_game_state;//Stan gry -> menu, gra, edytor itp.
    Mouse m_mouse;          //Dane o myszce

    int m_game_offset_scr_x;//Pozycja okna gry głównej
    int m_game_offset_scr_y;//Pozycja okna gry głównej

	bool	m_GSGP_noinput;  // GS_GAMEPLAY 模式下，没有任何按键输入
	int		m_tick_noinput;  // GS_GAMEPLAY 模式下，没有任何按键输入的时长

    //Wskaznki na główne obiekty gry
    CWindow*		m_Window;		//Obiekt okna (w SDL)
    CAudio*			m_Audio;		//Obiekt audio (muzyka, dźwięki)
    CLevel*			m_Level;		//Obiekt aktualnego poziomu w SDL
    CSprites*		m_Sprites;		//Obiekt sprite'ów (mapa wszystkich możliwych sprite'ów na podstawie której określamy info dot. rysowanego sprite'a.
    CRenderer*		m_Renderer;		//Obiekt renderer, którym rysuje wszystkie obiekty.
    CPlayer*		m_Player;		//Obiekt pierwszego gracza
    CPlayer*		m_PlayerTwo;	//Obiekt drugiego gracza
    CBullets*		m_Bullets;		//Obiekt pociskow
    CEditor*		m_Editor;		//Obiekt edytora
    CEnemies*		m_Enemies;		//Obiekt przeciwnikow
    CMenu*			m_Menu;			//Obiekt menu
    CGameTimer*		m_GameTimer;	//Obiekt timer'a gry
    CItems*			m_Items;		//Obiekt item'ów
    CGUI*			m_GUI;			//Obiekt GUI
    CEffects*		m_Effects;		//Obiekt efektów
	CDataManager*	m_DataMgr;
	CStrategy*		m_Stg;
};

/* Funkcja sprawdzająca kolizję między 2 prostokątami */
// 函数检查两个矩形之间的碰撞（上面是波兰语）
inline bool TwoRectangles(double x1, double y1, double x2, double y2,
	double x3, double y3, double x4, double y4) {
    if(x1 > x3) {
        swap(x1, x3); swap(y1, y3);
        swap(x2, x4); swap(y2, y4);
    }
    if (y1>y2) swap(y1, y2);
    if (y3>y4) swap(y3, y4);
    if (x1>x2) swap(x1, x2);
    if (x3>x4) swap(x3, x4);

    if(y1>y3) { swap(y1,y3); swap(y2,y4); }
    if(y1==y3 && y2>y4) swap(y2,y4);

    if(x3<x2 && y3<y2) {
        return true;
    }
    return false;
}

/* Spr. długości cyfry */
inline int intlen(double start) {           //Automatyczna konwersja na typ double ;)
   int end = 0;
   while(start >= 1) {
      start = start/10;
      end++;
   }
   return end;
}

/* Konwersja na string */
inline string ToString(double whatever) {
    ostringstream o;
    o << whatever;
    return o.str();
}

// 四舍五入
inline int round_double(double num) {
	return num>0.0 ? num+0.5 : num-0.5;
}

#endif // GAME_H_INCLUDED
