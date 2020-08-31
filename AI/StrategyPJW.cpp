#include "stdafx.h"
#include "StrategyPJW.h"
#include "Game.h"

using namespace MiniDNN;

#define XDIM 4 // (enemy pos, enemy next pos)*2; (bullet pos, bullet next pos)*2
#define HDIM 16
#define YDIM 6

StrategyPJW::StrategyPJW() : CStrategy(XDIM, YDIM) {}

OutputData StrategyPJW::MakeDecision(const InputData & id)
{
	OutputData od;
	if (!valid)
	{
		std::cout << "[Strategy] MakeDecision Failed. Network is not valid.\n";
		return od;
	}
	ConvertData(id);
	output = network.predict(input);

	// 找到最大概率的移动方向
	int label = 4; // 默认为None
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
	od.shoot = output(5, 0) > 0.1 ? true : false;
	return od;
}

void StrategyPJW::Train(const IODataVec & database, std::string folder, std::string fileName, float learning_rate, int batch_size, int epoch)
{
	SetupNetwork();

	int dataCount = database.size();
	int uselessCnt(0);
	for (int i = 0; i < dataCount; i++)
	{
		const InputData& id = database[i].first;
		if (id.player_bullet_pos.x != -1) {
			uselessCnt++; continue;
		}
		const OutputData& od = database[i].second;
		if (!od.shoot && od.mov == DIR_NONE) {
			uselessCnt++; continue;
		}
	}
	std::cout << "Useless Count: " << uselessCnt << std::endl;

	Matrix x(x_dim, dataCount - uselessCnt);
	Matrix y(y_dim, dataCount - uselessCnt);

	int writeIdx(0);
	for (int i = 0; i < dataCount; i++)
	{
		const InputData& id = database[i].first;
		if (id.player_bullet_pos.x != -1) continue;
		const OutputData& od = database[i].second;
		if (!od.shoot && od.mov == DIR_NONE) continue;

		ConvertData(id);
		ConvertData(od);

		for (int k = 0; k < x_dim; k++) x(k, writeIdx) = input(k, 0);
		for (int k = 0; k < y_dim; k++) y(k, writeIdx) = output(k, 0);
		writeIdx++;
	}

	MiniDNN::Adam opt;
	opt.m_lrate = learning_rate;

	MiniDNN::VerboseCallback callback;
	network.set_callback(callback);

	network.init(0, 0.01, 123);

	network.fit(opt, x, y, batch_size, epoch);

	valid = true;

	// export_net文件夹已存在时会报错，把Network.h中对应行注释掉，自己创建文件夹，或训练前把文件夹删除 （已修复）
	network.export_net(folder, fileName);
}

void StrategyPJW::AnalyzeData(const IODataVec & database)
{
	int dataCnt = database.size();
	std::cout << "Data Count: " << dataCnt << std::endl;

	int upCnt(0);
	int downCnt(0);
	int leftCnt(0);
	int rightCnt(0);
	int noneCnt(0);
	int shootCnt(0);
	
	int uselessCnt(0);

	for (int i = 0; i < dataCnt; i++)
	{
		const InputData& id = database[i].first;
		if (id.player_bullet_pos.x != -1) uselessCnt++;
		const OutputData& od = database[i].second;
		if (od.shoot) shootCnt++;
		else
		{
			switch (od.mov)
			{
			case DIR_UP: upCnt++; break;
			case DIR_DOWN: downCnt++; break;
			case DIR_LEFT: leftCnt++; break;
			case DIR_RIGHT: rightCnt++; break;
			case DIR_NONE: noneCnt++; break;
			}
		}
	}
	std::cout << "Useless Count: " << uselessCnt << std::endl;
	std::cout << "Up Count: " << upCnt << std::endl;
	std::cout << "Down Count: " << downCnt << std::endl;
	std::cout << "Left Count: " << leftCnt << std::endl;
	std::cout << "Right Count: " << rightCnt << std::endl;
	std::cout << "None Count: " << noneCnt << std::endl;
	std::cout << "Shoot Count: " << shootCnt << std::endl;
}

void StrategyPJW::SetupNetwork()
{
	Layer* layer1 = new FullyConnected<ReLU>(XDIM, HDIM);
	//Layer* layer11 = new FullyConnected<ReLU>(HDIM, HDIM);
	//Layer* layer2 = new FullyConnected<Sigmoid>(HDIM, YDIM);
	Layer* layer2 = new FullyConnected<Softmax>(HDIM, YDIM);

	network.add_layer(layer1);
	//network.add_layer(layer11);
	network.add_layer(layer2);

	//network.set_output(new RegressionMSE());
	network.set_output(new MultiClassEntropy());
}

void StrategyPJW::ConvertData(const InputData & id)
{
	const float inverse26 = 1.0f / 26.0f;
	int writeIdx(0);
	float px = id.player_pos.x + 0.5f;
	float py = id.player_pos.y + 0.5f;

	//input(writeIdx++, 0) = px * inverse26;
	//input(writeIdx++, 0) = py * inverse26;
	/*input(writeIdx++, 0) = 0;
	input(writeIdx++, 0) = 0;
	input(writeIdx++, 0) = 0;
	input(writeIdx++, 0) = 0;
	input(id.player_dir, 0) = 1;*/

	// enemy positon, deltaPosition
	int enemyCnt = id.enemies_pos.size();
	for (int i = 0; i < 1; i++)
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
	//int bulletCnt = id.enemies_bullet_pos.size();
	//for (int i = 0; i < 1; i++)
	//{
	//	if (i < bulletCnt)
	//	{
	//		float bx = id.enemies_bullet_pos[i].x + 0.5f;
	//		float by = id.enemies_bullet_pos[i].y + 0.5f;
	//		float dx = 0; // enemy delta x
	//		float dy = 0; // enemy delta y
	//		input(writeIdx++, 0) = (bx - px) / 26.0f;
	//		input(writeIdx++, 0) = (by - py) / 26.0f;
	//		switch (id.enemies_bullet_dir[i])
	//		{
	//		case DIR_UP:
	//			if (by + 0.5f < 25.0f) dy = inverse26; break;
	//		case DIR_DOWN:
	//			if (by - 0.5f > 0.0f) dy = -inverse26; break;
	//		case DIR_LEFT:
	//			if (bx - 0.5f > 0.0f) dx = -inverse26; break;
	//		case DIR_RIGHT:
	//			if (bx + 0.5f < 25.0f) dx = inverse26; break;
	//		}
	//		input(writeIdx++, 0) = dx;
	//		input(writeIdx++, 0) = dy;
	//	}
	//	else
	//	{
	//		input(writeIdx++, 0) = 0;
	//		input(writeIdx++, 0) = 0;
	//		input(writeIdx++, 0) = 0;
	//		input(writeIdx++, 0) = 0;
	//	}
	//}

	if (writeIdx != XDIM) std::cerr << "write index error" << std::endl;
}

void StrategyPJW::ConvertData(const OutputData & od)
{
	// Clear Matrix
	output.fill(0);

	output(od.mov, 0) = 1;

	if (od.shoot)
	{
		output(0, 0) = 0;
		output(1, 0) = 0;
		output(2, 0) = 0;
		output(3, 0) = 0;
		output(4, 0) = 0;
		output(5, 0) = 1;
	}
}