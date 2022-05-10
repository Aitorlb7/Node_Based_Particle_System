#ifndef _RANDOM_H_
#define _RANDOM_H_

typedef unsigned __int32 uint32;
#include "Dependencies/MathGeoLib/include/Math/float3.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"

//struct Random {
//public:
//	Random() {};
//	Random(uint32 num);
//	~Random() {};
//
//	uint32 GenerateRandomIntRange(int first, int last);
//	uint32 GenerateRandomInt();
//
//private:
//	uint32 num;
//
//};



namespace Random
{
	uint32 GenerateRandomIntRange(int first, int last);
	uint32 GenerateRandomInt();

	float GenerateRandomFloatRange(float first, float last);

	float3 GenerateRandomFloat3(const float3& a, const float3& b);
	float4 GenerateRandomFloat4(const float4& a, const float4& b);

}

#endif // ! _RANDOM_H_