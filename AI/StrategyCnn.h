#ifndef STRATEGYCNN_H_INCLUDED
#define STRATEGYCNN_H_INCLUDED

#include "Strategy.h"

class StrategyCNN : public CStrategy
{
public:
	StrategyCNN();
	virtual ~StrategyCNN() {};
	virtual OutputData MakeDecision(const InputData& id);
	virtual void Train(const IODataVec& database, std::string folder, std::string fileName, float learning_rate = 0.001f, int batch_size = 32, int epoch = 20);
	//virtual void LoadParameters(std::string folder, std::string fileName);
	virtual void Draw();

protected:
	float influence_map[26][26][2]; // 全局势力图

	virtual void SetupNetwork();
	virtual void ConvertData(const InputData& id);  // 填充 Matrix input
	virtual void ConvertData(const OutputData& od); // 填充 Matrix output
	void UpdateInfluenceMap(const InputData& id);
};

#endif
