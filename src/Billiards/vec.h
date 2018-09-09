#pragma once
struct vec
{
	float x;
	float y;
	float z;
	vec();
	vec(float* v);
	vec(float _x, float _y, float _z);
	void Set(float _x, float _y, float _z);
	friend vec operator -(const vec &x, const vec &y);
	friend vec operator +(const vec &x, const vec &y);
	friend vec operator *(float x, const vec &y);
	friend vec operator /(const vec &a,float b);
	float* toFloat();
	void LoadZero();
	const vec& CrossProduct(const vec &a);
	void Normalize();
	float GetLength();
}; 