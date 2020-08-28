#ifndef STRATEGY_H_INCLUDED
#define STRATEGY_H_INCLUDED

#include "stdafx.h"
#include "Data.h"

typedef Eigen::MatrixXf Matrix;

class CStrategy
{
public:
	bool valid;

	CStrategy() : valid(false) {};
	virtual ~CStrategy() {}
	virtual OutputData MakeDecision(const InputData& id) = 0;
	virtual void Train(const IODataVec& database, std::string folder, std::string fileName) = 0;
	virtual void LoadParameters(std::string folder, std::string fileName) = 0;
	virtual void Draw() = 0;

protected:
	MiniDNN::Network network;
};

#endif