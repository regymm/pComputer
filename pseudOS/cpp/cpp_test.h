#ifndef PSEUDOS_CPP_TEST_H
#define PSEUDOS_CPP_TEST_H

extern "C" int f_cpp_test(int);
extern "C" void f_cpp_init(int);
int f_cpp_test(int x);
void f_cpp_init(int x);

class Cls {
public:
	Cls() {
	}
	Cls(int v) {
		this->var = v;
	}
	void Increase();
	int var;
};

extern Cls ClsInst;

#endif
