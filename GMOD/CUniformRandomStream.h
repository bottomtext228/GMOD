#pragma once


class CUniformRandomStream
{
public:
	//Don't forget the constructor.
	/*0*/	virtual void SetSeed(int) = 0;
	/*1*/	virtual float RandomFloat(float, float) = 0;
	/*2*/	virtual int RandomInt(int, int) = 0;
	/*3*/	virtual float RandomFloatExp(float, float, float) = 0;
};