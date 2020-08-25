#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include <vector>
#include "Types.h"

struct Pos {
	int x, y;
};

// ����ϵ��x ���ң�y ���ϣ�ԭ�������½�
struct InputData {
	// Player Data
	Pos player_pos; // ռ�ĸ����ӵ����꣬��߸����������½�����
	DIRECTION player_dir; // enum DIRECTION
	Pos player_bullet_pos;
	DIRECTION player_bullet_dir;
	// Enermy Data
	std::vector<Pos> enermies_pos; // length=enermy_cnt
	std::vector<DIRECTION> enermies_dir; // length=enermy_cnt
	std::vector<Pos> enermies_bullet_pos; // length=enermy_bullet_cnt
	std::vector<DIRECTION> enermies_bullet_dir; // length=enermy_bullet_cnt
};

struct OutputData {
	DIRECTION mov;
	bool shoot;
};

typedef std::pair<InputData, OutputData>    IOData;
typedef std::vector<IOData>                 IODataVec;

class DataManager {
public:
	DataManager();

	IODataVec       m_IODataVec;
};

#endif
