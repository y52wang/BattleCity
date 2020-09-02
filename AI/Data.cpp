#include "stdafx.h"
#include "Data.h"
#include "Game.h"
#include <assert.h>
#include <fstream>
#include <time.h>

InputData::InputData()
	: player_dir(DIR_NONE)
	, player_bullet_dir(DIR_NONE)
{
}

void InputData::Reset() {
	player_pos.Reset();
	player_dir = DIR_NONE;
	player_bullet_pos.Reset();
	player_bullet_dir = DIR_NONE;

	for (int i=0; i<InputData::HisPosesCnt; ++i)
		player_his_poses[i].Reset();

	enemies_pos.clear();
	enemies_dir.clear();
	enemies_bullet_pos.clear();
	enemies_bullet_dir.clear();
}

bool InputData::IsEmpty() {
	if (player_pos.IsEmpty())  return true;
	return false;
}

InputData InputData::Normalize() const
{
	InputData nid = *this;

	int px = nid.player_pos.x;
	int py = nid.player_pos.y;

	nid.player_bullet_pos.x -= px;
	nid.player_bullet_pos.y -= py;

	for (int i=0; i<InputData::HisPosesCnt; ++i)
	{
		nid.player_his_poses[i].x -= px;
		nid.player_his_poses[i].y -= py;
	}

	for (auto it=nid.enemies_pos.begin(); it!=nid.enemies_pos.end(); ++it)
	{
		it->x -= px;
		it->y -= py;
	}

	for (auto it=nid.enemies_bullet_pos.begin(); it!=nid.enemies_bullet_pos.end(); ++it)
	{
		it->x -= px;
		it->y -= py;
	}

	nid.player_pos.x = 0;
	nid.player_pos.y = 0;

	return nid;
}

InputData InputData::MirrorLR() const
{
	InputData mlrid = *this;

	auto procPosTank = [](Pos& pos)
	{
		pos.x = 26 - pos.x - 2;
	};

	auto procPosBullet = [](Pos& pos)
	{
		pos.x = 26 - pos.x - 2;
	};

	auto procDir = [](DIRECTION& dir)
	{
		if (dir==DIR_LEFT)			dir = DIR_RIGHT;
		else if (dir==DIR_RIGHT)	dir = DIR_LEFT;
	};

	procPosTank(mlrid.player_pos);
	procDir(mlrid.player_dir);

	procPosBullet(mlrid.player_bullet_pos);
	procDir(mlrid.player_bullet_dir);

	std::for_each(mlrid.enemies_pos.begin(), mlrid.enemies_pos.end(), procPosTank);
	std::for_each(mlrid.enemies_dir.begin(), mlrid.enemies_dir.end(), procDir);

	std::for_each(mlrid.enemies_bullet_pos.begin(), mlrid.enemies_bullet_pos.end(), procPosBullet);
	std::for_each(mlrid.enemies_bullet_dir.begin(), mlrid.enemies_bullet_dir.end(), procDir);

	return mlrid;
}

// ------------------------------------------------------------------
OutputData::OutputData()
	: mov(DIR_NONE)
	, shoot(false)
{
}

void OutputData::Reset()
{
	mov = DIR_NONE;
	shoot = false;
}

OutputData OutputData::MirrorLR() const
{
	OutputData mlrod = *this;

	if (mlrod.mov==DIR_LEFT)		mlrod.mov = DIR_RIGHT;
	else if (mlrod.mov==DIR_RIGHT)	mlrod.mov = DIR_LEFT;

	return mlrod;
}

// ------------------------------------------------------------------
const int CDataManager::_MaxListInputData = 180;

CDataManager::CDataManager()
	: m_EnableLog(false)
	, m_accTime(0.0)
{
	m_IODataVec.reserve(1024*8);  // 预留空间大小

	m_InputData.Reset();
	m_OutputData.Reset();
}

void CDataManager::BeginLog() {
	// 每次都清空
	//if (!m_EnableLog)  return;

	m_InputData.Reset();
	m_OutputData.Reset();

	m_Force = false;
}

