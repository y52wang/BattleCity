#include "stdafx.h"
#include "StrategyCnn.h"

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
#define INDEX(x,y,c) ((c)*26*26+(x)*26+(y)) // c: channel

StrategyCNN::StrategyCNN()
{
	input = Matrix::Zero(26 * 26 * CHANNEL, 1); // (player, enemies, enemies' bullet)
	output = Matrix::Zero(6, 1); // 4 Move Direction & None & shoot
	valid = false;
}

OutputData StrategyCNN::MakeDecision(const InputData & id)
{
	OutputData od;
	if (!valid)
	{
		std::cout << "[StrategyCnn] MakeDecision Failed. Network is not valid.\n";
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
	od.shoot = output(5, 0) > 0.5 ? true : false;

	return od;
}

void StrategyCNN::Train(const IODataVec & database, std::string folder, std::string fileName)
{
	SetupNetwork();

	int dataCount = database.size();
	int featureCount = 26 * 26 * CHANNEL;

	Matrix x(featureCount, dataCount);
	Matrix y(6, dataCount);

	for (int i = 0; i < dataCount; i++)
	{
		ConvertData(database[i].first);
		ConvertData(database[i].second);

		for (int k = 0; k < featureCount; k++) x(k, i) = input(k, 0);
		for (int k = 0; k < 6; k++) y(k, i) = output(k, 0);
	}

	Adam opt;
	opt.m_lrate = 0.001;

	VerboseCallback callback;
	network.set_callback(callback);

	network.init(0, 0.01, 123);

	network.fit(opt, x, y, 64, 5);

	valid = true;

	// export_net文件夹已存在时会报错，把Network.h中对应行注释掉，自己创建文件夹，或训练前把文件夹删除 （已修复）
	network.export_net(folder, fileName);
}

void StrategyCNN::LoadParameters(std::string folder, std::string fileName)
{
	try
	{
		network.read_net(folder, fileName);
		valid = true;
	}
	catch(std::exception e)
	{
		std::cerr << "[Strategy] LoadParameters Error.\n";
		valid = false;
	}
}

void StrategyCNN::SetupNetwork()
{
	Layer* conv1 = new Convolutional<ReLU>(26, 26, CHANNEL, 16, 7, 7);
	Layer* pool1 = new MaxPooling<Identity>(20, 20, 16, 4, 4);

	//Layer* conv2 = new Convolutional<ReLU>(12, 12, 32, 64, 5, 5);
	//Layer* pool2 = new MaxPooling<Identity>(8, 8, 64, 2, 2);

	//Layer* fully1 = new FullyConnected<ReLU>(4 * 4 * 64, 512);
	Layer* fully1 = new FullyConnected<ReLU>(5 * 5 * 16, 128);
	Layer* out = new FullyConnected<Sigmoid>(128, 6);

	network.add_layer(conv1);
	network.add_layer(pool1);
	//network.add_layer(conv2);
	//network.add_layer(pool2);
	network.add_layer(fully1);
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
	input(INDEX(pos.x, pos.y, channel), 0) = baseValue;
	input(INDEX(pos.x + 1, pos.y, channel), 0) = baseValue;
	input(INDEX(pos.x, pos.y + 1, channel), 0) = baseValue;
	input(INDEX(pos.x + 1, pos.y + 1, channel), 0) = baseValue;
	switch (dir)
	{
	case DIR_UP:
		input(INDEX(pos.x, pos.y + 1, channel), 0) += addValue;
		input(INDEX(pos.x + 1, pos.y + 1, channel), 0) += addValue;
		break;
	case DIR_RIGHT:
		input(INDEX(pos.x + 1, pos.y, channel), 0) += addValue;
		input(INDEX(pos.x + 1, pos.y + 1, channel), 0) += addValue;
		break;
	case DIR_DOWN:
		input(INDEX(pos.x, pos.y, channel), 0) += addValue;
		input(INDEX(pos.x + 1, pos.y, channel), 0) += addValue;
		break;
	case DIR_LEFT:
		input(INDEX(pos.x, pos.y, channel), 0) += addValue;
		input(INDEX(pos.x, pos.y + 1, channel), 0) += addValue;
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

	if (od.shoot) output(5, 0) = 1;
}
