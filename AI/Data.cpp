#include "Data.h"
#include "Game.h"
#include <assert.h>

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
	if (!m_EnableLog)  return;

	m_InputData.Reset();
	m_OutputData.Reset();
}

void CDataManager::EndLog(double deltaTime) {
	if (!m_EnableLog)  return;

	m_accTime += deltaTime;
	if (m_accTime>0.5 && !m_InputData.IsEmpty())
	{
		//printf("accTime: %.3lf\n", m_accTime);
		m_accTime = 0.0;
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
	m_InputData.enemies_pos.push_back(Pos(pos_x, pos_y));
	m_InputData.enemies_dir.push_back(dir);
}

void CDataManager::LogPlayerBullet(int pos_x, int pos_y, DIRECTION dir)
{
	m_InputData.player_bullet_pos.x = pos_x;
	m_InputData.player_bullet_pos.y = pos_y;
	m_InputData.player_dir = dir;
}

void CDataManager::LogEnemyBullet(int pos_x, int pos_y, DIRECTION dir)
{
	m_InputData.enemies_bullet_pos.push_back(Pos(pos_x, pos_y));
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

				render->FillRect(st_x+ebp.x*4,
					st_y+ebp.y*4,
					4*2, 4, render->_magenta);
			}
		}

		if (++idx>=4)  break;
	}
}

void CDataManager::Save(const std::string fileName)
{
	for (auto it=m_IODataVec.begin(); it!=m_IODataVec.end(); ++it)
	{
		const InputData& id = it->first;
		const OutputData& od = it->second;
	}
}

void CDataManager::Load(const std::string fileName)
{
	m_IODataVec.clear();
}
