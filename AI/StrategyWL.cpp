#include "stdafx.h"
#include "StrategyWL.h"
#include "Game.h"
#include <algorithm>

using namespace MiniDNN;

#define ACTION		5	// �ƶ� 5 ����Shoot 1 ��
#define HIDDEN		4	// ���㵥Ԫ����

static SDL_Color Fade(SDL_Color c, float alpha)
{
	c.r *= alpha;
	c.g *= alpha;
	c.b *= alpha;

	return c;
}

static SDL_Rect GetTankRect(const Pos& pos)
{
	return SDL_Rect { pos.x, pos.y, 2, 2 };
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
const int InfluenceMethod9::_region_cnt = 9;  // �ܵ�����

void InfluenceMethod9::CalcInfluence(const InputData& id, std::vector<float>& out)
{
	InputData nid = id.Normalize();

	out.resize(_region_cnt);
	for (size_t i=0; i<out.size(); ++i)
		out[i] = 0.0f;

	for (size_t i=0; i<nid.enemies_pos.size(); ++i)
	{
		for (int idx=0; idx<_region_cnt; ++idx)
		{
			if (idx==4)  continue;  // �Լ���ռλ�ò�����

			SDL_Rect region = GetRectInfluence(idx);
			SDL_Rect rect = GetTankRect(nid.enemies_pos[i]);
			bool isInter = SDL_HasIntersection(&region, &rect);
			if (!isInter)  continue;

			out[idx] += 0.1f;
			switch (idx)
			{
			case 1:
				if (nid.enemies_dir[i]==DIR_DOWN)
					out[idx] += 0.2f;
			case 3:
				if (nid.enemies_dir[i]==DIR_RIGHT)
					out[idx] += 0.2f;
			case 5:
				if (nid.enemies_dir[i]==DIR_LEFT)
					out[idx] += 0.2f;
			case 7:
				if (nid.enemies_dir[i]==DIR_UP)
					out[idx] += 0.2f;
			}
		}
	}

	for (size_t i=0; i<nid.enemies_bullet_pos.size(); ++i)
	{
		for (int idx=0; idx<_region_cnt; ++idx)
		{
			if (idx==4)  continue;  // �Լ���ռλ�ò�����

			SDL_Rect region = GetRectInfluence(idx);
			DIRECTION dir = nid.enemies_bullet_dir[i];
			SDL_Rect rect = GetBulletRect(nid.enemies_bullet_pos[i], dir);
			bool isInter = SDL_HasIntersection(&region, &rect);
			if (!isInter)  continue;

			switch (idx)
			{
			case 0:
				if (dir==DIR_RIGHT || dir==DIR_DOWN)
					out[idx] += 0.1f;
				break;

			case 1:
				if (dir==DIR_DOWN)
					out[idx] += 0.3f;
				break;

			case 2:
				if (dir==DIR_LEFT || dir==DIR_DOWN)
					out[idx] += 0.1f;
				break;

			case 3:
				if (dir==DIR_RIGHT)
					out[idx] += 0.3f;
				break;

			case 5:
				if (dir==DIR_LEFT)
					out[idx] += 0.3f;
				break;

			case 6:
				if (dir==DIR_RIGHT || dir==DIR_UP)
					out[idx] += 0.1f;
				break;

			case 7:
				if (dir==DIR_UP)
					out[idx] += 0.3f;
				break;

			case 8:
				if (dir==DIR_LEFT || dir==DIR_UP)
					out[idx] += 0.1f;
				break;
			}
		}
	}

	for (int idx=0; idx<_region_cnt; ++idx)
		out[idx] = std::min<float>(1.0f, out[idx]);
}

void InfluenceMethod9::DebugDraw(const InputData& id, const int cx, const int cy)
{
	CGame&			game	= CGame::Get();
	CRenderer*		r		= game.Renderer();
	InputData		nid		= id.Normalize();

	std::vector<float> out;
	CalcInfluence(nid, out);

	// �� �Լ� ���ڵķ����Դ�Ϊ���ģ�
	r->DrawRect(cx, cy, 4*2, 4*2, r->_yellow);

	// �Ź������Ϸ������ 0��
	r->DrawRect(cx-4*12, cy+4*2, 4*12, 4*12, r->_yellow);
	r->FillRect(cx-4*12+1, cy+4*2+1, 4*12-2, 4*12-2, Fade(r->_red, out[0]) );
	// �Ź����Ϸ������ 1��
	r->DrawRect(cx, cy+4*2, 4*2, 4*12, r->_yellow);
	r->FillRect(cx+1, cy+4*2+1, 4*2-2, 4*12-2, Fade(r->_red, out[1]) );
	// �Ź������Ϸ������ 2��
	r->DrawRect(cx+4*2, cy+4*2, 4*12, 4*12, r->_yellow);
	r->FillRect(cx+4*2+1, cy+4*2+1, 4*12-2, 4*12-2, Fade(r->_red, out[2]) );

	// �Ź����󷽣���� 3��
	r->DrawRect(cx-4*12, cy, 4*12, 4*2, r->_yellow);
	r->FillRect(cx-4*12+1, cy+1, 4*12-2, 4*2-2, Fade(r->_red, out[3]) );
	// �Ź����ҷ������ 5��
	r->DrawRect(cx+4*2, cy, 4*12, 4*2, r->_yellow);
	r->FillRect(cx+4*2+1, cy+1, 4*12-2, 4*2-2, Fade(r->_red, out[5]) );

	// �Ź������·������ 6��
	r->DrawRect(cx-4*12, cy-4*12, 4*12, 4*12, r->_yellow);
	r->FillRect(cx-4*12+1, cy-4*12+1, 4*12-2, 4*12-2, Fade(r->_red, out[6]) );
	// �Ź����·������ 7��
	r->DrawRect(cx, cy-4*12, 4*2, 4*12, r->_yellow);
	r->FillRect(cx+1, cy-4*12+1, 4*2-2, 4*12-2, Fade(r->_red, out[7]) );
	// �Ź������·������ 8��
	r->DrawRect(cx+4*2, cy-4*12, 4*12, 4*12, r->_yellow);
	r->FillRect(cx+4*2+1, cy-4*12+1, 4*12-2, 4*12-2, Fade(r->_red, out[8]) );
}

// ����������½�Ϊ (0, 0)����������ͼ�����½� (-12, -12)
// ����뿪����ͼ��Զ��Զ���Ͳ������ˣ���Ϊ�;���û���κ��ཻ
SDL_Rect InfluenceMethod9::GetRectInfluence(const int idx)
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

// --------------------------------------------------------------------------------------
// ���򣺽���Զ*������*����
// ���򣺽���Զ*������*����
// ���Ͻǣ����Ͻǣ����½ǣ����½�
const int InfluenceMethodVerHorSquares::_region_cnt = 3*3*2 + 3*3*2 + 4;

void InfluenceMethodVerHorSquares::CalcInfluence(const InputData& id, std::vector<float>& out)
{
	InputData nid = id.Normalize();

	out.resize(_region_cnt);
	for (size_t i=0; i<out.size(); ++i)
		out[i] = 0.0f;

	for (size_t i=0; i<nid.enemies_pos.size(); ++i)
	{
		for (int idx=0; idx<_region_cnt; ++idx)
		{
			SDL_Rect region = GetRectInfluence(idx);
			SDL_Rect rect = GetTankRect(nid.enemies_pos[i]);
			bool isInter = SDL_HasIntersection(&region, &rect);
			if (!isInter)  continue;

			out[idx] += 0.1f;
		}
	}

	for (size_t i=0; i<nid.enemies_bullet_pos.size(); ++i)
	{
		for (int idx=0; idx<_region_cnt; ++idx)
		{
			SDL_Rect region = GetRectInfluence(idx);
			DIRECTION dir = nid.enemies_bullet_dir[i];
			SDL_Rect rect = GetBulletRect(nid.enemies_bullet_pos[i], dir);
			bool isInter = SDL_HasIntersection(&region, &rect);
			if (!isInter)  continue;

			out[idx] += 0.15f;
		}
	}

	for (int idx=0; idx<_region_cnt; ++idx)
		out[idx] = std::min<float>(1.0f, out[idx]);
}

void InfluenceMethodVerHorSquares::DebugDraw(const InputData& id, const int cx, const int cy)
{
	CGame&		game	= CGame::Get();
	CRenderer*	r		= game.Renderer();
	InputData	nid		= id.Normalize();

	std::vector<float> out;
	CalcInfluence(nid, out);

	// 4 ��ʾ�Ŵ���
	//r->DrawRect(cx, cy, 4*2, 4*2, r->_yellow);  �����Լ����ڵ�ԭ��λ��
	for (int i=0; i<_region_cnt; ++i)
	{
		int yoffset = 0;
		if (3*3*2<=i && i<3*3*2*2)
			yoffset = 115;  // �ֿ���ʾ���������һ��
		else if (3*3*2*2<=i)
			yoffset = -115;
		SDL_Rect rect = GetRectInfluence(i);
		r->DrawRect(cx+4*rect.x, cy+yoffset+4*rect.y, 4*rect.w, 4*rect.h, r->_yellow);
		r->FillRect(cx+4*rect.x+1, cy+yoffset+4*rect.y+1, 4*rect.w-2, 4*rect.h-2, Fade(r->_red, out[i]) );
	}
}

// ����������½�Ϊ (0, 0)����������ͼ�����½� (-12, -12)
// ����뿪����ͼ��Զ��Զ���Ͳ������ˣ���Ϊ�;���û���κ��ཻ
SDL_Rect InfluenceMethodVerHorSquares::GetRectInfluence(const int idx)
{
	assert(0<=idx && idx<_region_cnt);

	if (0<=idx && idx<3*3*2)
	{
		/*
		-------------------------------------
		|  0  |  1  |  2  |  9  | 10  | 11  |
		-------------------------------------
		|  3  |  4  |  5  | 12  | 13  | 14  |
		-------------------------------------
		|  6  |  7  |  8  | 15  | 16  | 17  |
		-------------------------------------
		��ҵ�λ���� 5��12 ���м䣬������½����� (0, 0)
		*/
		switch (idx)
		{
		case 0:
			return SDL_Rect { -12,  2, 5, 2 };
		case 1:
			return SDL_Rect {  -7,  2, 4, 2 };
		case 2:
			return SDL_Rect {  -3,  2, 4, 2 };
		case 3:
			return SDL_Rect { -12,  0, 5, 2 };
		case 4:
			return SDL_Rect {  -7,  0, 4, 2 };
		case 5:
			return SDL_Rect {  -3,  0, 4, 2 };
		case 6:
			return SDL_Rect { -12, -2, 5, 2 };
		case 7:
			return SDL_Rect {  -7, -2, 4, 2 };
		case 8:
			return SDL_Rect {  -3, -2, 4, 2 };

		case 9:
			return SDL_Rect {   1,  2, 4, 2 };
		case 10:
			return SDL_Rect {   5,  2, 4, 2 };
		case 11:
			return SDL_Rect {   9,  2, 5, 2 };
		case 12:
			return SDL_Rect {   1,  0, 4, 2 };
		case 13:
			return SDL_Rect {   5,  0, 4, 2 };
		case 14:
			return SDL_Rect {   9,  0, 5, 2 };
		case 15:
			return SDL_Rect {   1, -2, 4, 2 };
		case 16:
			return SDL_Rect {   5, -2, 4, 2 };
		case 17:
			return SDL_Rect {   9, -2, 5, 2 };
		}
	}
	else if (3*3*2<=idx && idx<_region_cnt-4)
	{
		/*
		-------------------
		| 18  | 19  | 20  |
		-------------------
		| 21  | 22  | 23  |
		-------------------
		| 24  | 25  | 26  |
		-------------------
		| 27  | 28  | 29  |
		-------------------
		| 30  | 31  | 32  |
		-------------------
		| 33  | 34  | 35  |
		-------------------
		��ҵ�λ���� 25��28 ���м䣬������½����� (0, 0)
		*/
		switch (idx)
		{
		case 18:
			return SDL_Rect { -2,   9, 2, 5 };
		case 19:
			return SDL_Rect {  0,   9, 2, 5 };
		case 20:
			return SDL_Rect {  2,   9, 2, 5 };
		case 21:
			return SDL_Rect { -2,   5, 2, 4 };
		case 22:  
			return SDL_Rect {  0,   5, 2, 4 };
		case 23:  
			return SDL_Rect {  2,   5, 2, 4 };
		case 24:
			return SDL_Rect { -2,   1, 2, 4 };
		case 25:
			return SDL_Rect {  0,   1, 2, 4 };
		case 26:
			return SDL_Rect {  2,   1, 2, 4 };
		case 27:
			return SDL_Rect { -2,  -3, 2, 4 };
		case 28:
			return SDL_Rect {  0,  -3, 2, 4 };
		case 29:
			return SDL_Rect {  2,  -3, 2, 4 };
		case 30:
			return SDL_Rect { -2,  -7, 2, 4 };
		case 31:
			return SDL_Rect {  0,  -7, 2, 4 };
		case 32:
			return SDL_Rect {  2,  -7, 2, 4 };
		case 33:
			return SDL_Rect { -2, -12, 2, 5 };
		case 34:
			return SDL_Rect {  0, -12, 2, 5 };
		case 35:
			return SDL_Rect {  2, -12, 2, 5 };
		}
	}
	else if (idx==_region_cnt-4)
	{
		return SDL_Rect { -12,   4, 10, 10 };
	}
	else if (idx==_region_cnt-3)
	{
		return SDL_Rect {   4,   4, 10, 10 };
	}
	else if (idx==_region_cnt-2)
	{
		return SDL_Rect { -12, -12, 10, 10 };
	}
	else if (idx==_region_cnt-1)
	{
		return SDL_Rect {   4, -12, 10, 10 };
	}

	return SDL_Rect { 0, 0, 0, 0 };
}

// --------------------------------------------------------------------------------------
template <typename InfluenceMethod>
StrategyWL<InfluenceMethod>::StrategyWL()
	: CStrategy(InfluenceMethod::_region_cnt, ACTION)
{
}

template <typename InfluenceMethod>
void StrategyWL<InfluenceMethod>::Draw()
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

	// ������ player Ϊ���ĵ�����ͼ
	int cx = 700;  // �������½� x ����
	int cy = 286;  // �������½� y ����

	InfluenceMethod::DebugDraw(id, cx, cy);
}

