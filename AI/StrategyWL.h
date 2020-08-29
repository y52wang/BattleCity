#ifndef STRATEGYWL_H_INCLUDED
#define STRATEGYWL_H_INCLUDED

#include "Strategy.h"

class StrategyWL : public CStrategy
{
public:
	StrategyWL();
	virtual ~StrategyWL()  {};

	virtual void Draw();

protected:
	virtual void SetupNetwork();
	virtual void ConvertData(const InputData& id);
	virtual void ConvertData(const OutputData& od);
};

#endif
