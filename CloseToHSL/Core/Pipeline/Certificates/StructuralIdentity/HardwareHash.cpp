#include "HardwareHash.h"

namespace YAKSys_Hybrid_CPU::Core
{

	void HardwareHash::Initialize(unsigned long long k0, unsigned long long k1)
	{
		_v0 = k0 ^ 0x736f6d6570736575ULL;
		_v1 = k1 ^ 0x646f72616e646f6dULL;
		_v2 = k0 ^ 0x6c7967656e657261ULL;
		_v3 = k1 ^ 0x7465646279746573ULL;
	}

	void HardwareHash::Compress(unsigned long long word)
	{
		_v3 ^= word;
		SipRound();
		SipRound();
		_v0 ^= word;
	}

	unsigned long long HardwareHash::Finalize()
	{
		_v2 ^= 0xFF;
		SipRound();
		SipRound();
		SipRound();
		SipRound();
		return _v0 ^ _v1 ^ _v2 ^ _v3;
	}

	inline void HardwareHash::SipRound()
	{
		_v0 += _v1;
		_v1 = RotateLeft(_v1, 13);
		_v1 ^= _v0;
		_v0 = RotateLeft(_v0, 32);
		_v2 += _v3;
		_v3 = RotateLeft(_v3, 16);
		_v3 ^= _v2;
		_v0 += _v3;
		_v3 = RotateLeft(_v3, 21);
		_v3 ^= _v0;
		_v2 += _v1;
		_v1 = RotateLeft(_v1, 17);
		_v1 ^= _v2;
		_v2 = RotateLeft(_v2, 32);
	}

	inline unsigned long long HardwareHash::RotateLeft(unsigned long long x, int n)
	{
		return (x << n) | (x >> (64 - n));
	}
}
