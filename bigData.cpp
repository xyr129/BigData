#include "bigData.h"
#include <cassert>

BigData::BigData(INT64 data)
	:m_llValue(data)
	, m_strData("")
	{
		Int64ToStr();
	}
BigData::BigData(const char* _str)
{	//"+123456"
	assert(_str);
	char symbol = _str[0];
	char *pData = (char*)_str;
	
	if ('+' == symbol || '-' == symbol)        //"+12345"   "-12345"    "+a12b3"   "-a12b3"
	{
		pData++;
	}
	else if (*pData >= '0'&& *pData <= '9')     //"123456"
	{
		symbol = '+';
	}
	else                                     //"abc123"
	{
		m_llValue = 0;
		m_strData = "0";
		return;
	}
	//去掉前置0
	while ('0' == *pData)                     //"00000123"   
	{
		pData++;
	}
	//resize
	m_strData.resize(strlen(pData) + 1);
	m_llValue = 0;
	m_strData[0] = symbol;
	int iCount = 1;
	//"123aaa456"
	while (pData)
	{
		if (*pData >= '0'&& *pData <= '9')
		{
			m_llValue = m_llValue*10 + *pData - '0';
			m_strData[iCount++] = *pData++;
		}
		else
		{
			break;
		}
	}
	m_strData.resize(iCount);
	
	if ('-' == symbol)
	{
		m_llValue = 0 - m_llValue;
	}
	
}

BigData BigData:: operator+(const BigData& bd)
{
	//两操作数都未溢出
	if (!IsINT64OverFlow() && !bd.IsINT64OverFlow())
	{
		//异号
		if (m_strData[0] != bd.m_strData[0])
		{
			return BigData(m_llValue + bd.m_llValue);
		}
		//同号 相加结果未溢出
		if (('+' == m_strData[0] && MAX_INT64 - m_llValue >= bd.m_llValue) || ('-' == m_strData[0] && MIN_INT64 - m_llValue <= bd.m_llValue))
		{
			return BigData(m_llValue + bd.m_llValue);
		}
	}
	
	string retStr;
	if (m_strData[0] == bd.m_strData[0])
	{
		retStr = Add(m_strData, bd.m_strData);
	}
	return BigData(retStr.c_str());
}

BigData BigData:: operator-(const BigData& bd)
{
	//两操作数都未溢出
	if (!IsINT64OverFlow() && !bd.IsINT64OverFlow())
	{
		//同号
		if (m_strData[0] == bd.m_strData[0])
		{
			return BigData(m_llValue - bd.m_llValue);
		}
		//异号，相减结果未溢出
		//-10 + 8 = -2 < -1        10 + -2 = 8 > 7
		else if (('+' == m_strData[0] && INT_MAX + bd.m_llValue >= m_llValue) || ('-' == m_strData[0] && INT_MIN + bd.m_llValue <= m_llValue))
		{
			return BigData(m_llValue - bd.m_llValue);
		}
	}

	//至少一个溢出或结果溢出
	string retStr;
	if (m_strData[0] == bd.m_strData[0])
	{
		retStr = Sub(m_strData, bd.m_strData);
	}
	else
	{
		retStr = Add(m_strData, bd.m_strData);
	}
	return BigData(retStr.c_str());
}

BigData BigData:: operator*(const BigData& bd)
{
	//操作数中有0
	if (0 == m_llValue || 0 == bd.m_llValue)
	{
		return BigData(INT64(0));
	}
	//两操作数都未溢出
	if (!IsINT64OverFlow() && !bd.IsINT64OverFlow())
	{
		//结果未溢出
		if (m_strData[0] == bd.m_strData[0])
		{
			if (('+' == m_strData[0] && INT_MAX / m_llValue >= bd.m_llValue) || ('-' == m_strData[0] && INT_MAX / m_llValue <= bd.m_llValue))
			{
				return BigData(m_llValue*bd.m_llValue);
			}
		}
		else
		{
			if (('+' == m_strData[0] && INT_MIN / m_llValue <= bd.m_llValue) || ('-' == m_strData[0] && INT_MIN / m_llValue >= bd.m_llValue))
			{
				return BigData(m_llValue*bd.m_llValue);
			}
		}
	}
	return BigData(Mul(m_strData, bd.m_strData).c_str());
}

