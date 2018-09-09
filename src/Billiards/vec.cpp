#include"vec.h"
#include<math.h>
vec::vec() { }
vec::vec(float _x, float _y, float _z) :
	x(_x), y(_y), z(_z) { }

vec::vec(float* v)
{
	x = v[0], y = v[1], z = v[2];
}

void vec::Set(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

void vec::LoadZero()
{
	x = y = z = 0;
}

vec operator -(const vec &a, const vec &b)
{
	vec ans(a.x - b.x, a.y - b.y, a.z - b.z);
    return ans;
}

vec operator +(const vec &a, const vec &b)
{
	vec ans(a.x + b.x, a.y + b.y, a.z + b.z);
	return ans;
}

float vec::GetLength()
{
	return sqrt(x*x + y*y + z*z);
}

void vec::Normalize()
{
	float len = GetLength();
	x = x / len;
	y = y / len;
	z = z / len;
}

vec operator *(float x, const vec &y)
{
	vec ans(x*y.x, x*y.y, x*y.z);
	return ans;
}

vec operator /(const vec &a, float b)
{
	vec ans(a.x / b, a.y / b, a.z / b);
	return ans;
}

float* vec::toFloat()
{
	float* ans = new float[3];
	return ans;
}

const vec& vec::CrossProduct(const vec &a)
{
	vec ans(y*a.z - a.y*z, z*a.x - a.z*x, x*a.y - a.x*y);
	return ans;
}