void CDataManager::ForceLog() {
	m_Force = true;
}

void CDataManager::EndLog(double deltaTime) {
	m_ListInputData.push_front(m_InputData);
	if (m_ListInputData.size() > _MaxListInputData)
		m_ListInputData.pop_back();

	//printf("%ld\n", m_ListInputData.size() );

	if (!m_EnableLog)  return;  // 记录开启的情况下才记录

	m_accTime += deltaTime;
	if ((m_Force || m_accTime>0.5) && !m_InputData.IsEmpty())
	{
		//printf("accTime: %.3lf\n", m_accTime);
		if (!m_Force)  m_accTime = 0.0;
		m_IODataVec.push_back(std::make_pair(m_InputData, m_OutputData));
	}
}

void CDataManager::LogPlayer(int pos_x, int pos_y, DIRECTION dir)
{
	m_InputData.player_pos.x = pos_x;
	m_InputData.player_pos.y = pos_y;
	m_InputData.player_dir = dir;
}

void CDataManager::LogPlayerHisPoses(const int (&poses_x)[InputData::HisPosesCnt],
	const int (&poses_y)[InputData::HisPosesCnt])
{
	for (int i=0; i<InputData::HisPosesCnt; ++i)
	{
		m_InputData.player_his_poses[i].x = poses_x[i];
		m_InputData.player_his_poses[i].y = poses_y[i];
	}
}

void CDataManager::LogEnemy(int pos_x, int pos_y, DIRECTION dir)
{
	assert(pos_x!=-1 && pos_y!=-1);
	m_InputData.enemies_pos.push_back(Pos(pos_x, pos_y));
	m_InputData.enemies_dir.push_back(dir);
}

void CDataManager::LogPlayerBullet(int pos_x, int pos_y, DIRECTION dir)
{
	int pos_x_ = pos_x;
	int pos_y_ = pos_y;

	switch (dir)
	{
	case DIR_UP:
		if (pos_x_>0)  pos_x_ -= 1;
		if (pos_y_>0)  pos_y_ -= 1;
		break;
	case DIR_DOWN:
		if (pos_x_>0)  pos_x_ -= 1;
		break;
	case DIR_LEFT:
		if (pos_x_>0)  pos_x_ -= 1;
		if (pos_y_>0)  pos_y_ -= 1;
		break;
	case DIR_RIGHT:
		if (pos_y_>0)  pos_y_ -= 1;
		break;
	}
	assert(pos_x_>=0);
	assert(pos_y_>=0);
	m_InputData.player_bullet_pos.x = pos_x_;
	m_InputData.player_bullet_pos.y = pos_y_;
	m_InputData.player_bullet_dir = dir;
}

void CDataManager::LogEnemyBullet(int pos_x, int pos_y, DIRECTION dir)
{
	int pos_x_ = pos_x;
	int pos_y_ = pos_y;

	switch (dir)
	{
	case DIR_UP:
		if (pos_x_>0)  pos_x_ -= 1;
		if (pos_y_>0)  pos_y_ -= 1;
		break;
	case DIR_DOWN:
		if (pos_x_>0)  pos_x_ -= 1;
		break;
	case DIR_LEFT:
		if (pos_x_>0)  pos_x_ -= 1;
		if (pos_y_>0)  pos_y_ -= 1;
		break;
	case DIR_RIGHT:
		if (pos_y_>0)  pos_y_ -= 1;
		break;
	}
	assert(pos_x_>=0);
	assert(pos_y_>=0);
	m_InputData.enemies_bullet_pos.push_back(Pos(pos_x_, pos_y_));
	m_InputData.enemies_bullet_dir.push_back(dir);
}

void CDataManager::LogPlayerMove(DIRECTION dir)
{
	m_OutputData.mov = dir;
}

void CDataManager::LogPlayerShoot(bool shoot)
{
	m_OutputData.shoot = shoot;
}

