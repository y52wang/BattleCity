#ifndef STRATEGYNN_H_HEAD
#define STRATEGYNN_H_HEAD

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
	float influence_map[26][26]; // ȫ������ͼ

	virtual void SetupNetwork();
	virtual void ConvertData(const InputData& id);  // ��� Matrix input
	virtual void ConvertData(const OutputData& od); // ��� Matrix output

	void UpdateInfluenceMap(const InputData& id);
};

#endif