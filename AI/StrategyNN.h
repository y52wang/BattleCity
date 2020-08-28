#ifndef STRATEGY_NN_HEAD
#define STRATEGY_NN_HEAD

#include "Data.h"
#include "Strategy.h"

class StrategyNN : public CStrategy
{
public:
	StrategyNN();
	virtual ~StrategyNN() {};
	virtual OutputData MakeDecision(const InputData& id);
	virtual void Train(const IODataVec& database, std::string folder, std::string fileName);
	virtual void LoadParameters(std::string folder, std::string fileName);
	virtual void Draw();

protected:
	Matrix input; //列向量(26 * 26 * channel, 1)
	Matrix output; //列向量(6, 1)

	float influence_map[26][26]; // 全局势力图

	void SetupNetwork();
	void ConvertData(const InputData& id);  // 填充 Matrix input
	void ConvertData(const OutputData& od); // 填充 Matrix output

	void UpdateInfluenceMap(const InputData& id);
};

#endif