#include "stdafx.h"
#include "StrategyCnn.h"
#include "Game.h"
#include <string.h>

using namespace MiniDNN;

const int view_size = 5;
const int map_width = view_size * 2 + 2;
const int map_size = map_width * map_width;
const int action = 5;
const int h_dim = 200;

#define EDGE -1					// ��Ե����Ӱ��
#define ENEMY_INFLUENCE -1		// �з�̹������Ӱ�����
#define ENEMY_FADE 0.2			// �з�̹������Ӱ��˥����
#define ENEMY_DIRECTION_INFLUENCE -1			// �з�̹������Ӱ��˥����
#define ENEMY_DIRECTION_FADE 0.7			// �з�̹������Ӱ��˥����
#define BULLET_INFLUENCE -2		// �ӵ�����Ӱ�����
#define BULLET_FADE 0.9			// �ӵ�����Ӱ��˥����

#define INDEX(x,y,c) ((c)*map_width*map_width+(x)*map_width+(y)) // c: channel

StrategyCNN::StrategyCNN() : CStrategy(map_size*3, action) {}

OutputData StrategyCNN::MakeDecision(const InputData & id)
{
	OutputData od;

	//ConvertData(id);

	if (!valid)
	{
		std::cout << "[Strategy] MakeDecision Failed. Network is not valid.\n";
		return od;
	}
	ConvertData(id);
	output = network.predict(input);

	// �ҵ������ʵ��ƶ�����
	int label = 4; // Ĭ��ΪNone
	float max = -1.0f;
	for (int i = 0; i < 5; i++)
	{
		float value = output(i, 0);
		if (value > max)
		{
			max = value;
			label = i;
		}
	}
	od.mov = (DIRECTION)label;
	//od.shoot = output(5, 0) > 0.5 ? true : false;
	
	
	return od;
}

void StrategyCNN::Train(const IODataVec & database, std::string folder, std::string fileName, float learning_rate, int batch_size, int epoch)
{
	SetupNetwork();

	int dataCount = database.size();

	Matrix x(x_dim, dataCount * 2);
	Matrix y(y_dim, dataCount * 2);

	for (int i = 0; i < dataCount; i++)
	{
		const InputData& id = database[i].first;
		const OutputData& od = database[i].second;

		ConvertData(id);
		ConvertData(od);
		for (int k = 0; k < x_dim; k++) x(k, i) = input(k, 0);
		for (int k = 0; k < y_dim; k++) y(k, i) = output(k, 0);

		const InputData& id_m = id.MirrorLR();
		const OutputData& od_m = od.MirrorLR();

		ConvertData(id_m);
		ConvertData(od_m);
		for (int k = 0; k < x_dim; k++) x(k, dataCount + i) = input(k, 0);
		for (int k = 0; k < y_dim; k++) y(k, dataCount + i) = output(k, 0);
	}

	MiniDNN::Adam opt;
	opt.m_lrate = learning_rate;

	MiniDNN::VerboseCallback callback;
	network.set_callback(callback);

	network.init(0, 0.01, 123);

	network.fit(opt, x, y, batch_size, epoch);

	valid = true;

	// export_net�ļ����Ѵ���ʱ�ᱨ����Network.h�ж�Ӧ��ע�͵����Լ������ļ��У���ѵ��ǰ���ļ���ɾ�� �����޸���
	network.export_net(folder, fileName);
}

