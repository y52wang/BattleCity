#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include <vector>
#include "../Types.h"

// 坐标系，x 向右，y 向上，原点在左下角
struct InputData {
	// Player Data
	int player_pos[8]; // 占四个格子的坐标
	DIRECTION player_dir; // enum DIRECTION
	int player_bullet_pos[8];
	DIRECTION player_bullet_dir;
	// Enermy Data
	std::vector<int[8]> enermies_pos; // length=enermy_cnt
	std::vector<DIRECTION> enermies_dir; // length=enermy_cnt
	std::vector<int[8]> enermies_bullet_pos; // length=enermy_bullet_cnt
	std::vector<DIRECTION> enermies_bullet_dir; // length=enermy_bullet_cnt
};

struct OutputData {
	DIRECTION mov;
	bool shoot;
};

class DataManager {
};

#endif
