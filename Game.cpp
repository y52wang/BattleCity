/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#include "stdafx.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <string>
#include "Game.h"
using namespace std;

void CGame::StartGameplay()
{
	Player()->SetPosition(8, 0);
	Player()->SetAlive(false);
	Player()->SetDirection(DIR_UP);
	Player()->SetState(PLAYER_STOP);
	Player()->SetXVelocity(0);
	Player()->SetYVelocity(0);

	if (PlayerTwo() != NULL)
	{
		PlayerTwo()->SetPosition(16, 0);
		PlayerTwo()->SetAlive(false);
		PlayerTwo()->SetDirection(DIR_UP);
		PlayerTwo()->SetState(PLAYER_STOP);
		PlayerTwo()->SetXVelocity(0);
		PlayerTwo()->SetYVelocity(0);
	}
}

void CGame::SetupPlayerTwo() {
	m_PlayerTwo = new CPlayer;
	PlayerTwo()->Init(16, 0, 2);
}

void CGame::DeletePlayerTwo() {
	if (m_PlayerTwo!=NULL) {
		delete m_PlayerTwo;
		m_PlayerTwo = NULL;
	}
}

void CGame::Init() {
	// 类与类之间有先后依赖关系的初始化，放在这里
	Window()->Init();
	Audio()->Init();
	Renderer()->Init();
	Level()->Init();
	Player()->Init(8, 0, 1);
	Menu()->Init();

	m_game_offset_scr_x = 32;  //Stała
	m_game_offset_scr_y = 16;  //Stała
	m_game_state = GS_MENU;    //Jak rozpoczyna - od razu w grze czy może w menu/edytorze
	delta_time = 0.0;
	cout << "\n\n";


	double time_step    = 1.0/60;  //Częstotliwość odswieżania fizyki/logiki 
	double accumulator  = 0.0;
	double max_time     = 1.0;

	ticks_o = SDL_GetTicks();

	// 游戏主循环
	while(!m_end_game) {
		// WangLiang:
		// 这边记录整个输入环境作为 Input
		if (m_game_state==GS_GAMEPLAY && !isEnd())
		{
			DataManager()->BeginLog();
			Player()->LogData(DataManager());
			Enemies()->LogData(DataManager());
			Bullets()->LogData(DataManager());
		}

		// 首先处理游戏输入
		switch(m_game_state) {
			case GS_MENU:
				Menu()->ProcessEvents();
				break;

			case GS_STAGESELECT:
				GameTimer()->ProcessEvents();
				break;

			case GS_GAMEPLAY:
				ProcessEvents();
				break;

			case GS_EDITOR:
				ProcessEvents();
				break;
		}

		//Frame Processing
		ticks_n		= SDL_GetTicks();
		delta_time	= double((ticks_n-ticks_o))/1000.0;
		ticks_o		= ticks_n;
		if (delta_time<0)  delta_time = 0;
		
		accumulator += delta_time;
		if (accumulator<0)				accumulator = 0;
		else if (accumulator>max_time)	accumulator = max_time;

		// 结束整个输入环境的记录
		if (m_game_state==GS_GAMEPLAY && !isEnd()) {
			CDataManager* dm = DataManager();
			dm->EndLog(accumulator);

			// 如果开启了实时策略运用，则此处使用策略
			if (Player()->Alive() && !GameLost())
			{
				if (m_Stg->valid)
				{
					// 使用策略控制 Player
					OutputData od = m_Stg->MakeDecision(dm->m_InputData);
					switch (od.mov) {
					case DIR_UP:	Player()->SetDirection(DIR_UP);		Player()->Drive();  break;
					case DIR_DOWN:	Player()->SetDirection(DIR_DOWN);	Player()->Drive();  break;
					case DIR_LEFT:	Player()->SetDirection(DIR_LEFT);	Player()->Drive();  break;
					case DIR_RIGHT:	Player()->SetDirection(DIR_RIGHT);	Player()->Drive();  break;
					case DIR_NONE:	Player()->Stop(Player()->GetDirection());  break;
					}
				}
			}
		}

		//FPS Counter
		fps_time += delta_time;
		++fps_count;
		if (fps_time>=1.0) {  //2.0 -> im więcej, tym stabilniejszy licznik FPS（FPS 计数器越稳定）
			int fps_num = fps_count/double(fps_time);
			ostringstream o;
			o << Window()->WindowTitle() << " | FPS: " << fps_num;

			SDL_SetWindowTitle(CGame::Get().Window()->GetWindow(), o.str().c_str());

			//cout << "FPS: " << fps_num << "\n";
			fps_count = 0;
			fps_time = 0;
		}

		//Audio
		if ((Player()->OnMove() && PlayerTwo()==NULL) || (Player()->OnMove() && PlayerTwo()->OnMove())) {
			Audio()->PlayChunk(SOUND_ONMOVE);
		} else {
			Audio()->StopChunk(SOUND_ONMOVE);
		}

		while (accumulator>time_step)
		{
			if (m_game_state==GS_GAMEPLAY)
			{
				GameTimer()->Update(time_step);
				if (Player()->Alive())
					Player()->Update(time_step);
				if (PlayerTwo()!=NULL && PlayerTwo()->Alive())
					PlayerTwo()->Update(time_step);
				if (Items()->GetCurrentItem().destroyed == false)
					Items()->Update(time_step);
				Enemies()->Update(time_step);
				Bullets()->Update(time_step);
				Effects()->Update(time_step);
			} else if (m_game_state == GS_STAGESELECT) {
				GameTimer()->Update(time_step);
			}
			accumulator -= time_step;
		}

		//Rendering
		Renderer()->StartRendering();  // 渲染开始 ----------------------------------------------------
		switch(m_game_state) {
			case GS_MENU:
				Menu()->DrawMenu();
				break;
		
			case GS_STAGESELECT:
				GameTimer()->DrawStageSelect();
				break;
		
			case GS_GAMEPLAY:
				Level()->DrawBackground();  //Rysuje tło (czarny kwadrat)
				Level()->DrawLevel(0);  //Rysuje spód poziomu (dolne warstwy jak 'slide' itp po których można jeździć
		
				if (Player()->Alive())  Player()->Draw();  //Rysuje gracza 1
				if (PlayerTwo()!=NULL && PlayerTwo()->Alive())
					PlayerTwo()->Draw();  //Rysuje gracza 2
				Enemies()->Draw();  //Rysuje przeciwników
				Bullets()->Draw();  //Rysuje pociski
				Effects()->Draw();
		
				Level()->DrawLevel(1);  //Rysuje górne warstwy poziomu (np. krzaki)
		
				if (Items()->GetCurrentItem().destroyed == false && Items()->GetCurrentItem().is_visible)
					Items()->Draw();  //Rysuje przedmiot (do zdobycia, np. życie, gwiazdkę)
		
				GUI()->DrawGameplayStats();  //Rysuje statystyki w trybie gry

				DataManager()->Draw();
				Strategy()->Draw();
				break;
		
			case GS_EDITOR:
				Level()->DrawBackground();
				Level()->DrawLevel(0);

				Editor()->DrawEditor();
		
				Level()->DrawLevel(1);
				break;
		}
		Renderer()->StopRendering();  // 渲染结束 -----------------------------------------------------
	}
	SDL_Quit();
}

