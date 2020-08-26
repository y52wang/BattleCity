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
	Matrix input; //列向量(26 * 26 * channel, 1)
	Matrix output; //列向量(5, 1)

	void SetupNetwork();
	void ConvertData(const InputData& id);  // 填充 Matrix input
	void ConvertData(const OutputData& od); // 填充 Matrix output
	void StrategyCNN::FillObjectToInputMatrix(int layer, const Pos& pos, DIRECTION dir, int baseValue);
};

#endif
