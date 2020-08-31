#include "stdafx.h"
#include "StrategyWL.h"
#include "Game.h"
#include <algorithm>

using namespace MiniDNN;

#define REGION_CNT	8	// �ܵ�����
#define ACTION		5	// �ƶ� 5 ����Shoot 1 ��
#define HIDDEN		16	// ���㵥Ԫ����

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

// 9 ��������ͼ���������� ---------------------------------------------------------------
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
			if (idx==4)  continue;  // �Լ���ռλ�ò�����

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
			if (idx==4)  continue;  // �Լ���ռλ�ò�����

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

	// �����Ϊ���ģ���������ͼ�Ļ���
	// Ҳ�кܶ��ֻ�����ÿһ�ֶ��ܳ���Ϊ�����������
	// ���¶�������Ϸ��ֱ�ӻ���

//	// ��һ�֣��� 24 �� Region
//	// �� 4 ������
//	r->DrawLine(0,	pos_y-ph,			lvl->LevelWidth()*ts,	pos_y-ph,		r->_yellow);
//	r->DrawLine(0,	pos_y,				lvl->LevelWidth()*ts,	pos_y,			r->_yellow);
//	r->DrawLine(0,	pos_y+ph,			lvl->LevelWidth()*ts,	pos_y+ph,		r->_yellow);
//	r->DrawLine(0,	pos_y+2*ph,			lvl->LevelWidth()*ts,	pos_y+2*ph,		r->_yellow);
//
//	// �� 4 ������
//	r->DrawLine(pos_x-pw,		0,		pos_x-pw,		lvl->LevelHeight()*ts,	r->_yellow);
//	r->DrawLine(pos_x,			0,		pos_x,			lvl->LevelHeight()*ts,	r->_yellow);
//	r->DrawLine(pos_x+pw,		0,		pos_x+pw,		lvl->LevelHeight()*ts,	r->_yellow);
//	r->DrawLine(pos_x+2*pw,		0,		pos_x+2*pw,		lvl->LevelHeight()*ts,	r->_yellow);

//	// �ڶ��֣��� 8 �� Region
//	// �� 2 ������
//	r->DrawLine(0,	pos_y,				lvl->LevelWidth()*ts,	pos_y,			r->_yellow);
//	r->DrawLine(0,	pos_y+ph,			lvl->LevelWidth()*ts,	pos_y+ph,		r->_yellow);
//
//	// �� 2 ������
//	r->DrawLine(pos_x,			0,		pos_x,			lvl->LevelHeight()*ts,	r->_yellow);
//	r->DrawLine(pos_x+pw,		0,		pos_x+pw,		lvl->LevelHeight()*ts,	r->_yellow);

	if (dm->m_IODataVec.empty())  return;

	const IOData&		iodata	= dm->m_IODataVec.back();
	const InputData&	id		= iodata.first;
	const OutputData&	od		= iodata.second;

	InputData nid = id.Normalize();
	float ary[9];
	CalcInfluence9(nid, ary);

	// ������ player Ϊ���ĵ�����ͼ
	int cx = 700;  // �������½� x ����
	int cy = 286;  // �������½� y ����
	
	// �� �Լ� ���ڵķ����Դ�Ϊ���ģ�
	r->DrawRect(cx, cy, 4*2, 4*2, r->_yellow);

	// �Ź������Ϸ������ 0��
	r->DrawRect(cx-4*12, cy+4*2, 4*12, 4*12, r->_yellow);
	r->FillRect(cx-4*12+1, cy+4*2+1, 4*12-1, 4*12-1, Fade(r->_red, ary[0]) );
	// �Ź����Ϸ������ 1��
	r->DrawRect(cx, cy+4*2, 4*2, 4*12, r->_yellow);
	r->FillRect(cx+1, cy+4*2+1, 4*2-1, 4*12-1, Fade(r->_red, ary[1]) );
	// �Ź������Ϸ������ 2��
	r->DrawRect(cx+4*2, cy+4*2, 4*12, 4*12, r->_yellow);
	r->FillRect(cx+4*2+1, cy+4*2+1, 4*12-1, 4*12-1, Fade(r->_red, ary[2]) );

	// �Ź����󷽣���� 3��
	r->DrawRect(cx-4*12, cy, 4*12, 4*2, r->_yellow);
	r->FillRect(cx-4*12+1, cy+1, 4*12-1, 4*2-1, Fade(r->_red, ary[3]) );
	// �Ź����ҷ������ 5��
	r->DrawRect(cx+4*2, cy, 4*12, 4*2, r->_yellow);
	r->FillRect(cx+4*2+1, cy+1, 4*12-1, 4*2-1, Fade(r->_red, ary[5]) );

	// �Ź������·������ 6��
	r->DrawRect(cx-4*12, cy-4*12, 4*12, 4*12, r->_yellow);
	r->FillRect(cx-4*12+1, cy-4*12+1, 4*12-1, 4*12-1, Fade(r->_red, ary[6]) );
	// �Ź����·������ 7��
	r->DrawRect(cx, cy-4*12, 4*2, 4*12, r->_yellow);
	r->FillRect(cx+1, cy-4*12+1, 4*2-1, 4*12-1, Fade(r->_red, ary[7]) );
	// �Ź������·������ 8��
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
