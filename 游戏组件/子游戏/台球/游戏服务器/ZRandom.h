// ZRandom.h
// 随机数 头文件
#pragma once

#include <memory.h>

////////////////////////////////////////////////////////////////////////
//	ZRand

class ZRandom
{
private:
	static DWORD MCoef_32[2];
public:
	DWORD Random(DWORD dwMset,DWORD dwKey,int nNumber,DWORD *pdwRandom);
	void RandomGenerator(void *pBuffer, int nLength);
	DWORD Random(DWORD max);
};

////////////////////////////////////////////////////////////////////////
//	ZRandom_List

class ZRandom_List : public ZRandom
{
private:
	DWORD	*m_pBuff;
	DWORD	m_dwSize;
public:
	ZRandom_List();
	~ZRandom_List() { Release(); }

	bool IsInit() const { return m_pBuff != NULL; }
	DWORD GetSize() const { return m_dwSize; }
	DWORD *GetBuff() { return m_pBuff; }

	HRESULT Init(DWORD size);

	void Release();

	void Random(DWORD nums,DWORD max);
};
