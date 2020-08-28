#ifndef STRATEGYCNN_H_INCLUDED
#define STRATEGYCNN_H_INCLUDED

#include "Strategy.h"
#include "Data.h"

class StrategyCNN : public CStrategy
{
public:
	StrategyCNN();
	virtual ~StrategyCNN() {};
	virtual OutputData MakeDecision(const InputData& id);
	virtual void Train(const IODataVec& database, std::string folder, std::string fileName);
	virtual void LoadParameters(std::string folder, std::string fileName);
	virtual void Draw();

protected:
	Matrix input; //������(26 * 26 * channel, 1)
	Matrix output; //������(6, 1)

	void SetupNetwork();
	void ConvertData(const InputData& id);  // ��� Matrix input
	void ConvertData(const OutputData& od); // ��� Matrix output
	void StrategyCNN::FillObjectToInputMatrix(int channel, const Pos& pos, DIRECTION dir, float baseValue, float addValue);
};

#endif