BigData BigData::operator/(const BigData&  bd)
{
	//判0
	assert(bd.m_llValue != 0);
	if (m_llValue == 0)
	{
		return BigData(INT64(0));
	}

	if (!IsINT64OverFlow() && !bd.IsINT64OverFlow())
	{
		return BigData(m_llValue / bd.m_llValue);
	}
	return BigData(Div(m_strData, bd.m_strData).c_str());
}

string BigData::Add(string left, string right)
{
	int iLSize = left.size();
	int iRSize = right.size();
	if (iLSize < iRSize)        //让左操作数位数大于右操作数位数
	{
		swap(left, right);
		swap(iLSize, iRSize);
	}

	char cStep = 0;
	string retStr;
	retStr.resize(iLSize + 1);
	for (int iIdx = 1; iIdx < iLSize; ++iIdx)
	{
		char cRet = left[iLSize - iIdx] - '0' + cStep;
		if (iIdx < iRSize)            
		{
			cRet += (right[iRSize - iIdx] - '0');
		}
		retStr[iLSize - iIdx + 1] = (cRet % 10) + '0';
		cStep = cRet / 10;
	}
	retStr[1] = cStep + '0';
	return retStr;
}

string BigData::Sub(string left, string right)
{
	//左操作数>右操作数
	//确定符号位
	int iLSize = left.size();
	int iRSize = right.size();
	char cSymbol = left[0];
	if (iLSize < iRSize || (iLSize == iRSize && left < right))
	{
		swap(left, right);
		swap(iLSize, iRSize);
		if ('+' == cSymbol)
		{
			cSymbol = '-';
		}
		else
		{
			cSymbol = '+';
		}
	}

	string retStr;
	retStr.resize(iLSize);
	retStr[0] = cSymbol;

	//逐位相减
	//1.取左操作数  2.取右操作数(未超出)  3.相减<0,借位：左前一位-=1,结果+10保存
	for (int iIdx = 1; iIdx < iLSize; ++iIdx)
	{
		char cRet = left[iLSize - iIdx] - '0';
		if (iIdx < iRSize)
		{
			cRet -= right[iRSize - iIdx] - '0';
		}
		if (cRet < 0)
		{
			left[iLSize - iIdx - 1] -= 1;
			cRet += 10;
		}
		retStr[iLSize - iIdx] = cRet + '0';
	}
	return retStr;
}

string BigData::Mul(string left, string right)
{
	//9999999   111
	int iLSize = left.size();
	int iRSize = right.size();
	if (iLSize > iRSize)
	{
		swap(left, right);
		swap(iLSize, iRSize);
	}

	char cSymbol = '+';
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}

	string retStr;
	retStr.assign(iLSize + iRSize - 1, '0');
	retStr[0] = cSymbol;
	int iDataLen = retStr.size();
	int iOffSet = 0;

	for (int iIdx = 1; iIdx < iLSize; ++iIdx)
	{
		char cLeft = left[iLSize - iIdx] - '0';
		char cStep = 0;
		if (0 == cLeft)    //遇0直接错位
		{
			iOffSet++;
			continue;
		}
		for (int iRIdx = 1; iRIdx < iRSize; ++iRIdx)
		{
			char cRet = cLeft*(right[iRSize - iRIdx] - '0');
			cRet += cStep;
			cRet += (retStr[iDataLen - iOffSet - iRIdx] - '0');
			retStr[iDataLen - iOffSet - iRIdx] = cRet % 10 + '0';
			cStep = cRet / 10;
		}
		retStr[iDataLen - iOffSet - iRSize] += cStep;
		iOffSet++;
	}
	return retStr;
}

