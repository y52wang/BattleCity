#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include <vector>
#include "Types.h"

struct Pos {
	int x, y;

	Pos(int _x=-1, int _y=-1) : x(_x), y(_y) {}

	void Reset() {
		x = -1;
		y = -1;
	}
};

// ����ϵ��x ���ң�y ���ϣ�ԭ�������½�
struct InputData {
	// Player Data
	Pos player_pos; // ռ�ĸ����ӵ����꣬��߸����������½�����
	DIRECTION player_dir; // enum DIRECTION
	Pos player_bullet_pos;
	DIRECTION player_bullet_dir;
	// Enermy Data
	std::vector<Pos> enemies_pos; // length=enemy_cnt
	std::vector<DIRECTION> enemies_dir; // length=enemy_cnt
	std::vector<Pos> enemies_bullet_pos; // length=enemy_bullet_cnt
	std::vector<DIRECTION> enemies_bullet_dir; // length=enemy_bullet_cnt

	InputData();
	void Reset();
};

struct OutputData {
	DIRECTION mov;
	bool shoot;

	OutputData();
	void Reset();
};

typedef std::pair<InputData, OutputData>    IOData;
typedef std::vector<IOData>                 IODataVec;

class CDataManager {
public:
	CDataManager();

	void BeginLog();
	void EndLog();

	void LogPlayer(int pos_x, int pos_y, DIRECTION dir);
	void LogEnemy(int pos_x, int pos_y, DIRECTION dir);
	void LogBullet(int pos_x, int pos_y, DIRECTION dir);

    bool IsEnableLog()    { return m_EnableLog; }
	void EnableLog(bool enable)	{ m_EnableLog = enable; }

	IODataVec       m_IODataVec;

protected:
    bool			m_EnableLog;    // ���ݼ�¼ ����/�ر�

	InputData		m_InputData;
	OutputData		m_OutputData;
};

#endif
