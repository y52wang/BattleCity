/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#include "stdafx.h"
#include <SDL2/SDL.h>
#include <iostream>
#include "Game.h"
#include "Menu.h"
#include "Renderer.h"

#include "AI/Data.h"
#include "AI/StrategyCnn.h"

using namespace std;

void CMenu::Init() {
    m_Background    = CGame::Get().Sprites()->Get("lvl_background");
    m_Logo          = CGame::Get().Sprites()->Get("menu_logo");
    m_MenuOptions   = CGame::Get().Sprites()->Get("menu_options");
    m_Pointer       = CGame::Get().Sprites()->Get("player_one_right_0");
}

void CMenu::DrawMenu() {
	CGame& game = CGame::Get();
    CRenderer* render = game.Renderer();

    //Czarne tło（黑色背景）
    //Wymiary całego okna（全窗口尺寸）
    render->DrawSprite(m_Background,   0,        -32,    -16,    640,                    480);

    render->DrawSprite(m_Logo,         0,        110,    260,    m_Logo.width,           m_Logo.height);

    render->DrawSprite(m_MenuOptions,  0,        232,    150,    m_MenuOptions.width,    m_MenuOptions.height);

    switch(m_selected_item) {
        case 1:
            //if (m_EnableLog)  // 数据记录 开启 时，绘制填充矩形
            //    render->FillRect(190, 205, m_Pointer.width, m_Pointer.height, render->_red);
            render->DrawSprite(m_Pointer, 0, 190, 205, m_Pointer.width, m_Pointer.height);
			if (game.DataManager()->IsEnableLog())  // 数据记录 开启 时，绘制矩形框
                render->DrawRect(190, 205, m_Pointer.width, m_Pointer.height, render->_red);
            break;
        case 2:
            render->DrawSprite(m_Pointer, 0, 190, 173, m_Pointer.width, m_Pointer.height);
			if (game.DataManager()->IsEnableLog())  // 数据记录 开启 时，绘制矩形框
				render->DrawRect(190, 173, m_Pointer.width, m_Pointer.height, render->_red);
            break;
        case 3:
            render->DrawSprite(m_Pointer, 0, 190, 140, m_Pointer.width, m_Pointer.height);
            break;
    }

    CGame::Get().GUI()->RenderText("RIPPER", 500, 0);
}

void CMenu::ChooseItem() {
    switch(m_selected_item) {
        case 1:
            CGame::Get().SetGameState(GS_STAGESELECT);
            CGame::Get().Player()->SetLifes(2);
            CGame::Get().Player()->Born();
            CGame::Get().DeletePlayerTwo();
            break;
        case 2:
            CGame::Get().SetGameState(GS_STAGESELECT);
            CGame::Get().SetupPlayerTwo();
            CGame::Get().Player()->SetLifes(2);
            CGame::Get().PlayerTwo()->SetLifes(2);
            CGame::Get().Player()->Born();
            CGame::Get().PlayerTwo()->Born();
            break;
        case 3:
            CGame::Get().SetGameState(GS_EDITOR);
            CGame::Get().Level()->LoadLevel("data/levels/temp.lvl");
            break;
    }
}

void CMenu::ProcessEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
		CGame& game = CGame::Get();

        if (event.type == SDL_QUIT) {
            game.EndGame();
        } else if (event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_ESCAPE) {
                game.EndGame();
            } else if (event.key.keysym.sym == SDLK_UP) {
                int i = SelectedItem() - 1;
                if(i==0) i = 3;
                SelectItem(i);
            } else if (event.key.keysym.sym == SDLK_DOWN) {
                int i = SelectedItem() + 1;
                if (i==4) i = 1;
                SelectItem(i);
            } else if (event.key.keysym.sym == SDLK_RETURN) {
                ChooseItem();
            } else if (event.key.keysym.sym == SDLK_SPACE) {
                int i = SelectedItem();
                if (i==1 || i==2) {
					CDataManager* dm = game.DataManager();
					dm->EnableLog(!dm->IsEnableLog());
				}
            } else if (event.key.keysym.sym == SDLK_t) {  // 按下 字母 t 键
                // 读取数据，进行卷积网络的训练
				CDataManager* dm = game.DataManager();
				dm->Load((event.key.keysym.mod&KMOD_LSHIFT) ? "data/IOData.dat" : "data/IOData_Balance.dat");
				game.Strategy()->Train(dm->m_IODataVec, "data", "Stg");
            } else if (event.key.keysym.sym == SDLK_s) {  // 按下 字母 s 键
				// 保存数据
				game.DataManager()->Save("data/IOData.dat");
				game.DataManager()->DumpTrainDataToCSV("data/TrainData.csv", game.Strategy() );
			} else if (event.key.keysym.sym == SDLK_l) {  // 按下 字母 L 键
				// 载入CNN参数
				game.Strategy()->LoadParameters("data", "Stg");
			} else if (event.key.keysym.sym == SDLK_b) {
				game.DataManager()->DataBalance("data/IOData.dat", "data/IOData_Balance.dat");
			}
        }
    }
}
