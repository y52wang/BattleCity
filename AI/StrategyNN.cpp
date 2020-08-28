#include "stdafx.h"
#include "StrategyNN.h"
#include "Game.h"
#include <string.h>

using namespace MiniDNN;

#define VIEW_SIZE 2 // ��Ұ��С
#define MAP_WIDTH 6 // MAP_WIDTH = VIEW_SIZE * 2 + 2
#define MAP_SIZE 36 // MAP_SIZE = (VIEW_SIZE * 2 + 2)^2
#define ACTION 5    // �����ռ��С
#define HDIM 20		// ������Ԫ����

#define EDGE -1					// ��Ե����Ӱ��
#define ENEMY_INFLUENCE -1		// �з�̹������Ӱ�����
#define ENEMY_FADE 0.3			// �з�̹������Ӱ��˥����
#define BULLET_INFLUENCE -2		// �ӵ�����Ӱ�����
#define BULLET_FADE 0.8			// �ӵ�����Ӱ��˥����

#define INDEX(x,y) ((x)*6+(y))

StrategyNN::StrategyNN() : CStrategy(MAP_SIZE, ACTION) {}

void StrategyNN::Draw()
{
	// ����ȫ������ͼ
	int st_x = 620;
	int st_y = 10;
	int pixel_size = 4;
	CRenderer* render = CGame::Get().Renderer();

	render->DrawRect(st_x, st_y, 26 * pixel_size, 26 * pixel_size, render->_white);

	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			int pixel_x = st_x + i * pixel_size;
			int pixel_y = st_y + j * pixel_size;
			
			int influence = -influence_map[i][j] * 256;
			if (influence > 255) influence = 255;
			SDL_Color color { (Uint8)influence, 0, 0, SDL_ALPHA_OPAQUE };
			render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, color);
		}
	}

	// ���ƾֲ�����ͼ
	if (valid)
	{
		st_x = 620;
		st_y = 120;
		pixel_size = 10;
		render->DrawRect(st_x, st_y, MAP_WIDTH * pixel_size, MAP_WIDTH * pixel_size, render->_white);
		for (int i = 0; i < MAP_WIDTH; i++)
		{
			for (int j = 0; j < MAP_WIDTH; j++)
			{
				int pixel_x = st_x + i * pixel_size;
				int pixel_y = st_y + j * pixel_size;

				int influence = -input(INDEX(i, j), 0) * 256;
				if (influence > 255) influence = 255;
				SDL_Color color{ (Uint8)influence, 0, 0, SDL_ALPHA_OPAQUE };
				render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, color);
			}
		}
	}
}

void StrategyNN::SetupNetwork()
{
	Layer* layer1 = new FullyConnected<ReLU>(MAP_SIZE, HDIM);
	Layer* layer11 = new FullyConnected<ReLU>(HDIM, HDIM);
	Layer* layer2 = new FullyConnected<Softmax>(HDIM, ACTION);

	network.add_layer(layer1);
	network.add_layer(layer11);
	network.add_layer(layer2);

	network.set_output(new MultiClassEntropy());
}

void StrategyNN::ConvertData(const InputData & id)
{
	UpdateInfluenceMap(id);

	// Clear Input Matrix
	input.fill(0);

	// Find out edge
	int x_min = id.player_pos.x - VIEW_SIZE;	
	int x_max = id.player_pos.x + VIEW_SIZE + 1; // ̹��ռ�ĸ�����Ҫ��һ
	int y_min = id.player_pos.y - VIEW_SIZE;	
	int y_max = id.player_pos.y + VIEW_SIZE + 1; // ̹��ռ�ĸ�����Ҫ��һ

	// Fill Input Matrix
	for (int i = x_min; i <= x_max; i++)
	{
		for (int j = y_min; j <= y_max; j++)
		{
			if(i < 0 || j < 0 || i > 25 || j > 25			 // ��Ե
				|| (i >= 11 && i <= 14 && j >= 0 && j <= 2)) // ���ط�Χ������
			{
				input(INDEX(i - x_min, j - y_min), 0) = EDGE;
			}
			else
			{
				input(INDEX(i - x_min, j - y_min), 0) = influence_map[i][j];
			}
		}
	}
}

void StrategyNN::ConvertData(const OutputData & od)
{
	// Clear Matrix
	output.fill(0);

	output(od.mov, 0) = 1;

	//if (od.shoot) output(5, 0) = 1;
}

void StrategyNN::UpdateInfluenceMap(const InputData & id)
{
	// Clear
	memset(influence_map, 0, 26 * 26 * sizeof(float));

	int enemy_num = id.enemies_pos.size();
	int bullet_num = id.enemies_bullet_pos.size();

	for (int x = 0; x < 26; x++)
	{
		for (int y = 0; y < 26; y++)
		{
			// Enemy Influence
			for (int i = 0; i < enemy_num; i++)
			{
				int ex = id.enemies_pos[i].x;
				int ey = id.enemies_pos[i].y;
				// ̹��ռ�ĸ���������ǵ���һ����������پ���
				// ����ͼ��ʾ�����ֱ�ʾ���룩��
				//       3 3
				//     3 2 2 3
				//   3 2 1 1 2 3
				// 3 2 1 0 0 1 2 3
				// 3 2 1 0 0 1 2 3
				//   3 2 1 1 2 3
				//     3 2 2 3
				//       3 3
				int dist_x = min(abs(x - ex), abs(x - ex - 1));
				int dist_y = min(abs(y - ey), abs(y - ey - 1));
				int dist = dist_x + dist_y;
				influence_map[x][y] += ENEMY_INFLUENCE * pow(ENEMY_FADE, dist);
			}

			// Bullet Influence
			for (int i = 0; i < bullet_num; i++)
			{
				int bx = id.enemies_bullet_pos[i].x;
				int by = id.enemies_bullet_pos[i].y;

				// �ӵ������䷽�����һ�����Ϊ2��Ӱ����������˥��
				// ��ͼ�����ҵ��ӵ�Ϊ��
				// 1 0.99 0.98 0.97 0.96 ...
				// 1 0.99 0.98 0.97 0.96 ...
				switch (id.enemies_bullet_dir[i])
				{
				case DIR_UP:
					if ((x == bx || x == bx + 1) && y > by)
					{
						influence_map[x][y] += BULLET_INFLUENCE * pow(BULLET_FADE, abs(y - by - 1));
					}
					break;
				case DIR_DOWN:
					if ((x == bx || x == bx + 1) && y <= by)
					{
						influence_map[x][y] += BULLET_INFLUENCE * pow(BULLET_FADE, abs(y - by));
					}
					break;
				case DIR_LEFT:
					if ((y == by || y == by + 1) && x <= bx)
					{
						influence_map[x][y] += BULLET_INFLUENCE * pow(BULLET_FADE, abs(x - bx));
					}
					break;
				case DIR_RIGHT:
					if ((y == by || y == by + 1) && x > bx)
					{
						influence_map[x][y] += BULLET_INFLUENCE * pow(BULLET_FADE, abs(x - bx - 1));
					}
					break;
				default:
					std::cerr << "Enemy Bullet Direction Error!" << std::endl;
					break;
				}
			}
		}
	}
}