void CDataManager::Draw()
{
	CLevel* level = CGame::Get().Level();
	CRenderer* render = CGame::Get().Renderer();

	int lw = level->LevelWidth();
	int lh = level->LevelHeight();

	int idx = 0;
	// 绘制最后 4 组数据 IOData
	for (auto it=m_IODataVec.rbegin(); it!=m_IODataVec.rend(); ++it)
	{
		int st_x = 500;  // 起始位置 x
		int st_y = 10 + lw*4*idx + 10*idx;  // 起始位置 y
		render->DrawRect(st_x, st_y, lw*4, lh*4, render->_cyan);

		// 绘制 InputData 和 OutputData
		{
			const InputData& id = it->first;
			const OutputData& od = it->second;

			// 注释掉上上行，打开下面这行，则显示镜像数据
			//const InputData& id = it->first.MirrorLR();

			// 绘制 我方
			render->FillRect(st_x+id.player_pos.x*4,
				st_y+id.player_pos.y*4,
				4*2, 4*2, render->_green);

			// 绘制势力图格子
			// 画 2 条横线
			render->DrawLine(st_x+0, st_y+id.player_pos.y*4,
				st_x+lw*4, st_y+id.player_pos.y*4,
				render->_yellow);
			render->DrawLine(st_x+0, st_y+id.player_pos.y*4+4*2,
				st_x+lw*4, st_y+id.player_pos.y*4+4*2,
				render->_yellow);
			// 画 2 条纵线
			render->DrawLine(st_x+id.player_pos.x*4, st_y+0,
				st_x+id.player_pos.x*4, st_y+lh*4,
				render->_yellow);
			render->DrawLine(st_x+id.player_pos.x*4+4*2, st_y+0,
				st_x+id.player_pos.x*4+4*2, st_y+lh*4,
				render->_yellow);

			// 绘制 我方子弹
			switch (id.player_bullet_dir)
			{
			case DIR_UP:
			case DIR_DOWN:
				render->FillRect(st_x+id.player_bullet_pos.x*4,
					st_y+id.player_bullet_pos.y*4,
					8, 4, render->_blue);
				break;
		
			case DIR_LEFT:
			case DIR_RIGHT:
				render->FillRect(st_x+id.player_bullet_pos.x*4,
					st_y+id.player_bullet_pos.y*4,
					4, 8, render->_blue);
				break;
			}

			// 绘制 敌方
			assert(id.enemies_pos.size() == id.enemies_dir.size());
			for (size_t i=0; i<id.enemies_pos.size(); ++i)
			{
				const Pos& ep = id.enemies_pos[i];

				render->FillRect(st_x+ep.x*4,
					st_y+ep.y*4,
					4*2, 4*2, render->_red);
			}

			// 绘制 敌方子弹
			assert(id.enemies_bullet_pos.size() == id.enemies_bullet_dir.size());
			for (size_t i=0; i<id.enemies_bullet_pos.size(); ++i)
			{
				const Pos& ebp = id.enemies_bullet_pos[i];
				const DIRECTION dir = id.enemies_bullet_dir[i];
			
				switch (dir)
				{
				case DIR_UP:
				case DIR_DOWN:
					render->FillRect(st_x+ebp.x*4,
						st_y+ebp.y*4,
						8, 4, render->_magenta);
					break;
			
				case DIR_LEFT:
				case DIR_RIGHT:
					render->FillRect(st_x+ebp.x*4,
						st_y+ebp.y*4,
						4, 8, render->_magenta);
					break;
				}
			}
		}

		if (++idx>=4)  break;
	}
}

