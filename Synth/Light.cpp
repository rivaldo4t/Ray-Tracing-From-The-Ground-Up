#include "Light.hpp"

Light :: Light(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir) : pos(_pos), color(_color), dir(_dir)
{
	dir = dir.IsZero() ? dir : dir.GetNormalized();
}

AreaLight :: AreaLight(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir, cyPoint3d _updir,
	double _scaleX, double _scaleY, int _pxmax, int _pymax, 
	int _pxsub, int _pysub) :
	pos(_pos), color(_color), dir(_dir), updir(_updir), scaleX(_scaleX), scaleY(_scaleY), 
	pxmax(_pxmax), pymax(_pymax), pxsub(_pxsub), pysub(_pysub)
{
	n2 = dir.GetNormalized();
	n0 = dir.Cross(updir).GetNormalized();
	n1 = n0.Cross(n2).GetNormalized();
}