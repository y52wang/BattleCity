#include "stdafx.h"
#include "StrategyWL.h"
#include "Game.h"
#include <algorithm>

using namespace MiniDNN;

#define REGION_CNT	8	// 总的区域
#define ACTION		5	// 移动 5 个，Shoot 1 个
#define HIDDEN		16	// 隐层单元个数

static SDL_Color Fade(SDL_Color c, float alpha)
{
	c.r *= alpha;
	c.g *= alpha;
	c.b *= alpha;

	return c;
}

static SDL_Rect GetBulletRect(const Pos& pos, const DIRECTION dir)
{
	SDL_Rect rect { pos.x, pos.y, 1, 1 };
	switch (dir)
	{
	case DIR_UP:
	case DIR_DOWN:
		rect.w += 1;
		break;

	case DIR_LEFT:
	case DIR_RIGHT:
		rect.h += 1;
		break;
	}

	return rect;
}

// 9 宫格势力图，势力计算 ---------------------------------------------------------------
static SDL_Rect GetRectInfluence9(const int idx)
{
	switch (idx)
	{
	case 0:
		return SDL_Rect { -12,   2, 12, 12 };
	case 1:
		return SDL_Rect {   0,   2,  2, 12 };
	case 2:
		return SDL_Rect {   2,   2, 12, 12 };
	case 3:
		return SDL_Rect { -12,   0, 12,  2 };
	case 4:
		return SDL_Rect {   0,   0,  2,  2 };
	case 5:
		return SDL_Rect {   2,   0, 12,  2 };
	case 6:
		return SDL_Rect { -12, -12, 12, 12 };
	case 7:
		return SDL_Rect {   0, -12,  2, 12 };
	case 8:
		return SDL_Rect {   2, -12, 12, 12 };
	default:
		assert(0);
		return SDL_Rect{ 0, 0, 0, 0 };
	}
}

static void CalcInfluence9(const InputData& nid, float (&ary)[9])
{
	memset(ary, 0, sizeof(ary) );

	for (size_t i=0; i<nid.enemies_pos.size(); ++i)
	{
		for (int idx=0; idx<9; ++idx)
		{
			if (idx==4)  continue;  // 自己所占位置不处理

			SDL_Rect region = GetRectInfluence9(idx);
			SDL_Rect rect { nid.enemies_pos[i].x, nid.enemies_pos[i].y, 2, 2 };
			bool isInter = SDL_HasIntersection(&region, &rect);
			if (!isInter)  continue;

			ary[idx] += 0.1f;
			switch (idx)
			{
			case 1:
				if (nid.enemies_dir[i]==DIR_DOWN)
					ary[idx] += 0.2f;
			case 3:
				if (nid.enemies_dir[i]==DIR_RIGHT)
					ary[idx] += 0.2f;
			case 5:
				if (nid.enemies_dir[i]==DIR_LEFT)
					ary[idx] += 0.2f;
			case 7:
				if (nid.enemies_dir[i]==DIR_UP)
					ary[idx] += 0.2f;
			}
		}
	}

	for (size_t i=0; i<nid.enemies_bullet_pos.size(); ++i)
	{
		for (int idx=0; idx<9; ++idx)
		{
			if (idx==4)  continue;  // 自己所占位置不处理

			SDL_Rect region = GetRectInfluence9(idx);
			DIRECTION dir = nid.enemies_bullet_dir[i];
			SDL_Rect rect = GetBulletRect(nid.enemies_bullet_pos[i], dir);
			bool isInter = SDL_HasIntersection(&region, &rect);
			if (!isInter)  continue;

			switch (idx)
			{
			case 0:
				if (dir==DIR_RIGHT || dir==DIR_DOWN)
					ary[idx] += 0.1f;
				break;

			case 1:
				if (dir==DIR_DOWN)
					ary[idx] += 0.3f;
				break;

			case 2:
				if (dir==DIR_LEFT || dir==DIR_DOWN)
					ary[idx] += 0.1f;
				break;

			case 3:
				if (dir==DIR_RIGHT)
					ary[idx] += 0.3f;
				break;

			case 5:
				if (dir==DIR_LEFT)
					ary[idx] += 0.3f;
				break;

			case 6:
				if (dir==DIR_RIGHT || dir==DIR_UP)
					ary[idx] += 0.1f;
				break;

			case 7:
				if (dir==DIR_UP)
					ary[idx] += 0.3f;
				break;

			case 8:
				if (dir==DIR_LEFT || dir==DIR_UP)
					ary[idx] += 0.1f;
				break;
			}
		}
	}

	for (int idx=0; idx<9; ++idx)
		ary[idx] = std::min<float>(1.0f, ary[idx]);
}

// --------------------------------------------------------------------------------------

StrategyWL::StrategyWL()
	: CStrategy(REGION_CNT, ACTION)
{
}

