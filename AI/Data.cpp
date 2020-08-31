#include "stdafx.h"
#include "Data.h"
#include "Game.h"
#include <assert.h>
#include <fstream>

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

	enemies_pos.clear();
	enemies_dir.clear();
	enemies_bullet_pos.clear();
	enemies_bullet_dir.clear();
}

bool InputData::IsEmpty() {
	if (player_pos.IsEmpty())  return true;
	return false;
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

// ------------------------------------------------------------------
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

			// 绘制 我方
			render->FillRect(st_x+id.player_pos.x*4,
				st_y+id.player_pos.y*4,
				4*2, 4*2, render->_green);

			// 绘制势力图格子

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
	}
}
