#include "stdafx.h"
#include "StrategyPJW.h"
#include "Game.h"

using namespace MiniDNN;

#define XDIM 4 // (enemy pos, enemy next pos)*2; (bullet pos, bullet next pos)*2
#define HDIM 32
#define YDIM 6

StrategyPJW::StrategyPJW() : CStrategy(XDIM, YDIM) { valid = true; }

OutputData StrategyPJW::MakeDecision(const InputData & id)
{
	// 硬编码决策
	OutputData od;
	if (id.enemies_pos.size() > 0)
	{
		ConvertData(id);
		DIRECTION pd = id.player_dir;
		// make decision
		// Hard Code
		//od = MakeDecisionHardCode(input(0,0), input(1, 0), input(2, 0), input(3, 0));
		//ConvertData(od);

		// NN
		output = network.predict(input);
		// 找到最大概率的移动方向
		int label = 4; // 默认为None
		float max = -1.0f;
		for (int i = 0; i < 6; i++)
		{
			float value = output(i, 0);
			if (value > max)
			{
				max = value;
				label = i;
			}
		}
		if(label != 5) od.mov = (DIRECTION)label;
		else
		{
			od.shoot = true;
			od.mov = DIR_NONE;
		}

		// renormalize
		if (od.mov == DIR_UP) od.mov = pd;
		else if (od.mov == DIR_DOWN)
		{
			if (pd == DIR_UP) od.mov = DIR_DOWN;
			else if (pd == DIR_DOWN) od.mov = DIR_UP;
			else if (pd == DIR_LEFT) od.mov = DIR_RIGHT;
			else if (pd == DIR_RIGHT) od.mov = DIR_LEFT;
		}
		else if (od.mov == DIR_LEFT)
		{
			if (pd == DIR_UP) od.mov = DIR_LEFT;
			else if (pd == DIR_DOWN) od.mov = DIR_RIGHT;
			else if (pd == DIR_LEFT) od.mov = DIR_DOWN;
			else if (pd == DIR_RIGHT) od.mov = DIR_UP;
		}
		else if(od.mov == DIR_RIGHT)
		{
			if (pd == DIR_UP) od.mov = DIR_RIGHT;
			else if (pd == DIR_DOWN) od.mov = DIR_LEFT;
			else if (pd == DIR_LEFT) od.mov = DIR_UP;
			else if (pd == DIR_RIGHT) od.mov = DIR_DOWN;
		}
	}
	else
	{
		od.mov = DIR_NONE;
		od.shoot = false;
	}
	return od;
}

void StrategyPJW::Train(const IODataVec & database, std::string folder, std::string fileName, float learning_rate, int batch_size, int epoch)
{
	SetupNetwork();

	int dataCount = database.size();

	Matrix x(x_dim, dataCount);
	Matrix y(y_dim, dataCount);

	int writeIdx(0);
	for (int i = 0; i < dataCount; i++)
	{
		const InputData& id = database[i].first;
		const OutputData& od = database[i].second;

		ConvertData(id);
		DIRECTION pd = id.player_dir;
		OutputData nod = od;
		if (nod.mov == DIR_UP)
		{
			if (pd == DIR_UP) nod.mov = DIR_UP;
			else if (pd == DIR_DOWN) nod.mov = DIR_DOWN;
			else if (pd == DIR_LEFT) nod.mov = DIR_RIGHT;
			else if (pd == DIR_RIGHT) nod.mov = DIR_LEFT;
		}
		else if (od.mov == DIR_DOWN)
		{
			if (pd == DIR_UP) nod.mov = DIR_DOWN;
			else if (pd == DIR_DOWN) nod.mov = DIR_UP;
			else if (pd == DIR_LEFT) nod.mov = DIR_LEFT;
			else if (pd == DIR_RIGHT) nod.mov = DIR_RIGHT;
		}
		else if (nod.mov == DIR_LEFT)
		{
			if (pd == DIR_UP) nod.mov = DIR_LEFT;
			else if (pd == DIR_DOWN) nod.mov = DIR_RIGHT;
			else if (pd == DIR_LEFT) nod.mov = DIR_UP;
			else if (pd == DIR_RIGHT) nod.mov = DIR_DOWN;
		}
		else if (nod.mov == DIR_RIGHT)
		{
			if (pd == DIR_UP) nod.mov = DIR_RIGHT;
			else if (pd == DIR_DOWN) nod.mov = DIR_LEFT;
			else if (pd == DIR_LEFT) nod.mov = DIR_DOWN;
			else if (pd == DIR_RIGHT) nod.mov = DIR_UP;
		}
		ConvertData(nod);

		for (int k = 0; k < x_dim; k++) x(k, writeIdx) = input(k, 0);
		for (int k = 0; k < y_dim; k++) y(k, writeIdx) = output(k, 0);
		writeIdx++;
	}

	// Fake Data
	//Matrix x, y;
	//MakeFakeData(x, y);

	// WangLiang: Dump data to csv --------------------------------------------
	{
		assert(x.cols()==y.cols());
		FILE* fp = fopen("data/TrainDataPJW.csv", "w");
		if (fp!=NULL)
		{
			for (int i=0; i<x.cols(); ++i)
			{
				for (int m=0; m<x.rows(); ++m)
				{
					fprintf(fp, "%8.4f,", x(m, i));
				}
				for (int n=0; n<y.rows(); ++n)
				{
					fprintf(fp, "%8.4f", y(n, i));
					if (n==y.rows()-1)
						fprintf(fp, "\n");
					else
						fprintf(fp, ",");
				}
			}

			fclose(fp);
		}
	}
	// ------------------------------------------------------------------------

	MiniDNN::Adam opt;
	opt.m_lrate = learning_rate;

	MiniDNN::VerboseCallback callback;
	network.set_callback(callback);

	network.init(0, 0.01, 123);

	network.fit(opt, x, y, 32, 200);

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
	Layer* layer11 = new FullyConnected<ReLU>(HDIM, HDIM);
	//Layer* layer111 = new FullyConnected<ReLU>(HDIM, HDIM);
	//Layer* layer2 = new FullyConnected<Sigmoid>(HDIM, YDIM);
	Layer* layer2 = new FullyConnected<Softmax>(HDIM, YDIM);

	network.add_layer(layer1);
	network.add_layer(layer11);
	//network.add_layer(layer111);
	network.add_layer(layer2);

	network.set_output(new RegressionMSE());
	//network.set_output(new MultiClassEntropy());
}