string BigData::Div(string left, string right)
{
	char cSymbol = '+';
	int iLSize = left.size();
	int iRSize = right.size();
	
	//left < right
	if (iLSize < iRSize || (iLSize == iRSize && strcmp(left.c_str()+1, right.c_str()+1) < 0))
	{
		return "0";
	}
	
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}
	if ("+1" == right || "-1" == right)
	{
		left[0] = cSymbol;
		return left;
	}

	string retStr;
	char *pLeft = (char*)(left.c_str() + 1);
	char *pRight = (char*)(right.c_str() + 1);
	int Datalen = 1;
	iLSize -= 1;
	retStr.append(1, cSymbol);
	for (int iIdx = 0; iIdx < iLSize;)
	{
		//处理被除数中的‘0’
		if ('0' == *pLeft)
		{
			retStr.append(1, '0');
			pLeft++;
			++iIdx;
			continue;
		}
		if (!IsLeftStrBig(pLeft, iLSize, pRight, iRSize - 1))
		{
			Datalen++;
			if (iIdx + Datalen > iLSize)
			{
				break;
			}
			retStr.append(1, '0');
			continue;
		}
		else
		{
			retStr.append(1, subLoop(pLeft, Datalen, pRight, iRSize - 1));  //循环相减
		}
		//删除掉因为进行减法而出现的0
		while ('0' == *pLeft && Datalen > 0)
		{
			pLeft++;
			iIdx++;
			Datalen--;
		}
		
		Datalen++;
		if (iIdx + Datalen > iLSize)
		{
			break;
		}
	}
	return retStr;

}

bool BigData::IsLeftStrBig(char* pLeft, size_t iLSize, char *pRight, size_t iRSize)
{
	assert(NULL != pLeft && NULL != pRight);
	if ((iLSize > iRSize) || 
			(iLSize == iRSize && strncmp(pLeft, pRight, iRSize) >= 0))
	{
		return true;
	}
	return false;
}

//循环相减
int BigData::subLoop(char* pLeft, size_t iLSize, char *pRight, size_t iRSize)
{
	assert(pLeft != NULL);
	char cRet = '0';
	while (true)
	{
		if (!IsLeftStrBig(pLeft, iLSize, pRight, iRSize))
		{
			break;
		}

		int iLDataLen = iLSize - 1;
		int iRDataLen = iRSize - 1;
		while (iLDataLen >= 0 && iRDataLen >= 0)
		{
			if (pLeft[iLDataLen] < pRight[iRDataLen])
			{
				pLeft[iLDataLen - 1] -= 1;
				pLeft[iLDataLen] += 10;
			}
			pLeft[iLDataLen] = pLeft[iLDataLen] - pRight[iRDataLen] + '0';
			iLDataLen--;
			iRDataLen--;
		}

		//被除数中的0    "1230000000000000432"
		while ('0' == *pLeft && iLSize > 0)
		{
			pLeft++;
			iLSize--;
		}
		cRet++;
	}
	return cRet;
}

bool BigData:: IsINT64OverFlow()const
{
	string tmpStr = ('+' == m_strData[0] ? "+9223372036854775807" : "-9223372036854775808");
	if (m_strData.size() > tmpStr.size())
	{
		return true;
	}
	else if (m_strData.size() == tmpStr.size() && m_strData > tmpStr)
	{
		return true;
	}
	return false;

}

void BigData:: Int64ToStr()
{
	char cSymbol = '+';
	INT64 temp = m_llValue;        
	if (temp < 0)
	{
		cSymbol = '-';
		temp = 0 - temp;            //123456
	}
	m_strData.append(1, cSymbol);

	while (temp>0)                  //654321
	{
		m_strData.append(1, temp % 10 + '0');
		temp /= 10;
	}
	//逆置->123456
	char *pLeft = (char*)(m_strData.c_str() + 1);
	char *pRight = (char*)(m_strData.c_str() + m_strData.size() - 1);
	while (pLeft < pRight)
	{
		char tmp = *pLeft;
		*pLeft++ = *pRight;
		*pRight-- = tmp;
	}

}


ostream& operator<<(ostream& os, const BigData& bd)
{
	if (!bd.IsINT64OverFlow())
	{
		os << bd.m_llValue;
	}
	else
	{
		char* retStr = (char*)bd.m_strData.c_str();
		if ('+' == retStr[0])
		{
			retStr++;
		}
		os << retStr;
	}
	return os;
}




