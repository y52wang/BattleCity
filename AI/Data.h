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

// ����ϵ��x ���ң�y ���ϣ�ԭ�������½�
struct InputData {
	enum
	{
		HisPosesCnt = 5,
	};

	// Player Data
	Pos						player_pos; // ռ�ĸ����ӵ����꣬��߸����������½�����
	DIRECTION				player_dir; // enum DIRECTION
	Pos						player_bullet_pos;  // ռ�������ӣ������������½�����
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

	// ��׼������player_pos ����Ϊԭ��
	InputData Normalize() const;  // Normalize �� MirrorLR ����֧��
	// �������ݣ����ң�
	InputData MirrorLR() const;
};

struct OutputData {
	DIRECTION	mov;
	bool		shoot;

	OutputData();
	void Reset();

	// �������ݣ����ң�
	OutputData MirrorLR() const;
};

typedef std::pair<InputData, OutputData>    IOData;
typedef std::vector<IOData>                 IODataVec;

typedef std::list<InputData>				ListInputData;
typedef ListInputData::iterator				ListInputDataIter;

class CDataManager {
public:
	CDataManager();

	void BeginLog();
	void ForceLog();
	// deltaTime ���ʱ��
	// �ۻ����ʱ�䳬��һ���޶ȲŽ��м�¼
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

	void Save(const std::string fileName);
	void Load(const std::string fileName);

	IODataVec       m_IODataVec;

	InputData		m_InputData;
	OutputData		m_OutputData;

	ListInputData	m_ListInputData;

	static const int	_MaxListInputData;

protected:
    bool			m_EnableLog;    // ���ݼ�¼ ����/�ر�
	bool			m_Force;  // ���� Log ���� m_accTime ���ۻ�ʱ�䣬ǿ��д��
	double			m_accTime;
};

#endif
