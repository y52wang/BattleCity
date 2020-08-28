#include "stdafx.h"
#include "Strategy.h"

OutputData CStrategy::MakeDecision(const InputData& id)
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
	//od.shoot = output(5, 0) > 0.5 ? true : false;
	return od;
}
void CStrategy::Train(const IODataVec& database, std::string folder, std::string fileName, int x_dim, int y_dim, float learning_rate, int batch_size, int epoch)
{
	SetupNetwork();

	int dataCount = database.size();

	Matrix x(x_dim, dataCount);
	Matrix y(y_dim, dataCount);

	for (int i = 0; i < dataCount; i++)
	{
		ConvertData(database[i].first);
		ConvertData(database[i].second);

		for (int k = 0; k < x_dim; k++) x(k, i) = input(k, 0);
		for (int k = 0; k < y_dim; k++) y(k, i) = output(k, 0);
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
void CStrategy::LoadParameters(std::string folder, std::string fileName)
{
	try
	{
		network.read_net(folder, fileName);
		valid = true;
	}
	catch (std::exception e)
	{
		std::cerr << "[Strategy] LoadParameters Error.\n";
		valid = false;
	}
}