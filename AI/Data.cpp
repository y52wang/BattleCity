#include "Data.h"
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
{
	m_IODataVec.reserve(1024*8);  // Ԥ���ռ��С
}

void CDataManager::BeginLog() {
	m_InputData.Reset();
	m_OutputData.Reset();
}

void CDataManager::EndLog() {
	if (!m_EnableLog)  return;

	if (!m_InputData.IsEmpty())
		m_IODataVec.push_back(std::make_pair(m_InputData, m_OutputData));
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

// �ļ���ʽ��
// ��ͷ�ĸ��ֽڱ�ʾ���ݵ������������Ӧ�����Ľṹ���(InputData, OutputData)
void CDataManager::Save(const std::string fileName)
{
	std::ofstream fs(fileName, std::ios::out | std::ios::binary);
	int dataCount = m_IODataVec.size();
	fs.write((char*)&dataCount, sizeof(int));
	for (auto it=m_IODataVec.begin(); it!=m_IODataVec.end(); ++it)
	{
		const InputData& id = it->first;
		const OutputData& od = it->second;

		fs.write((char*)&id, sizeof(InputData));
		fs.write((char*)&id, sizeof(OutputData));
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
		fs.read((char*)&data.first, sizeof(InputData));
		fs.read((char*)&data.second, sizeof(OutputData));
		m_IODataVec.push_back(data);
	}
}
