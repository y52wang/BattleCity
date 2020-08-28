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
	Matrix input; //������(26 * 26 * channel, 1)
	Matrix output; //������(6, 1)

	float influence_map[26][26]; // ȫ������ͼ

	void SetupNetwork();
	void ConvertData(const InputData& id);  // ��� Matrix input
	void ConvertData(const OutputData& od); // ��� Matrix output

	void UpdateInfluenceMap(const InputData& id);
};

#endif