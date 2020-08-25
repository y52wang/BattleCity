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
{
}

void CDataManager::BeginLog() {
	m_InputData.Reset();
	m_OutputData.Reset();
}

void CDataManager::EndLog() {
	if (!m_EnableLog)  return;
}

void CDataManager::LogPlayer(int pos_x, int pos_y, DIRECTION dir)
{
}

void CDataManager::LogEnemy(int pos_x, int pos_y, DIRECTION dir)
{
}

void CDataManager::LogPlayerBullet(int pos_x, int pos_y, DIRECTION dir)
{
}

void CDataManager::LogEnemyBullet(int pos_x, int pos_y, DIRECTION dir)
{
}