void CGame::SetGameState(GAME_STATE state) {
    if(m_game_state == GS_GAMEPLAY) {
        Bullets()->DestroyAllBullets();
        Enemies()->DestroyAllEnemies();
        Audio()->StopAll();
    }
    if (state == GS_GAMEPLAY) StartGameplay();
    else if(state == GS_MENU) {
        SetGameLost(false);
        Player()->SetPlayerLevel(0);
        if(PlayerTwo() != NULL) PlayerTwo()->SetPlayerLevel(0);
        GameTimer()->SetSelectedLevel(1);
        Level()->SetLevelNum(0);
    }
    m_game_state = state;
}

void CGame::ProcessEvents()
{
    if (isEnd()) return;

    SDL_Event event;
    while (SDL_PollEvent(&event))
	{
        if(event.type == SDL_QUIT)
		{
            m_end_game = true;
        }
		else if(event.type == SDL_KEYDOWN)
		{
			CDataManager* dm = DataManager();
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                SetGameState(GS_MENU);
            } else if(event.key.keysym.sym == SDLK_UP && Player()->Alive() && !GameLost()) {
                Player()->SetDirection(DIR_UP);
                Player()->Drive();

				dm->LogPlayerMove(DIR_UP);
				dm->ForceLog();
            } else if(event.key.keysym.sym == SDLK_LEFT && Player()->Alive() && !GameLost()) {
                Player()->SetDirection(DIR_LEFT);
                Player()->Drive();

				dm->LogPlayerMove(DIR_LEFT);
				dm->ForceLog();
            } else if(event.key.keysym.sym == SDLK_RIGHT && Player()->Alive() && !GameLost()) {
                Player()->SetDirection(DIR_RIGHT);
                Player()->Drive();

				dm->LogPlayerMove(DIR_RIGHT);
				dm->ForceLog();
            } else if(event.key.keysym.sym == SDLK_DOWN && Player()->Alive() && !GameLost()) {
                Player()->SetDirection(DIR_DOWN);
                Player()->Drive();

				dm->LogPlayerMove(DIR_DOWN);
				dm->ForceLog();
            } else if(event.key.keysym.sym == SDLK_SPACE && Player()->Alive() && !GameLost()) {
                Player()->Shoot();

				dm->LogPlayerShoot(true);
				dm->ForceLog();
            } else if(event.key.keysym.sym == SDLK_w && PlayerTwo() != NULL && PlayerTwo()->Alive() && !GameLost()) {
                PlayerTwo()->SetDirection(DIR_UP);
                PlayerTwo()->Drive();
            } else if(event.key.keysym.sym == SDLK_a && PlayerTwo() != NULL && PlayerTwo()->Alive() && !GameLost()) {
                PlayerTwo()->SetDirection(DIR_LEFT);
                PlayerTwo()->Drive();
            } else if(event.key.keysym.sym == SDLK_d && PlayerTwo() != NULL && PlayerTwo()->Alive() && !GameLost()) {
                PlayerTwo()->SetDirection(DIR_RIGHT);
                PlayerTwo()->Drive();
            } else if(event.key.keysym.sym == SDLK_s && PlayerTwo() != NULL && PlayerTwo()->Alive() && !GameLost()) {
                PlayerTwo()->SetDirection(DIR_DOWN);
                PlayerTwo()->Drive();
            } else if(event.key.keysym.sym == SDLK_LCTRL && PlayerTwo() != NULL && PlayerTwo()->Alive() && !GameLost()) {
                PlayerTwo()->Shoot();
            } else if(event.key.keysym.sym == SDLK_F5) {
                if(m_game_state == GS_EDITOR)
                    Level()->SaveLevel();
			} else if(event.key.keysym.sym == SDLK_g) {
				CLevel* level = CGame::Get().Level();
				level->m_drawGrid = !level->m_drawGrid;
			}
        }
		else if(event.type == SDL_KEYUP)
		{
            if(event.key.keysym.sym == SDLK_UP) {
                Player()->Stop(DIR_UP);
            } else if(event.key.keysym.sym == SDLK_LEFT) {
                Player()->Stop(DIR_LEFT);
            } else if(event.key.keysym.sym == SDLK_RIGHT) {
                Player()->Stop(DIR_RIGHT);
            } else if(event.key.keysym.sym == SDLK_DOWN) {
                Player()->Stop(DIR_DOWN);
            } else if(event.key.keysym.sym == SDLK_w && PlayerTwo() != NULL) {
                PlayerTwo()->Stop(DIR_UP);
            } else if(event.key.keysym.sym == SDLK_a && PlayerTwo() != NULL) {
                PlayerTwo()->Stop(DIR_LEFT);
            } else if(event.key.keysym.sym == SDLK_d && PlayerTwo() != NULL) {
                PlayerTwo()->Stop(DIR_RIGHT);
            } else if(event.key.keysym.sym == SDLK_s && PlayerTwo() != NULL) {
                PlayerTwo()->Stop(DIR_DOWN);
            }
        }
		else if(event.type == SDL_MOUSEBUTTONDOWN)
		{
            if(event.button.button == SDL_BUTTON_LEFT)
			{
                if(m_game_state == GS_EDITOR)
				{
                    Editor()->LMB(m_mouse.x, m_mouse.y);
                }
            }
			else if(event.button.button == SDL_BUTTON_RIGHT)
			{
                if(m_game_state == GS_EDITOR)
				{
                    Editor()->RMB(m_mouse.x, m_mouse.y);
                }
            }
        }
		else if(m_game_state == GS_EDITOR && event.type == SDL_MOUSEMOTION)
		{
            m_mouse.x = event.motion.x;
            m_mouse.y = event.motion.y;
        }
    }
}
