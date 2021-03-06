#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include <list>
#include <string>
#include <vector>
#include "Types.h"

struct Pos {
	int x, y;

	Pos(int _x=-1, int _y=-1) : x(_x), y(_y) {}

	void Reset() {
		x = -1;
		y = -1;
	}

	bool IsEmpty() {
		return x==-1 && y==-1;
	}
};

// 坐标系，x 向右，y 向上，原点在左下角
struct InputData {
	enum
	{
		HisPosesCnt = 5,
	};

	// Player Data
	Pos						player_pos; // 占四个格子的坐标，这边给出的是左下角坐标
	DIRECTION				player_dir; // enum DIRECTION
	Pos						player_bullet_pos;  // 占二个格子，给出的是左下角坐标
	DIRECTION				player_bullet_dir;
	Pos						player_his_poses[HisPosesCnt];

	// Enermy Data
	std::vector<Pos>		enemies_pos; // length=enemy_cnt
	std::vector<DIRECTION>	enemies_dir; // length=enemy_cnt
	std::vector<Pos>		enemies_bullet_pos; // length=enemy_bullet_cnt
	std::vector<DIRECTION>	enemies_bullet_dir; // length=enemy_bullet_cnt

	InputData();
	void Reset();
	bool IsEmpty();

	// 标准化处理，player_pos 设置为原点
	InputData Normalize() const;  // Normalize 后 MirrorLR 不再支持
	// 镜像数据（左右）
	InputData MirrorLR() const;
};

struct OutputData {
	DIRECTION	mov;
	bool		shoot;

	OutputData();
	void Reset();

	// 镜像数据（左右）
	OutputData MirrorLR() const;
};

typedef std::pair<InputData, OutputData>    IOData;
typedef std::vector<IOData>                 IODataVec;

typedef std::list<InputData>				ListInputData;
typedef ListInputData::iterator				ListInputDataIter;

class CStrategy;

class CDataManager {
public:
	CDataManager();

	void BeginLog();
	void ForceLog();
	// deltaTime 间隔时间
	// 累积间隔时间超过一定限度才进行记录
	void EndLog(double deltaTime);

	// Log Input Data
	void LogPlayer(int pos_x, int pos_y, DIRECTION dir);
	void LogPlayerHisPoses(const int (&poses_x)[InputData::HisPosesCnt], const int (&poses_y)[InputData::HisPosesCnt]);
	void LogEnemy(int pos_x, int pos_y, DIRECTION dir);

	void LogPlayerBullet(int pos_x, int pos_y, DIRECTION dir);
	void LogEnemyBullet(int pos_x, int pos_y, DIRECTION dir);

	// Log Output Data
	void LogPlayerMove(DIRECTION dir);
	void LogPlayerShoot(bool shoot);

	void Draw();

    bool IsEnableLog()    { return m_EnableLog; }
	void EnableLog(bool enable)	{ m_EnableLog = enable; }

	void DumpTrainDataToCSV(const std::string& fileName, CStrategy* stg);

	void Save(const std::string& fileName);
	void Load(const std::string& fileName);

	void DataBalance(const std::string loadFileName, const std::string saveFileName);

	IODataVec       m_IODataVec;

	InputData		m_InputData;
	OutputData		m_OutputData;

	ListInputData	m_ListInputData;

	static const int	_MaxListInputData;

protected:
    bool			m_EnableLog;    // 数据记录 开启/关闭
	bool			m_Force;  // 本次 Log 忽略 m_accTime 的累积时间，强制写入
	double			m_accTime;
};

#endif