template <typename InfluenceMethod>
void StrategyWL<InfluenceMethod>::SetupNetwork()
{
	Layer* layer1 = new FullyConnected<ReLU>(InfluenceMethod::_region_cnt, HIDDEN);
	Layer* layer2 = new FullyConnected<ReLU>(HIDDEN, HIDDEN);
	Layer* layer3 = new FullyConnected<Softmax>(HIDDEN, ACTION);

	network.add_layer(layer1);
	network.add_layer(layer2);
	network.add_layer(layer3);

	network.set_output(new MultiClassEntropy() );
}

template <typename InfluenceMethod>
void StrategyWL<InfluenceMethod>::ConvertData(const InputData& id)
{
	input.fill(0);

	std::vector<float> out;
	InfluenceMethod::CalcInfluence(id, out);

	for (int i=0; i<InfluenceMethod::_region_cnt; ++i)
	{
		input(i, 0) = out[i];
	}

	for (size_t i=0; i<out.size(); ++i)
	{
		printf("%.1f, ", out[i]);
	}
	printf("\n");
}

template <typename InfluenceMethod>
void StrategyWL<InfluenceMethod>::ConvertData(const OutputData& od)
{
	output.fill(0);

	output(od.mov, 0) = 1;

	printf("%d\n", od.mov);
}
