#ifndef STRATEGYPJW_H_HEAD
#define STRATEGYPJW_H_HEAD

#include "Strategy.h"
#include "Data.h"

class StrategyPJW : public CStrategy
{
public:
	StrategyPJW();
	virtual ~StrategyPJW() {};
	//virtual OutputData MakeDecision(const InputData& id);
	//virtual void Train(const IODataVec& database, std::string folder, std::string fileName);
	//virtual void LoadParameters(std::string folder, std::string fileName);
	virtual void Draw() {};

protected:
	void SetupNetwork();
	void ConvertData(const InputData& id);  // 野割 Matrix input
	void ConvertData(const OutputData& od); // 野割 Matrix output
};

#endif