#include "stdafx.h"
#include "StrategyWL.h"
#include "Game.h"

using namespace MiniDNN;

#define REGION_CNT	8	// �ܵ�����
#define ACTION		6	// �ƶ� 5 ����Shoot 1 ��
#define HIDDEN		16	// ���㵥Ԫ����

StrategyWL::StrategyWL()
	: CStrategy(REGION_CNT, ACTION)
{
}

void StrategyWL::Draw()
{
	CGame&		game	= CGame::Get();
	CRenderer*	r		= game.Renderer();
	CLevel*		lvl		= game.Level();
	CPlayer*	p		= game.Player();

	int			ts		= game.TailSize();
	int			pos_x	= p->GetX() * ts;
	int			pos_y	= p->GetY() * ts;
	int			ph		= p->GetPlayerHeight();
	int			pw		= p->GetPlayerWidth();


	// �����Ϊ���ģ���������ͼ�Ļ���
	// Ҳ�кܶ��ֻ�����ÿһ�ֶ��ܳ���Ϊ�����������

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

	// �ڶ��֣��� 8 �� Region
	// �� 2 ������
	r->DrawLine(0,	pos_y,				lvl->LevelWidth()*ts,	pos_y,			r->_yellow);
	r->DrawLine(0,	pos_y+ph,			lvl->LevelWidth()*ts,	pos_y+ph,		r->_yellow);

	// �� 2 ������
	r->DrawLine(pos_x,			0,		pos_x,			lvl->LevelHeight()*ts,	r->_yellow);
	r->DrawLine(pos_x+pw,		0,		pos_x+pw,		lvl->LevelHeight()*ts,	r->_yellow);
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
