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

	// export_net�ļ����Ѵ���ʱ�ᱨ����Network.h�ж�Ӧ��ע�͵����Լ������ļ��У���ѵ��ǰ���ļ���ɾ�� �����޸���
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