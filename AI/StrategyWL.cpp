#include "stdafx.h"
#include "StrategyWL.h"
#include "Game.h"

using namespace MiniDNN;

#define REGION_CNT	8	// 总的区域
#define ACTION		6	// 移动 5 个，Shoot 1 个
#define HIDDEN		16	// 隐层单元个数

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


	// 以玩家为中心，进行势力图的绘制
	// 也有很多种画法，每一种都能抽象为神经网络的输入

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

	// 第二种：共 8 个 Region
	// 画 2 条横线
	r->DrawLine(0,	pos_y,				lvl->LevelWidth()*ts,	pos_y,			r->_yellow);
	r->DrawLine(0,	pos_y+ph,			lvl->LevelWidth()*ts,	pos_y+ph,		r->_yellow);

	// 画 2 条纵线
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
