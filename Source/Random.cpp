#include "Random.h"
#include <random>
#include "Dependencies/PCG/pcg_random.hpp"
#include "Dependencies/PCG/pcg_extras.hpp"

//Random::Random(uint32 num) : num(num)
//{
//}
uint32 Random::GenerateRandomIntRange(int first, int last)
{
	pcg_extras::seed_seq_from<std::random_device> seed;
	pcg32 rng(seed);

	std::uniform_int_distribution<int> uniform_dist1(math::Min(first, last), math::Max(first, last));
	
	return uniform_dist1(rng);
}

uint32 Random::GenerateRandomInt()
{
	pcg_extras::seed_seq_from<std::random_device> seed;
	pcg32 rng(seed);

	std::uniform_int_distribution<uint32> uniform_dist1(0, 429496);
	
	return uniform_dist1(rng);
}

float Random::GenerateRandomFloatRange(float first, float last)
{
	pcg_extras::seed_seq_from<std::random_device> seed;
	pcg32 rng(seed);

	std::uniform_real_distribution<float> uniform_dist1(math::Min(first, last), math::Max(first, last));

	return uniform_dist1(rng);
}

float3 Random::GenerateRandomFloat3(const float3& a, const float3& b)
{
	//return float3(math::Lerp(a.x, b.x, GenerateRandomIntRange(0,1)), math::Lerp(a.y, b.y, GenerateRandomIntRange(0, 1)), math::Lerp(a.z, b.z, GenerateRandomIntRange(0, 1)));


	return float3(GenerateRandomFloatRange(a.x, b.x), GenerateRandomFloatRange(a.y, b.y), GenerateRandomFloatRange(a.z, b.z));

}

float4 Random::GenerateRandomFloat4(const float4& a, const float4& b)
{
	/*return float4(math::Lerp(a.x, b.x, GenerateRandomIntRange(0, 1)), math::Lerp(a.y, b.y, GenerateRandomIntRange(0, 1)),
				  math::Lerp(a.z, b.z, GenerateRandomIntRange(0, 1)), math::Lerp(a.w, b.w, GenerateRandomIntRange(0, 1)));*/

	return float4(GenerateRandomFloatRange(a.x, b.x), GenerateRandomFloatRange(a.y, b.y), GenerateRandomFloatRange(a.z, b.z), GenerateRandomFloatRange(a.w, b.w));

}
