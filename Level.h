/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <string>
#include <vector>
#include <fstream>
using namespace std;

#include "Types.h"

struct sLevel {
  vector < vector <LVL_FIELD> > data;
  int m_level_num;
};

struct sBrick {
  sBrick()
    : top_left(false), top_right(false)
    , bottom_left(false), bottom_right(false)
  { }

  void clear() {
    top_left = top_right = bottom_left = bottom_right = true;
  }

  bool top_left;
  bool top_right;
  bool bottom_left;
  bool bottom_right;
};

class CSprites;
struct SpriteData;

class CLevel {
  public:
    CLevel(CSprites* sprites);

    void LoadLevel(const string fileName);
    void LoadNextLevel()  {}
    void DrawLevel(bool up);
    void DrawBackground();
    void DestroyTile(int x1, int y1, int x2, int y2, int power, DIRECTION dir);

    void DumpLevel()  {}  //Dump do konsoli
    void SaveLevel();   //Zapis do pliku

    int LevelNum() { return m_level.m_level_num; }
    //int LevelWidth() { return m_level_width; }
    //int LevelHeight() { return m_level_height; }
    int AllLevels()     { return m_all_levels; }

    void SetLevelField(int x, int y, LVL_FIELD type)    { m_level.data[y][x] = type; }
    void SetLevelNum(int num)                           { m_level.m_level_num = num; }
    LVL_FIELD LevelField(int x, int y)                  { return m_level.data[y][x]; }
    sBrick BrickField(int x, int y)                     { return m_bricks[y][x]; }
    void SetBrickTopLeft(int x, int y, bool value)      { m_bricks[y][x].top_left = value; }
    void SetBrickTopRight(int x, int y, bool value)     { m_bricks[y][x].top_right = value; }
    void SetBrickBottomLeft(int x, int y, bool value)   { m_bricks[y][x].bottom_left = value; }
    void SetBrickBottomRight(int x, int y, bool value)  { m_bricks[y][x].bottom_right = value; }
    void SetClearBrick(int x, int y)                    { m_bricks[y][x].clear(); }

  private:
    int m_level_width;
    int m_level_height;

    int m_all_levels;

    sLevel m_level;
    vector< vector<sBrick> > m_bricks;

    SpriteData*   m_SD_Brick;
    SpriteData*   m_SD_White;
    SpriteData*   m_SD_Slide;
    SpriteData*   m_SD_Bush;
    SpriteData*   m_SD_Water;
    SpriteData*   m_SD_Eagle;
    SpriteData*   m_SD_BG;

    SpriteData* GetSpriteData(const LVL_FIELD lf);
};

#endif // LEVEL_H_INCLUDED
