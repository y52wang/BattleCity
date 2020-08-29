#ifndef STRATEGY_H_INCLUDED
#define STRATEGY_H_INCLUDED

#include "Data.h"

typedef Eigen::MatrixXf Matrix;

class CStrategy
{
public:
	bool valid;

	CStrategy(int x_dim, int y_dim) : x_dim(x_dim),y_dim(y_dim),input(x_dim,1),output(y_dim,1), valid(false) {}
	virtual ~CStrategy() {}
	virtual OutputData MakeDecision(const InputData& id);
	virtual void Train(const IODataVec& database, std::string folder, std::string fileName, float learning_rate = 0.001f, int batch_size = 32, int epoch = 20);
	virtual void LoadParameters(std::string folder, std::string fileName);
	virtual void Draw() {};

protected:
	MiniDNN::Network network;
	int x_dim;
	int y_dim;

	Matrix input; //列向量
	Matrix output; //列向量

	virtual void SetupNetwork() {};
	virtual void ConvertData(const InputData& id) {};  // 填充 Matrix input
	virtual void ConvertData(const OutputData& od) {}; // 填充 Matrix output
};

#endif