void StrategyCNN::Draw()
{
	// ����ȫ������ͼ���ϰ��
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

			int influence = -influence_map[i][j][0] * 256;
			if (influence > 255) influence = 255;
			SDL_Color color{ (Uint8)influence, 0, 0, SDL_ALPHA_OPAQUE };
			render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, color);
		}
	}

	// ����ȫ������ͼ��Σ������
	st_x = 770;
	st_y = 10;
	pixel_size = 4;

	render->DrawRect(st_x, st_y, 26 * pixel_size, 26 * pixel_size, render->_white);

	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			int pixel_x = st_x + i * pixel_size;
			int pixel_y = st_y + j * pixel_size;

			int influence = -influence_map[i][j][1] * 256;
			if (influence > 255) influence = 255;
			SDL_Color color{ (Uint8)influence, 0, 0, SDL_ALPHA_OPAQUE };
			render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, color);
		}
	}

	if (valid)
	{
		// ���ƾֲ�����ͼ���ϰ��
		st_x = 620;
		st_y = 120;
		pixel_size = 5;
		render->DrawRect(st_x, st_y, map_width * pixel_size, map_width * pixel_size, render->_white);
		for (int i = 0; i < map_width; i++)
		{
			for (int j = 0; j < map_width; j++)
			{
				int pixel_x = st_x + i * pixel_size;
				int pixel_y = st_y + j * pixel_size;

				int influence = -input(INDEX(i, j, 0), 0) * 256;
				if (influence > 255) influence = 255;
				SDL_Color color{ (Uint8)influence, 0, 0, SDL_ALPHA_OPAQUE };
				render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, color);
			}
		}

		// ���ƾֲ�����ͼ�����ˣ�
		st_x = 700;
		st_y = 120;
		pixel_size = 5;
		render->DrawRect(st_x, st_y, map_width * pixel_size, map_width * pixel_size, render->_white);
		for (int i = 0; i < map_width; i++)
		{
			for (int j = 0; j < map_width; j++)
			{
				int pixel_x = st_x + i * pixel_size;
				int pixel_y = st_y + j * pixel_size;

				int influence = -input(INDEX(i, j, 1), 0) * 256;
				if (influence > 255) influence = 255;
				SDL_Color color{ (Uint8)influence, 0, 0, SDL_ALPHA_OPAQUE };
				render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, color);
			}
		}

		// ���ƾֲ�����ͼ��Σ������
		st_x = 780;
		st_y = 120;
		pixel_size = 5;
		render->DrawRect(st_x, st_y, map_width * pixel_size, map_width * pixel_size, render->_white);
		for (int i = 0; i < map_width; i++)
		{
			for (int j = 0; j < map_width; j++)
			{
				int pixel_x = st_x + i * pixel_size;
				int pixel_y = st_y + j * pixel_size;

				int influence = -input(INDEX(i, j, 2), 0) * 256;
				if (influence > 255) influence = 255;
				SDL_Color color{ (Uint8)influence, 0, 0, SDL_ALPHA_OPAQUE };
				render->FillRect(pixel_x, pixel_y, pixel_size, pixel_size, color);
			}
		}
	}
}

void StrategyCNN::SetupNetwork()
{
	Layer* conv1 = new Convolutional<ReLU>(12, 12, 3, 16, 3, 3);
	Layer* pool1 = new MaxPooling<Identity>(10, 10, 16, 2, 2);
	Layer* layer1 = new FullyConnected<ReLU>(5*5*16, h_dim);
	//Layer* layer1 = new FullyConnected<ReLU>(map_size, h_dim);
	Layer* layer11 = new FullyConnected<ReLU>(h_dim, h_dim);
	Layer* layer2 = new FullyConnected<Softmax>(h_dim, action);

	network.add_layer(conv1);
	network.add_layer(pool1);
	network.add_layer(layer1);
	network.add_layer(layer11);
	network.add_layer(layer2);

	network.set_output(new MultiClassEntropy());
}

void StrategyCNN::ConvertData(const InputData & id)
{
	UpdateInfluenceMap(id);

	// Clear Input Matrix
	input.fill(0);

	// Find out edge
	int x_min = id.player_pos.x - view_size;
	int x_max = id.player_pos.x + view_size + 1; // ̹��ռ�ĸ�����Ҫ��һ
	int y_min = id.player_pos.y - view_size;
	int y_max = id.player_pos.y + view_size + 1; // ̹��ռ�ĸ�����Ҫ��һ

	// Fill Input Matrix
	for (int i = x_min; i <= x_max; i++)
	{
		for (int j = y_min; j <= y_max; j++)
		{
			if (i < 0 || j < 0 || i > 25 || j > 25			 // ��Ե
				|| (i >= 11 && i <= 14 && j >= 0 && j <= 2)) // ���ط�Χ������
			{
				input(INDEX(i - x_min, j - y_min, 0), 0) = EDGE;
			}
			else
			{
				input(INDEX(i - x_min, j - y_min, 1), 0) = influence_map[i][j][0];
				input(INDEX(i - x_min, j - y_min, 2), 0) = influence_map[i][j][1];
			}
		}
	}
}

