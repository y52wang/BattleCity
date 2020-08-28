#include "stdafx.h"
#include "StrategyNN.h"

#define MAP_SIZE 5
#define ACTION 5
StrategyNN::StrategyNN() : CStrategy(MAP_SIZE * MAP_SIZE, ACTION) {}

void StrategyNN::Draw()
{
}

void StrategyNN::SetupNetwork()
{
}

void StrategyNN::ConvertData(const InputData & id)
{
	UpdateInfluenceMap(id);
}

void StrategyNN::ConvertData(const OutputData & od)
{
}

void StrategyNN::UpdateInfluenceMap(const InputData & id)
{
}
