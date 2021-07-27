#include "cpp_test.h"

Cls ClsInst;

int f_cpp_test(int x)
{
	return ClsInst.var * x;
}

void f_cpp_init(int x)
{
	ClsInst.var = x;
	ClsInst.Increase();
}

void Cls::Increase()
{
	this->var++;
}

