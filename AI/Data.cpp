#include "Data.h"

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
	m_IODataVec.reserve(1024*8);  // Ô¤Áô¿Õ¼ä´óÐ¡
}

void CDataManager::BeginLog() {
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
