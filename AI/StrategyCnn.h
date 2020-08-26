#ifndef STRATEGY_H_INCLUDED
#define STRATEGY_H_INCLUDED

#include <MiniDNN/MiniDNN.h>
#include "Data.h"

typedef Eigen::MatrixXf Matrix;

class StrategyCNN
{
public:
	bool valid;

	StrategyCNN();
	OutputData MakeDecision(const InputData& id);
	void Train(const IODataVec& database, std::string folder, std::string fileName);
	void LoadParameters(std::string folder, std::string fileName);

private:
	MiniDNN::Network network;
	Matrix input; //������(26 * 26 * channel, 1)
	Matrix output; //������(5, 1)

	void SetupNetwork();
	void ConvertData(const InputData& id);  // ��� Matrix input
	void ConvertData(const OutputData& od); // ��� Matrix output
	void StrategyCNN::FillObjectToInputMatrix(int layer, const Pos& pos, DIRECTION dir, int baseValue);
};

#endif
