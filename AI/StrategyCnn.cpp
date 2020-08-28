#include "stdafx.h"
#include "StrategyCnn.h"
#include "Game.h"

using namespace MiniDNN;

/* CNN：一个如下的输入数据
 * ########################################
 * #   1   2   #   5   6   #  9   10  # 
 * #   3   4   #   7   8   #  11  12  #
 * #########################################
 * |<channel 1>|<channel 2>|<channel 3>
 *
 * 应该转化为如下的列向量
 * (1,2,3,4,5,6,7,8,9,10,11,12)T
 */

#define CHANNEL 3
#define ACTIONNUM 5
#define INDEX(x,y,c) ((c)*26*26+(x)*26+(y)) // c: channel

StrategyCNN::StrategyCNN() : CStrategy()
{
	input = Matrix::Zero(26 * 26 * CHANNEL, 1); // (player, enemies, enemies' bullet)
	output = Matrix::Zero(ACTIONNUM, 1); // 4 Move Direction & None & shoot
}

void StrategyCNN::Draw()
{
	int st_x = 620;
	int st_y = 10;
	int pixel_size = 4;
	CRenderer* render = CGame::Get().Renderer();
	// 绘制边框
	render->DrawRect(st_x, st_y, 26 * pixel_size, 26 * pixel_size, render->_white);
	
	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			int playerIdx = INDEX(i, j, 0);
			int enemyIdx = INDEX(i, j, 1);
			int enemyBulletIdx = INDEX(i, j, 2);
			int pixel_x = st_x + i * pixel_size;
			int pixel_y = st_y + j * pixel_size;
			// 绘制player层
			if (abs(input(playerIdx, 0) - 0.5f) < 0.1f) render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, render->_red_half);
			if (abs(input(playerIdx, 0) - 1.0f) < 0.1f) render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, render->_red);
			// 绘制enemy层
			if (abs(input(enemyIdx, 0) - 0.5f) < 0.1f) render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, render->_green_half);
			if (abs(input(enemyIdx, 0) - 1.0f) < 0.1f) render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, render->_green);
			// 绘制enemy bullet层
			if (abs(input(enemyBulletIdx, 0) - 0.5f) < 0.1f) render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, render->_blue_half);
			if (abs(input(enemyBulletIdx, 0) - 1.0f) < 0.1f) render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, render->_blue);
		}
	}
}

void StrategyCNN::SetupNetwork()
{
	Layer* conv1 = new Convolutional<ReLU>(26, 26, CHANNEL, 16, 3, 3);
	Layer* pool1 = new MaxPooling<Identity>(24, 24, 16, 2, 2);

	//Layer* conv2 = new Convolutional<ReLU>(12, 12, 16, 32, 5, 5);
	//Layer* pool2 = new MaxPooling<Identity>(8, 8, 32, 2, 2);

	//Layer* fully = new FullyConnected<ReLU>(8 * 8 * 32, 64);
	//Layer* fully1 = new FullyConnected<ReLU>(64, 64);
	Layer* out = new FullyConnected<Softmax>(12 * 12 * 16, ACTIONNUM);

	network.add_layer(conv1);
	network.add_layer(pool1);
	//network.add_layer(conv2);
	//network.add_layer(pool2);
	//network.add_layer(fully);
	//network.add_layer(fully1);
	network.add_layer(out);

	network.set_output(new MultiClassEntropy());
}

void StrategyCNN::ConvertData(const InputData & id)
{
	// clear input matrix
	input.fill(0);

	// channel0: player position
	FillObjectToInputMatrix(0, id.player_pos, id.player_dir, 0.5f, 0.5f);

	// channel1: enemies' position
	int enemyCount = id.enemies_pos.size();
	for (int i = 0; i < enemyCount; i++)
	{
		FillObjectToInputMatrix(1, id.enemies_pos[i], id.enemies_dir[i], 0.5f, 0.5f);
	}

	// channel2: enemies' bullet
	int enemyBulletCount = id.enemies_bullet_pos.size();
	for (int i = 0; i < enemyBulletCount; i++)
	{
		FillObjectToInputMatrix(2, id.enemies_bullet_pos[i], id.enemies_bullet_dir[i], 0.5f, 0.5f);
	}
} 

void StrategyCNN::FillObjectToInputMatrix(int channel, const Pos& pos, DIRECTION dir, float baseValue, float addValue)
{
	int x = pos.x; if (x > 24) x = 24;
	int y = pos.y; if (y > 24) y = 24;
	input(INDEX(x, y, channel), 0) = baseValue;
	input(INDEX(x + 1, y, channel), 0) = baseValue;
	input(INDEX(x, y + 1, channel), 0) = baseValue;
	input(INDEX(x + 1, y + 1, channel), 0) = baseValue;
	switch (dir)
	{
	case DIR_UP:
		input(INDEX(x, y + 1, channel), 0) += addValue;
		input(INDEX(x + 1, y + 1, channel), 0) += addValue;
		break;
	case DIR_RIGHT:
		input(INDEX(x + 1, y, channel), 0) += addValue;
		input(INDEX(x + 1, y + 1, channel), 0) += addValue;
		break;
	case DIR_DOWN:
		input(INDEX(x, y, channel), 0) += addValue;
		input(INDEX(x + 1, y, channel), 0) += addValue;
		break;
	case DIR_LEFT:
		input(INDEX(x, y, channel), 0) += addValue;
		input(INDEX(x, y + 1, channel), 0) += addValue;
		break;
	default:
		std::cout << "[Strategy] ConvertInputData Error. Direction is None.\n";
		break;
	}
}

void StrategyCNN::ConvertData(const OutputData & od)
{
	// Clear Matrix
	output.fill(0);

	output(od.mov, 0) = 1;

	//if (od.shoot) output(5, 0) = 1;
}
