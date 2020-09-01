#ifndef STRATEGYWL_H_INCLUDED
#define STRATEGYWL_H_INCLUDED

#include "Strategy.h"
#include <SDL2/SDL_rect.h>

// ----------------------------------------------------------------------------
struct InfluenceMethod9
{
	static const int _region_cnt;

	static void CalcInfluence(const InputData& nid, std::vector<float>& out);
	static void DebugDraw(const InputData& nid, const int cx, const int cy);

protected:
	static SDL_Rect GetRectInfluence(const int idx);
};

// ----------------------------------------------------------------------------
struct InfluenceMethodVerHorSquares
{
	static const int _region_cnt;

	static void CalcInfluence(const InputData& nid, std::vector<float>& out);
	static void DebugDraw(const InputData& nid, const int cx, const int cy);

protected:
	static SDL_Rect GetRectInfluence(const int idx);
};

// ----------------------------------------------------------------------------
template <typename InfluenceMethod>
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
