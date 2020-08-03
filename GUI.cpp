/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#include "GUI.h"
#include "Game.h"
using namespace std;

void CGUI::DrawGameplayStats() {
  //Rysowanie ilosci przeciwników
  int x = 27;
  int y = 25;
  int ts = CGame::Get().TailSize();
  SpriteData EnemyTank = CGame::Get().Sprites()->Get("enemy_stat");
  SpriteData PlayerTank = CGame::Get().Sprites()->Get("player_stat");
  SpriteData Flag = CGame::Get().Sprites()->Get("flag");

  for (int i=0; i<MAX_ENEMY_COUNT-CGame::Get().Enemies()->Enemies(); ++i) {
    if (i%2 == 0)
      CGame::Get().Renderer()->DrawSprite(EnemyTank, 0, x*ts, int(y-(i/2))*ts, EnemyTank.width, EnemyTank.height);
    else
      CGame::Get().Renderer()->DrawSprite(EnemyTank, 0, (x+1)*ts, int(y-(i/2))*ts, EnemyTank.width, EnemyTank.height);
  }

  //Statystyki pierwszego gracza
  string lifes = "0";
  if (CGame::Get().Player()->Lifes()>0)
    lifes = ToString(CGame::Get().Player()->Lifes() );

  RenderText("IP", x*ts, 10*ts, COLOR_BLACK);
  CGame::Get().Renderer()->DrawSprite(PlayerTank, 0, x*ts, 9*ts, PlayerTank.width, PlayerTank.height);
  RenderText(lifes, (x+1)*ts, 9*ts, COLOR_BLACK);

  if (CGame::Get().PlayerTwo() != NULL) {
    //Statystyki drugiego gracza
    lifes = "0";
    if (CGame::Get().PlayerTwo()->Lifes() > 0)  lifes = ToString(CGame::Get().PlayerTwo()->Lifes() );

    RenderText("IIP", x*ts, 7*ts, COLOR_BLACK);
    CGame::Get().Renderer()->DrawSprite(PlayerTank, 0, x*ts, 6*ts, PlayerTank.width, PlayerTank.height);
    RenderText(lifes, (x+1)*ts, 6*ts, COLOR_BLACK);
  }

  CGame::Get().Renderer()->DrawSprite(Flag, 0, x*ts, 3*ts, Flag.width, Flag.height);
  RenderText(ToString(CGame::Get().GameTimer()->ShowedLevel()), (x+1)*ts, 2*ts, COLOR_BLACK);

  if (CGame::Get().GameLost() ) {
    RenderText("GAME", 11*ts, 14*ts, COLOR_RED);
    RenderText("OVER", 11*ts, 13*ts, COLOR_RED);
  }
}

void CGUI::RenderText(const string& text, double x, double y, COLOR color) {
  SpriteData chars = CGame::Get().Sprites()->Get("chars");
  SpriteData signs = CGame::Get().Sprites()->Get("signs");
  int pixels_per_one_sign = max(chars.width, signs.width);

  //Rysowanie
  int temp;
  for (int i=0; i<(int)text.length(); ++i) {
    temp = int(text[i]);
    if(temp >= 65 && temp <=90) {               //Drukowane litery
      CGame::Get().Renderer()->DrawSprite(chars, temp-65, x + i*pixels_per_one_sign, y, chars.width, chars.height, color);
    } else if(temp >= 48 && temp <= 57) {       //Cyfry
      CGame::Get().Renderer()->DrawSprite(signs, temp-48, x + i*pixels_per_one_sign, y, chars.width, chars.height, color);
    }
  }
}