// 文件格式：
// 开头四个字节表示数据的数量，后面对应数量的结构体对(InputData, OutputData)
void CDataManager::Save(const std::string fileName)
{
	std::ofstream fs(fileName, std::ios::out | std::ios::binary);
	int dataCount = m_IODataVec.size();
	fs.write((char*)&dataCount, sizeof(int));
	for (auto it = m_IODataVec.begin(); it != m_IODataVec.end(); ++it)
	{
		const InputData& id = it->first;
		const OutputData& od = it->second;

		// input data
		fs.write((char*)&id.player_pos, sizeof(Pos));
		fs.write((char*)&id.player_dir, sizeof(DIRECTION));
		fs.write((char*)&id.player_bullet_pos, sizeof(Pos));
		fs.write((char*)&id.player_bullet_dir, sizeof(DIRECTION));

		for (int i=0; i<InputData::HisPosesCnt; ++i)
			fs.write((const char*)&(id.player_his_poses[i]), sizeof(Pos));

		int enemyCount = id.enemies_pos.size();
		fs.write((char*)&enemyCount, sizeof(int));
		for (int i = 0; i < enemyCount; i++)
		{
			assert(id.enemies_pos[i].x!=-1);
			assert(id.enemies_pos[i].y!=-1);
			fs.write((char*)&id.enemies_pos[i], sizeof(Pos));
			fs.write((char*)&id.enemies_dir[i], sizeof(DIRECTION));
		}

		int enemyBulletCount = id.enemies_bullet_pos.size();
		fs.write((char*)&enemyBulletCount, sizeof(int));
		for (int i = 0; i < enemyBulletCount; i++)
		{
			fs.write((char*)&id.enemies_bullet_pos[i], sizeof(Pos));
			fs.write((char*)&id.enemies_bullet_dir[i], sizeof(DIRECTION));
		}

		// output data
		fs.write((char*)&od.mov, sizeof(DIRECTION));
		fs.write((char*)&od.shoot, sizeof(bool));
	}
	fs.close();
}

void CDataManager::Load(const std::string fileName)
{
	m_IODataVec.clear();
	std::ifstream fs(fileName, std::ios::in | std::ios::binary);
	int dataCount(0);
	fs.read((char*)&dataCount, sizeof(int));
	for (int i = 0; i < dataCount; ++i)
	{
		IOData data;
		InputData id;
		OutputData od;
		fs.read((char*)&id.player_pos, sizeof(Pos));
		fs.read((char*)&id.player_dir, sizeof(DIRECTION));
		fs.read((char*)&id.player_bullet_pos, sizeof(Pos));
		fs.read((char*)&id.player_bullet_dir, sizeof(DIRECTION));

		for (int i=0; i<InputData::HisPosesCnt; ++i)
			fs.read((char*)&(id.player_his_poses[i]), sizeof(Pos));

		int enemyCount(0);
		fs.read((char*)&enemyCount, sizeof(int));
		std::vector<Pos> enemies_pos(enemyCount);
		std::vector<DIRECTION> enemies_dir(enemyCount);
		for (int i = 0; i < enemyCount; i++)
		{
			Pos pos;
			DIRECTION dir;
			fs.read((char*)&pos, sizeof(Pos));
			fs.read((char*)&dir, sizeof(DIRECTION));
			assert(pos.x != -1);
			assert(pos.y != -1);
			enemies_pos[i] = pos;
			enemies_dir[i] = dir;
		}
		id.enemies_pos = enemies_pos;
		id.enemies_dir = enemies_dir;

		int enemyBulletCount(0);
		fs.read((char*)&enemyBulletCount, sizeof(int));
		std::vector<Pos> enemies_bullet_pos(enemyBulletCount);
		std::vector<DIRECTION> enemies_bullet_dir(enemyBulletCount);
		for (int i = 0; i < enemyBulletCount; i++)
		{
			Pos pos;
			DIRECTION dir;
			fs.read((char*)&pos, sizeof(Pos));
			fs.read((char*)&dir, sizeof(DIRECTION));
			enemies_bullet_pos[i] = pos;
			enemies_bullet_dir[i] = dir;
		}
		id.enemies_bullet_pos = enemies_bullet_pos;
		id.enemies_bullet_dir = enemies_bullet_dir;

		// output data
		fs.read((char*)&od.mov, sizeof(DIRECTION));
		fs.read((char*)&od.shoot, sizeof(bool));

		data.first = id;
		data.second = od;
		m_IODataVec.push_back(data);

		//m_IODataVec.push_back(IOData(id.MirrorLR(), od.MirrorLR() ) );
	}
}