void StrategyPJW::ConvertData(const InputData & id)
{
	if (id.enemies_pos.size() == 0)
	{
		input(0, 0) = 999;
		input(1, 0) = 999;
		input(2, 0) = 999;
		input(3, 0) = 999;
		return;
	}
	int px = id.player_pos.x;
	int py = id.player_pos.y;
	int ex = id.enemies_pos[0].x;
	int ey = id.enemies_pos[0].y;
	int dx = 0;
	int dy = 0;
	DIRECTION ed = id.enemies_dir[0];
	if (ed == DIR_UP && ey < 24) dy = 1;
	else if (ed == DIR_DOWN && ey > 3) dy = -1;
	else if (ed == DIR_LEFT && ex > 1) dx = -1;
	else if (ed == DIR_RIGHT && ex < 24) dx = 1;
	// tank collision
	if (ex >= -2 && ex < 0 && dx > 0) dx = 0;
	if (ex <= 2 && ex > 0 && dx < 0) dx = 0;
	if (ey >= -2 && ey < 0 && dy > 0) dy = 0;
	if (ey <= 2 && ey > 0 && dy < 0) dy = 0;

	// normalize data
	ex = ex - px;
	ey = ey - py;
	DIRECTION pd = id.player_dir;
	if (pd == DIR_DOWN)
	{
		ex = -ex; ey = -ey;
		dx = -dx; dy = -dy;
	}
	else if (pd == DIR_LEFT)
	{
		int tmp = ex; ex = ey; ey = -tmp;
		tmp = dx; dx = dy; dy = -tmp;
	}
	else if (pd == DIR_RIGHT)
	{
		int tmp = ex; ex = -ey; ey = tmp;
		tmp = dx; dx = -dy; dy = tmp;
	}

	// make decision
	// Hard Code
	//od = MakeDecisionHardCode(ex, ey, dx, dy);
	//ConvertData(od);

	// NN
	//input(0, 0) = (float)ex / 26.0f;
	//input(1, 0) = (float)ey / 26.0f;
	input(0, 0) = (float)ex;
	input(1, 0) = (float)ey;
	input(2, 0) = dx;
	input(3, 0) = dy;
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

void StrategyPJW::MakeFakeData(Matrix& x, Matrix& y)
{
	x = Matrix(XDIM, 13520);
	y = Matrix(YDIM, 13520);

	int write = 0;
	// for each enemy position
	OutputData od;
	for (int i = -26; i < 26; i++)
	{
		for (int j = -26; j < 26; j++)
		{
			//x(0, write) = (float)i / 26.0f;
			//x(1, write) = (float)j / 26.0f;
			x(0, write) = (float)i;
			x(1, write) = (float)j;
			x(2, write) = 0;
			x(3, write) = 0;
			od = MakeDecisionHardCode(i, j, 0, 0);
			ConvertData(od);
			for (int k = 0; k < YDIM; k++) y(k, write) = output(k, 0);
			write++;

			//x(0, write) = (float)i / 26.0f;
			//x(1, write) = (float)j / 26.0f;
			x(0, write) = (float)i;
			x(1, write) = (float)j;
			x(2, write) = 1;
			x(3, write) = 0;
			od = MakeDecisionHardCode(i, j, 1, 0);
			ConvertData(od);
			for (int k = 0; k < YDIM; k++) y(k, write) = output(k, 0);
			write++;

			//x(0, write) = (float)i / 26.0f;
			//x(1, write) = (float)j / 26.0f;
			x(0, write) = (float)i;
			x(1, write) = (float)j;
			x(2, write) = -1;
			x(3, write) = 0;
			od = MakeDecisionHardCode(i, j, -1, 0);
			ConvertData(od);
			for (int k = 0; k < YDIM; k++) y(k, write) = output(k, 0);
			write++;

			//x(0, write) = (float)i / 26.0f;
			//x(1, write) = (float)j / 26.0f;
			x(0, write) = (float)i;
			x(1, write) = (float)j;
			x(2, write) = 0;
			x(3, write) = 1;
			od = MakeDecisionHardCode(i, j, 0, 1);
			ConvertData(od);
			for (int k = 0; k < YDIM; k++) y(k, write) = output(k, 0);
			write++;

			//x(0, write) = (float)i / 26.0f;
			//x(1, write) = (float)j / 26.0f;
			x(0, write) = (float)i;
			x(1, write) = (float)j;
			x(2, write) = 0;
			x(3, write) = -1;
			od = MakeDecisionHardCode(i, j, 0, -1);
			ConvertData(od);
			for (int k = 0; k < YDIM; k++) y(k, write) = output(k, 0);
			write++;
		}
	}
	assert(write == 13520);
}

OutputData StrategyPJW::MakeDecisionHardCode(int ex, int ey, int dx, int dy)
{
	OutputData od;
	// player move speed 5.5
	// player bullet speed 13
	// enemy move speed 3
	float pms = 5.5f;
	float pbs = 13;
	float ems = 3;
	float wait_threshold = 0.8;
	float shoot_offset = 1;

	if (abs(ex) > abs(ey))
	{
		od.shoot = false;
		bool canShoot = false;
		bool canWait = false;
		if (ex < 0)
		{
			float t = (float)(-ex) / (pbs - ems * (-dx));
			float ey_t = ey + dy * ems * t;
			if (ey_t >= -shoot_offset && ey_t <= shoot_offset)
			{
				od.mov = DIR_LEFT;
				canShoot = true;
			}
			else if (ey_t < -shoot_offset && dy > 0 || ey_t > shoot_offset && dy < 0)
			{
				if (t < wait_threshold)
				{
					od.mov = DIR_LEFT;
					canShoot = true;
				}
				else
				{
					canWait = true;
				}
			}
		}
		else if (ex > 0)
		{
			float t = (float)ex / (pbs - ems * dx);
			float ey_t = ey + dy * ems * t;
			if (ey_t >= -shoot_offset && ey_t <= shoot_offset)
			{
				od.mov = DIR_RIGHT;
				canShoot = true;
			}
			else if (ey_t < -shoot_offset && dy > 0 || ey_t > shoot_offset && dy < 0)
			{
				if (t < wait_threshold)
				{
					od.mov = DIR_RIGHT;
					canShoot = true;
				}
				else
				{
					canWait = true;
				}
			}
		}

		if (!canShoot)
		{
			if (ey > 0)
			{
				if (dy >= 0 || canWait)
				{
					od.mov = DIR_UP;
				}
				else
				{
					od.mov = DIR_DOWN;
				}
			}
			else
			{
				if (dy <= 0 || canWait)
				{
					od.mov = DIR_DOWN;
				}
				else
				{
					od.mov = DIR_UP;
				}
			}
		}
	}
	else
	{
		if (ey < 0)
		{
			od.mov = DIR_DOWN;
			od.shoot = false;
		}
		else
		{
			float t = (float)ey / (pbs - ems * dy);
			float ex_t = ex + dx * ems * t;
			if (ex_t >= -shoot_offset && ex_t <= shoot_offset)
			{
				// 经计算能打中，直接开炮
				od.mov = DIR_NONE;
				od.shoot = true;
			}
			else
			{
				if (ex_t < -shoot_offset && dx > 0 && t < wait_threshold || ex_t > shoot_offset && dx < 0 && t < wait_threshold)
				{
					// 敌人正在进入火力范围，守株待兔
					od.mov = DIR_NONE;
					od.shoot = false;
				}
				else
				{
					od.mov = ex < 0 ? DIR_LEFT : DIR_RIGHT;
					od.shoot = false;
				}
			}
		}
	}
	return od;
}
