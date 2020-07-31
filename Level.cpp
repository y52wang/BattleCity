/**************************************************
**************************************************/

#include "Level.h"
#include <assert.h>

CLevel::CLevel()
	: m_level_width(26)
	, m_level_height(26)
	, m_all_levels(20)
{
}

void CLevel::LoadLevel(const string fileName)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	assert(fp);

	m_level.data.clear();
	m_bricks.clear();

	fscanf(fp, "%d", &m_level.m_level_num);
	for (int i=0; i<m_level_height; ++i)  {
		vector<LVL_FIELD>	vecLF;
		vector<sBrick>		vecB;
		for (int j=0; j<m_level_width; ++j)  {
			int d = 0;
			fscanf(fp, "%d", &d);
			vecLF.push_back((LVL_FIELD)d);
			vecB.push_back(sBrick() );
		}
		m_level.data.push_back(vecLF);
		m_bricks.push_back(vecB);
	}

	fclose(fp);
}

void CLevel::DrawLevel(bool up)
{
}

void CLevel::DrawBackground()
{
}

void CLevel::DestroyTile(int x1, int y1, int x2, int y2, int power, DIRECTION dir)
{
}

void CLevel::SaveLevel()
{
}
