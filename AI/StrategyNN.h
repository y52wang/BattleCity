#ifndef STRATEGYNN_H_HEAD
#define STRATEGYNN_H_HEAD

#include "Data.h"
#include "Strategy.h"

class StrategyNN : public CStrategy
{
public:
	StrategyNN();
	virtual ~StrategyNN() {};
	//virtual OutputData MakeDecision(const InputData& id);
	//virtual void Train(const IODataVec& database, std::string folder, std::string fileName);
	//virtual void LoadParameters(std::string folder, std::string fileName);
	virtual void Draw();

protected:
	float influence_map[26][26]; // 全局势力图

	void SetupNetwork();
	void ConvertData(const InputData& id);  // 填充 Matrix input
	void ConvertData(const OutputData& od); // 填充 Matrix output

	void UpdateInfluenceMap(const InputData& id);
};

#endif