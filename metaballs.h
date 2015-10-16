#include "vector3d.h"
class METABALL
{
public:
	VECTOR3D position;
	float squaredRadius;

	void Init(VECTOR3D newPosition, float newSquaredRadius)
	{
		position = newPosition;
		squaredRadius = newSquaredRadius;
	}
};