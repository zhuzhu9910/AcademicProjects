#include "test.h"

int gob = 4;
const int akb = 9;

test2::test2(int m) :mm(m) { };

int test2::test2k(char c, int i, long h)
{
	return c + i + h;
}

#ifdef TEST_MAIN
void main()
{
	test2 t2(9);
	int j = t2.test2k('a',1,2);
	int* k = new int[7];
	testc tc;
	tc.tc(2);
	test1 t1(0);
	t1.f();
	
	//delete[] k;
}

#endif