void StrategyWL::Draw()
{
	CGame&			game	= CGame::Get();
	CRenderer*		r		= game.Renderer();
	CLevel*			lvl		= game.Level();
	CPlayer*		p		= game.Player();
	CDataManager*	dm		= game.DataManager();

//	int			ts		= game.TailSize();
//	int			pos_x	= p->GetX() * ts;
//	int			pos_y	= p->GetY() * ts;
//	int			ph		= p->GetPlayerHeight();
//	int			pw		= p->GetPlayerWidth();

	int			lw = lvl->LevelWidth();
	int			lh = lvl->LevelHeight();

	// 以玩家为中心，进行势力图的绘制
	// 也有很多种画法，每一种都能抽象为神经网络的输入
	// 以下都是在游戏中直接绘制

//	// 第一种：共 24 个 Region
//	// 画 4 条横线
//	r->DrawLine(0,	pos_y-ph,			lvl->LevelWidth()*ts,	pos_y-ph,		r->_yellow);
//	r->DrawLine(0,	pos_y,				lvl->LevelWidth()*ts,	pos_y,			r->_yellow);
//	r->DrawLine(0,	pos_y+ph,			lvl->LevelWidth()*ts,	pos_y+ph,		r->_yellow);
//	r->DrawLine(0,	pos_y+2*ph,			lvl->LevelWidth()*ts,	pos_y+2*ph,		r->_yellow);
//
//	// 画 4 条纵线
//	r->DrawLine(pos_x-pw,		0,		pos_x-pw,		lvl->LevelHeight()*ts,	r->_yellow);
//	r->DrawLine(pos_x,			0,		pos_x,			lvl->LevelHeight()*ts,	r->_yellow);
//	r->DrawLine(pos_x+pw,		0,		pos_x+pw,		lvl->LevelHeight()*ts,	r->_yellow);
//	r->DrawLine(pos_x+2*pw,		0,		pos_x+2*pw,		lvl->LevelHeight()*ts,	r->_yellow);

//	// 第二种：共 8 个 Region
//	// 画 2 条横线
//	r->DrawLine(0,	pos_y,				lvl->LevelWidth()*ts,	pos_y,			r->_yellow);
//	r->DrawLine(0,	pos_y+ph,			lvl->LevelWidth()*ts,	pos_y+ph,		r->_yellow);
//
//	// 画 2 条纵线
//	r->DrawLine(pos_x,			0,		pos_x,			lvl->LevelHeight()*ts,	r->_yellow);
//	r->DrawLine(pos_x+pw,		0,		pos_x+pw,		lvl->LevelHeight()*ts,	r->_yellow);

	if (dm->m_IODataVec.empty())  return;

	const IOData&		iodata	= dm->m_IODataVec.back();
	const InputData&	id		= iodata.first;
	const OutputData&	od		= iodata.second;

	InputData nid = id.Normalize();
	float ary[9];
	CalcInfluence9(nid, ary);

	// 绘制以 player 为中心的势力图
	int cx = 700;  // 中心左下角 x 坐标
	int cy = 286;  // 中心左下角 y 坐标
	
	// 画 自己 所在的方格（以此为中心）
	r->DrawRect(cx, cy, 4*2, 4*2, r->_yellow);

	// 九宫格左上方（编号 0）
	r->DrawRect(cx-4*12, cy+4*2, 4*12, 4*12, r->_yellow);
	r->FillRect(cx-4*12+1, cy+4*2+1, 4*12-1, 4*12-1, Fade(r->_red, ary[0]) );
	// 九宫格上方（编号 1）
	r->DrawRect(cx, cy+4*2, 4*2, 4*12, r->_yellow);
	r->FillRect(cx+1, cy+4*2+1, 4*2-1, 4*12-1, Fade(r->_red, ary[1]) );
	// 九宫格右上方（编号 2）
	r->DrawRect(cx+4*2, cy+4*2, 4*12, 4*12, r->_yellow);
	r->FillRect(cx+4*2+1, cy+4*2+1, 4*12-1, 4*12-1, Fade(r->_red, ary[2]) );

	// 九宫格左方（编号 3）
	r->DrawRect(cx-4*12, cy, 4*12, 4*2, r->_yellow);
	r->FillRect(cx-4*12+1, cy+1, 4*12-1, 4*2-1, Fade(r->_red, ary[3]) );
	// 九宫格右方（编号 5）
	r->DrawRect(cx+4*2, cy, 4*12, 4*2, r->_yellow);
	r->FillRect(cx+4*2+1, cy+1, 4*12-1, 4*2-1, Fade(r->_red, ary[5]) );

	// 九宫格左下方（编号 6）
	r->DrawRect(cx-4*12, cy-4*12, 4*12, 4*12, r->_yellow);
	r->FillRect(cx-4*12+1, cy-4*12+1, 4*12-1, 4*12-1, Fade(r->_red, ary[6]) );
	// 九宫格下方（编号 7）
	r->DrawRect(cx, cy-4*12, 4*2, 4*12, r->_yellow);
	r->FillRect(cx+1, cy-4*12+1, 4*2-1, 4*12-1, Fade(r->_red, ary[7]) );
	// 九宫格右下方（编号 8）
	r->DrawRect(cx+4*2, cy-4*12, 4*12, 4*12, r->_yellow);
	r->FillRect(cx+4*2+1, cy-4*12+1, 4*12-1, 4*12-1, Fade(r->_red, ary[8]) );
}

void StrategyWL::SetupNetwork()
{
	Layer* layer1 = new FullyConnected<ReLU>(REGION_CNT,	HIDDEN);
	Layer* layer2 = new FullyConnected<ReLU>(HIDDEN,		HIDDEN);
	Layer* layer3 = new FullyConnected<Sigmoid>(HIDDEN,		ACTION);

	network.add_layer(layer1);
	network.add_layer(layer2);
	network.add_layer(layer3);

	network.set_output(new RegressionMSE());
}

void StrategyWL::ConvertData(const InputData& id)
{
}

void StrategyWL::ConvertData(const OutputData& od)
{
}
