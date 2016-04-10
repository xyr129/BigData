#include"bigData.h"

int main()
{
	BigData bd1("9223372036854775809");
	BigData bd2(234); 
	cout << bd1/bd2 << endl;

	system("pause");
	return 0;
}