void CDataManager::DataBalance(const std::string loadFileName, const std::string saveFileName)
{
	srand((unsigned)time(0));

	Load(loadFileName);

	// 数据统计
	std::cout << "----------Before Data Balance-----------" << std::endl;
	int dataCnt = m_IODataVec.size();
	std::cout << "Data Count: " << dataCnt << std::endl;
	// 0: up
	// 1: down
	// 2: left
	// 3: right
	// 4: none
	// 5: shoot
	std::vector< vector<int> > classIds(6);
	for (int i = 0; i < dataCnt; i++)
	{
		const OutputData& od = m_IODataVec[i].second;
		if (od.shoot) classIds[5].push_back(i);
		else
		{
			switch (od.mov)
			{
			case DIR_UP:
				classIds[0].push_back(i); break;
			case DIR_DOWN:
				classIds[1].push_back(i); break;
			case DIR_LEFT:
				classIds[2].push_back(i); break;
			case DIR_RIGHT:
				classIds[3].push_back(i); break;
			case DIR_NONE:
				classIds[4].push_back(i); break;
			}
		}
	}
	int upCnt = classIds[0].size();
	int downCnt = classIds[1].size();
	int leftCnt = classIds[2].size();
	int rightCnt = classIds[3].size();
	int noneCnt = classIds[4].size();
	int shootCnt = classIds[5].size();
	std::cout << "Move Up    Count: " << upCnt << std::endl;
	std::cout << "Move Down  Count: " << downCnt << std::endl;
	std::cout << "Move Left  Count: " << leftCnt << std::endl;
	std::cout << "Move Right Count: " << rightCnt << std::endl;
	std::cout << "No   Move  Count: " << noneCnt << std::endl;
	std::cout << "Shoot      Count: " << shootCnt << std::endl;
	std::cout << "----------------------------------------" << std::endl;

	// Data Balance
	// 取平均，多的采样，少的镜像/复制补全
	IODataVec newDataSet(0);
	//int maxCnt(shootCnt), minCnt(shootCnt);
	int avg(0);
	for (int i = 0; i < 6; i++)
	{
		int cnt = classIds[i].size();
		avg += cnt;
		//if (cnt > maxCnt) maxCnt = cnt;
		//if (cnt < minCnt) minCnt = cnt;
	}
	avg /= 6;
	
	for (int i = 0; i < 6; i++)
	{
		int cnt = classIds[i].size();
		if (cnt >= avg)
		{
			// 多的采样
			for (int j = 0; j < avg; j++)
			{
				int idx = rand() % cnt;
				newDataSet.push_back(m_IODataVec[classIds[i][idx]]);
			}
		}
		else if (cnt < avg)
		{
			// 少的补
			// 先全部塞进去
			for (int j = 0; j < cnt; j++) newDataSet.push_back(m_IODataVec[classIds[i][j]]);
			// 想办法补全
			if (i != 3 && i != 4)
			{
				// 自身镜像填充
				for (int j = 0; j < (avg - cnt) / 2; j++)
				{
					int idx = rand() % cnt;
					InputData& id = m_IODataVec[classIds[i][idx]].first;
					OutputData& od = m_IODataVec[classIds[i][idx]].second;
					IOData mirror;
					mirror.first = id.MirrorLR();
					mirror.second = od.MirrorLR();
					newDataSet.push_back(m_IODataVec[classIds[i][idx]]);
					newDataSet.push_back(mirror);
				}
			}
			else
			{
				// 对方镜像填充
				for (int j = 0; j < (avg - cnt) / 2; j++)
				{
					int other = i == 3 ? 4 : 3;
					int otherCnt = classIds[other].size();
					int idx = rand() % cnt;
					int otherIdx = rand() % otherCnt;

					InputData& id = m_IODataVec[classIds[other][otherIdx]].first;
					OutputData& od = m_IODataVec[classIds[other][otherIdx]].second;
					IOData mirror;
					mirror.first = id.MirrorLR();
					mirror.second = od.MirrorLR();
					newDataSet.push_back(m_IODataVec[classIds[i][idx]]);
					newDataSet.push_back(mirror);
				}
			}
			
		}
	}
	m_IODataVec.assign(newDataSet.begin(), newDataSet.end());

	Save(saveFileName);
}