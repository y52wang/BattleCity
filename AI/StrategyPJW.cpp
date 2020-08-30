#include "stdafx.h"
#include "StrategyPJW.h"
#include "Game.h"

using namespace MiniDNN;

#define XDIM 16 // (enemy pos, enemy next pos)*2; (bullet pos, bullet next pos)*2
#define HDIM 64
#define YDIM 6

StrategyPJW::StrategyPJW() : CStrategy(XDIM, YDIM) {}

void StrategyPJW::SetupNetwork()
{
	Layer* layer1 = new FullyConnected<ReLU>(XDIM, HDIM);
	Layer* layer11 = new FullyConnected<ReLU>(HDIM, HDIM);
	Layer* layer2 = new FullyConnected<Sigmoid>(HDIM, YDIM);

	network.add_layer(layer1);
	network.add_layer(layer11);
	network.add_layer(layer2);

	network.set_output(new MultiClassEntropy());
}

void StrategyPJW::ConvertData(const InputData & id)
{
	const float inverse26 = 1.0f / 26.0f;
	int writeIdx(0);
	float px = id.player_pos.x + 0.5f;
	float py = id.player_pos.y + 0.5f;

	// enemy positon, deltaPosition
	int enemyCnt = id.enemies_pos.size();
	for (int i = 0; i < 2; i++)
	{
		if (i < enemyCnt)
		{
			float ex = id.enemies_pos[i].x + 0.5f;
			float ey = id.enemies_pos[i].y + 0.5f;
			float dx = 0; // enemy delta x
			float dy = 0; // enemy delta y
			input(writeIdx++, 0) = (ex - px) / 26.0f;
			input(writeIdx++, 0) = (ey - py) / 26.0f;
			switch (id.enemies_dir[i])
			{
			case DIR_UP:
				if (ey + 0.5f < 25.0f) dy = inverse26; break;
			case DIR_DOWN:
				if (ey - 0.5f > 0.0f) dy = -inverse26; break;
			case DIR_LEFT:
				if (ex - 0.5f > 0.0f) dx = -inverse26; break;
			case DIR_RIGHT:
				if (ex + 0.5f < 25.0f) dx = inverse26; break;
			}
			input(writeIdx++, 0) = dx;
			input(writeIdx++, 0) = dy;
		}
		else
		{
			input(writeIdx++, 0) = 0;
			input(writeIdx++, 0) = 0;
			input(writeIdx++, 0) = 0;
			input(writeIdx++, 0) = 0;
		}
	}

	// bullet position, deltaPosition
	int bulletCnt = id.enemies_bullet_pos.size();
	for (int i = 0; i < 2; i++)
	{
		if (i < bulletCnt)
		{
			float bx = id.enemies_bullet_pos[i].x + 0.5f;
			float by = id.enemies_bullet_pos[i].y + 0.5f;
			float dx = 0; // enemy delta x
			float dy = 0; // enemy delta y
			input(writeIdx++, 0) = (bx - px) / 26.0f;
			input(writeIdx++, 0) = (by - py) / 26.0f;
			switch (id.enemies_bullet_dir[i])
			{
			case DIR_UP:
				if (by + 0.5f < 25.0f) dy = inverse26; break;
			case DIR_DOWN:
				if (by - 0.5f > 0.0f) dy = -inverse26; break;
			case DIR_LEFT:
				if (bx - 0.5f > 0.0f) dx = -inverse26; break;
			case DIR_RIGHT:
				if (bx + 0.5f < 25.0f) dx = inverse26; break;
			}
			input(writeIdx++, 0) = dx;
			input(writeIdx++, 0) = dy;
		}
		else
		{
			input(writeIdx++, 0) = 0;
			input(writeIdx++, 0) = 0;
			input(writeIdx++, 0) = 0;
			input(writeIdx++, 0) = 0;
		}
	}

	if (writeIdx != XDIM) std::cerr << "write index error" << std::endl;
}

void StrategyPJW::ConvertData(const OutputData & od)
{
	// Clear Matrix
	output.fill(0);

	output(od.mov, 0) = 1;

	/*if (od.shoot)
	{
		output(0, 0) = 0;
		output(1, 0) = 0;
		output(2, 0) = 0;
		output(3, 0) = 0;
		output(4, 0) = 0;
		output(5, 0) = 1;
	}*/
}