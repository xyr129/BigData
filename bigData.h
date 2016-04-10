#ifndef BIG_DATA_H
#define BIG_DATA_H
#include<iostream>
#include<string>
using namespace std;

typedef long long INT64;

#define MIN_INT64 0x8000000000000000
#define MAX_INT64 0x7fffffffffffffff
#define UN_INT64  0xcccccccccccccccc

class BigData
{
public:
	BigData(INT64 data = UN_INT64);
	BigData(const char* _str);
	BigData operator+(const BigData& bd);
	BigData operator-(const BigData& bd);
	BigData operator*(const BigData& bd);
	BigData operator/(const BigData& bd);
	BigData operator%(const BigData& bd);
	friend ostream& operator<<(ostream& os,const BigData& bd);
	friend istream& operator>>(istream& is,BigData& bd);
	bool IsINT64OverFlow()const;
	bool IsLeftStrBig(char* pLeft, size_t LSize, char *pRight, size_t RSize);

private:
	string Add(string left, string right);
	string Sub(string left, string right);
	string Mul(string left, string right);
	string Div(string left, string right);
	string Mod(string left, string right);
	void Int64ToStr();
	int subLoop(char* pLeft, size_t iLSize, char *pRight, size_t iRSize);
private:
	long long m_llValue;
	string m_strData;
};

#endif