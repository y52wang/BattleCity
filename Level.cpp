/**************************************************
**************************************************/

#include <assert.h>
#include "Level.h"
#include "Game.h"

CLevel::CLevel()
	: m_level_width(26)
	, m_level_height(26)
	, m_all_levels(20)

    , m_SD_Brick(NULL)
    , m_SD_White(NULL)
    , m_SD_Slide(NULL)
    , m_SD_Bush(NULL)
    , m_SD_Water(NULL)
    , m_SD_Eagle(NULL)
    , m_SD_BG(NULL)
{
}

void CLevel::Init()
{
    CSprites* sprites = CGame::Get().Sprites();

    m_SD_Brick  = &(sprites->Get("lvl_brick") );
    m_SD_White  = &(sprites->Get("lvl_white") );
    m_SD_Slide  = &(sprites->Get("lvl_slide") );
    m_SD_Bush   = &(sprites->Get("lvl_bush") );
    m_SD_Water  = &(sprites->Get("lvl_water") );
    m_SD_Eagle  = &(sprites->Get("lvl_eagle") );
    m_SD_BG     = &(sprites->Get("lvl_background") );
}

/*
  bin\data\levels 目录下 lvl 文件格式
  第 1 行：关卡号
  第 2-27 行（共 26 行）：地图数据，每行 26 个数字对应 LVL_FIELD
  第 28 行：一个数字 -1，表示敌方坦克类型 ENEMY_TYPE 随机出现
            否则二十个数字，表示敌方坦克的类型
*/
void CLevel::LoadLevel(const string fileName)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	assert(fp);

	m_level.data.clear();
	m_bricks.clear();

	fscanf(fp, "%d", &m_level.m_level_num);
	for (int i=0; i<m_level_height; ++i)  {
		vector<LVL_FIELD>	vecLF;
		vector<sBrick>		vecB;
		for (int j=0; j<m_level_width; ++j)  {
			int d = 0;
			fscanf(fp, "%d", &d);
			vecLF.push_back((LVL_FIELD)d);
			vecB.push_back(sBrick() );
		}
		m_level.data.push_back(vecLF);
		m_bricks.push_back(vecB);
	}
  reverse(m_level.data.begin(), m_level.data.end() );
  reverse(m_bricks.begin(), m_bricks.end() );

  CEnemies* enemies = CGame::Get().Enemies();
  int et = -1;
  fscanf(fp, "%d", &et);
  if (et==-1) {
    for (int i=0; i<MAX_ENEMY_COUNT; ++i) {
      et = rand() % ENEMY_COUNT;
      enemies->SetEnemyType(i, (ENEMY_TYPE)et);
    }
  } else {
    enemies->SetEnemyType(0, (ENEMY_TYPE)et);
    for (int i=1; i<MAX_ENEMY_COUNT; ++i) {
      fscanf(fp, "%d", &et);
      enemies->SetEnemyType(i, (ENEMY_TYPE)et);
    }
  }

	fclose(fp);
}

// level 分两层，up 用于控制绘制顺序
void CLevel::DrawLevel(bool up)
{
  auto filter = [up](LVL_FIELD lf) {
    return up ? lf==LVL_BUSH : lf!=LVL_BUSH;
  };

  int ts = CGame::Get().TailSize();
	for (size_t i=0; i<m_level.data.size(); ++i) {
		for (size_t j=0; j<m_level.data[i].size(); ++j) {
      double pos_x = j * ts;
      double pos_y = i * ts;

      LVL_FIELD lf = m_level.data[i][j];
      if (filter(lf) ) {
        SpriteData* sd = GetSpriteData(lf);
        if (sd!=NULL)
          CGame::Get().Renderer()->DrawSprite(*sd, 0, pos_x, pos_y, ts, ts);
      }
		}
	}
}

// 绘制黑色背景
void CLevel::DrawBackground()
{
  int ts = CGame::Get().TailSize();
  for (int i=0; i<m_level_height; ++i) {
    for (int j=0; j<m_level_width; ++j) {
      double pos_x = j * ts;
      double pos_y = i * ts;
      CGame::Get().Renderer()->DrawSprite(*m_SD_BG, 0, pos_x, pos_y, ts, ts);
    }
  }
}

void CLevel::DestroyTile(int x1, int y1, int x2, int y2, int power, DIRECTION dir)
{
}

void CLevel::SaveLevel()
{
}

SpriteData* CLevel::GetSpriteData(const LVL_FIELD lf) {
  switch (lf) {
  case LVL_BRICK:
    return m_SD_Brick;
  case LVL_WHITE:
    return m_SD_White;
  case LVL_SLIDE:
    return m_SD_Slide;
  case LVL_BUSH:
    return m_SD_Bush;
  case LVL_WATER:
    return m_SD_Water;
  case LVL_EAGLE:
    return m_SD_Eagle;
  }
  return NULL;
}