void StrategyCNN::ConvertData(const OutputData & od)
{
	// Clear Matrix
	output.fill(0);

	output(od.mov, 0) = 1;

	//if (od.shoot) output(5, 0) = 1;
}

void StrategyCNN::UpdateInfluenceMap(const InputData & id)
{
	// Clear
	memset(influence_map, 0, 26 * 26 * 2 * sizeof(float));

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
				influence_map[x][y][0] += ENEMY_INFLUENCE * pow(ENEMY_FADE, dist);
			}

			// Enemy Direction Influence
			for (int i = 0; i < enemy_num; i++)
			{
				int bx = id.enemies_pos[i].x;
				int by = id.enemies_pos[i].y;

				// ���˳����䷽�����һ�����Ϊ2��Ӱ����������˥��
				// ��ͼ�����ҵĳ���Ϊ��
				// 1 0.99 0.98 0.97 0.96 ...
				// 1 0.99 0.98 0.97 0.96 ...
				switch (id.enemies_dir[i])
				{
				case DIR_UP:
					if ((x == bx || x == bx + 1) && y > by)
					{
						influence_map[x][y][0] += ENEMY_DIRECTION_INFLUENCE * pow(ENEMY_DIRECTION_FADE, abs(y - by - 1));
					}
					break;
				case DIR_DOWN:
					if ((x == bx || x == bx + 1) && y <= by)
					{
						influence_map[x][y][0] += ENEMY_DIRECTION_INFLUENCE * pow(ENEMY_DIRECTION_FADE, abs(y - by));
					}
					break;
				case DIR_LEFT:
					if ((y == by || y == by + 1) && x <= bx)
					{
						influence_map[x][y][0] += ENEMY_DIRECTION_INFLUENCE * pow(ENEMY_DIRECTION_FADE, abs(x - bx));
					}
					break;
				case DIR_RIGHT:
					if ((y == by || y == by + 1) && x > bx)
					{
						influence_map[x][y][0] += ENEMY_DIRECTION_INFLUENCE * pow(ENEMY_DIRECTION_FADE, abs(x - bx - 1));
					}
					break;
				default:
					std::cerr << "Enemy Bullet Direction Error!" << std::endl;
					break;
				}
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
						influence_map[x][y][1] += BULLET_INFLUENCE * pow(BULLET_FADE, abs(y - by - 1));
					}
					break;
				case DIR_DOWN:
					if ((x == bx || x == bx + 1) && y <= by)
					{
						influence_map[x][y][1] += BULLET_INFLUENCE * pow(BULLET_FADE, abs(y - by));
					}
					break;
				case DIR_LEFT:
					if ((y == by || y == by + 1) && x <= bx)
					{
						influence_map[x][y][1] += BULLET_INFLUENCE * pow(BULLET_FADE, abs(x - bx));
					}
					break;
				case DIR_RIGHT:
					if ((y == by || y == by + 1) && x > bx)
					{
						influence_map[x][y][1] += BULLET_INFLUENCE * pow(BULLET_FADE, abs(x - bx - 1));
					}
					break;
				default:
					std::cerr << "Enemy Bullet Direction Error!" << std::endl;
					break;
				}
			}

			//if (influence_map[x][y][0] < -1) influence_map[x][y][0] = -1;
			//if (influence_map[x][y][1] < -1) influence_map[x][y][1] = -1;
		}
	}
}