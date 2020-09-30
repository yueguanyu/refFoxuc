#include "StdAfx.h"
#include "Resource.h"
#include "GoldView.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
bool							CGoldView::m_bInit=false;				//初始标志
CBitImage						CGoldView::m_ImageGold;					//正常筹码

//////////////////////////////////////////////////////////////////////////

//构造函数
CGoldView::CGoldView()
{
	m_lGold=0L;
	m_lMaxLayer=10;
	m_lCellScore=10L;
	m_cbFirstIndex=3;
	m_bBeelinePut=false;
	m_lScoreIndex[0]=10000000;
	m_lScoreIndex[1]=1000000;
	m_lScoreIndex[2]=100000;
	m_lScoreIndex[3]=10000;
	m_lScoreIndex[4]=1000;
	m_lScoreIndex[5]=100;
	m_lScoreIndex[6]=10;
	m_lScoreIndex[7]=1;
	memset(m_lDrawOrder,0,sizeof(m_lDrawOrder));
	memset(m_ptDrawJetton,0,sizeof(m_ptDrawJetton));
	memset(m_JettonHeapInfo,0,sizeof(m_JettonHeapInfo));
	if (m_bInit==false)
	{
		m_bInit=true;
		m_ImageGold.LoadFromResource(GetModuleHandle(NULL),IDB_SMALL_JETTON);
	}
	HINSTANCE hInstance=AfxGetInstanceHandle();

	//筹码大小
	//CImageHandle ImageHandle(&m_ImageGold);
	m_JettonSize.SetSize(m_ImageGold.GetWidth()/CountArray(m_lScoreIndex),m_ImageGold.GetHeight());

	return;
}

//析构函数
CGoldView::~CGoldView()
{
}

//设置摆放
void CGoldView::SetBeelinePut(bool bBeelinePut)
{
	if (bBeelinePut!=m_bBeelinePut)
	{
		m_bBeelinePut=bBeelinePut;
		RectifyGoldLayer();
	}
	return ;
}

//设置筹码
void CGoldView::SetGold(LONGLONG lGold)
{
	if (m_lGold!=lGold)
	{
		m_lGold=lGold;
		RectifyGoldLayer();
	}
	return;
}

//设置单元
void CGoldView::SetCellJetton(LONGLONG lCellJetton)
{
	m_lCellScore = lCellJetton;

	for (BYTE i=0;i<CountArray(m_lScoreIndex);i++)
	{
		if(m_lScoreIndex[i]==m_lCellScore)break;
	}

	if(i<CONTROL_COUNT-1)i=CONTROL_COUNT-1;
	m_cbFirstIndex=i-(CONTROL_COUNT-1);

	return ;
}

//设置层数
void CGoldView::SetMaxGoldLayer(LONGLONG lMaxLayer)
{
	if (m_lMaxLayer!=lMaxLayer)
	{
		m_lMaxLayer=lMaxLayer; 
	}
	return;
}

//绘画筹码
void CGoldView::DrawGoldView(CDC * pDC, int nXPos, int nYPos, bool bCount,bool bCenter)
{
	if(GetGold()==0)return;

	//加载位图
	//CImageHandle ImageHandle(&m_ImageGold);

	//位置信息
	int nYPosDraw=nYPos;
	int nXPosDraw=nXPos;

	//绘画筹码
	INT nCount=0;
	for (INT i=0;i<CountArray(m_JettonHeapInfo);i++)
	{
		LONGLONG lIndex = (!m_bBeelinePut)?m_lDrawOrder[i]:i;
		LONGLONG lLayerCount = __min(m_JettonHeapInfo[lIndex].cbLayerCount,m_lMaxLayer);
		for (INT j=0;j<lLayerCount;j++)
		{
			//绘画位置
			if(j==0)
			{
				nYPosDraw=nYPos+m_ptDrawJetton[nCount].y;
				nXPosDraw=nXPos+m_ptDrawJetton[nCount++].x;
			}

			//绘画筹码
			INT nIndex = m_JettonHeapInfo[lIndex].cbDrawIndex;
			m_ImageGold.TransDrawImage(pDC,nXPosDraw,nYPosDraw-JETTON_SPECE*j,
				m_JettonSize.cx,m_JettonSize.cy,nIndex*m_JettonSize.cx,0,RGB(255,0,255));
		}
		if(!bCenter && nCount==CONTROL_COUNT/*+1*/)break;
	}

	//绘画数字
	if((m_lGold>=1L)&&(bCount))
	{
		//创建字体
		CFont ViewFont;
		INT nFontSize = (bCenter)?(-15):(-13);
		ViewFont.CreateFont(nFontSize,0,0,0,400,0,0,0,134,3,2,1,1,TEXT("宋体"));
		CFont *pOldFont=pDC->SelectObject(&ViewFont);
		pDC->SetTextAlign(TA_CENTER);
       int iBkMode = pDC->SetBkMode( TRANSPARENT );

	   //输出位置
	   if(!m_bBeelinePut)
	   {
		   if(nCount<3)nYPos+=32;
		   else nYPos+=45;
	   }
	   else nYPos+=30;

	   //输出信息
	   TCHAR szBuffer[64]=TEXT(""),szCountBuffer[64]=TEXT("");
	   GetGlodString(m_lGold,szCountBuffer);
	   if(bCenter)_sntprintf(szBuffer,sizeof(szBuffer),TEXT("总簸:%s"),szCountBuffer);
	   else _sntprintf(szBuffer,sizeof(szBuffer),TEXT("%s"),szCountBuffer);
	   DrawTextString(pDC,szBuffer,RGB(255,255,0),RGB(0,0,0),nXPos+16,nYPos);

		//释放资源
	   pDC->SetBkMode(iBkMode);
	   pDC->SelectObject(pOldFont);
	   ViewFont.DeleteObject();	
	}

	return;
}

//调整筹码层
void CGoldView::RectifyGoldLayer()
{
	if(GetGold()==0)
	{
		memset(m_JettonHeapInfo,0,sizeof(m_JettonHeapInfo));
		return;
	}

	//变量定义
	LONGLONG lGold=m_lGold;
	memset(m_JettonHeapInfo,0,sizeof(m_JettonHeapInfo));

	//调整筹码层
	BYTE cbDrawCount=0;
	struct tagOrderInfo
	{
		BYTE cbLayerCount;
		LONGLONG lIndex;
	}OrderInfo[JETTON_COUNT];
	ZeroMemory(OrderInfo,sizeof(OrderInfo));
	for (BYTE i=0;i<JETTON_COUNT;i++)
	{
		OrderInfo[i].lIndex=i;
		if (lGold>=m_lScoreIndex[i])
		{
			cbDrawCount++;
			m_JettonHeapInfo[i].cbLayerCount=BYTE(lGold/m_lScoreIndex[i]);
			m_JettonHeapInfo[i].cbDrawIndex=JETTON_COUNT-i-1;
			lGold-=m_JettonHeapInfo[i].cbLayerCount*m_lScoreIndex[i];
			OrderInfo[i].cbLayerCount = m_JettonHeapInfo[i].cbLayerCount;
		}
	}

	//直线摆放
	if(m_bBeelinePut)
	{
		//计算位置
		switch(cbDrawCount)
		{
		case 1:
			{
				m_ptDrawJetton[0].x=0;
				m_ptDrawJetton[0].y=0;
				return;
			}
		case 2:
			{
				m_ptDrawJetton[0].x=-m_JettonSize.cx/2-1;
				m_ptDrawJetton[0].y=0;
				m_ptDrawJetton[1].x=m_JettonSize.cx/2+1;
				m_ptDrawJetton[1].y=0;
				return;
			}
		case 3:
			{
				m_ptDrawJetton[0].x=0;
				m_ptDrawJetton[0].y=0;
				m_ptDrawJetton[1].x=-m_JettonSize.cx-1;
				m_ptDrawJetton[1].y=0;
				m_ptDrawJetton[2].x=m_JettonSize.cx+1;
				m_ptDrawJetton[2].y=0;
				return;
			}
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			{
				m_ptDrawJetton[0].x=-m_JettonSize.cx/2-1;
				m_ptDrawJetton[0].y=0;
				m_ptDrawJetton[1].x=m_JettonSize.cx/2+1;
				m_ptDrawJetton[1].y=0;
				m_ptDrawJetton[2].x=-m_JettonSize.cx*3/2-2;
				m_ptDrawJetton[2].y=0;
				m_ptDrawJetton[3].x=m_JettonSize.cx*3/2+3;
				m_ptDrawJetton[3].y=0;
				m_ptDrawJetton[4].x=m_JettonSize.cx*5/2+4;
				m_ptDrawJetton[4].y=0;
				return;
			}
		}
		return;
	}

	//从多到少
	for (LONGLONG i=0;i<JETTON_COUNT;i++)
	{
		for (LONGLONG j=i+1;j<JETTON_COUNT;j++)
		{
			if (OrderInfo[i].cbLayerCount<OrderInfo[j].cbLayerCount)
			{
				LONGLONG lTemp = OrderInfo[i].cbLayerCount;
				OrderInfo[i].cbLayerCount = OrderInfo[j].cbLayerCount;
				OrderInfo[j].cbLayerCount = (BYTE)lTemp;

				lTemp = OrderInfo[i].lIndex;
				OrderInfo[i].lIndex = OrderInfo[j].lIndex;
				OrderInfo[j].lIndex = lTemp;
			}
		}
	}

	//绘画顺序
	for (LONGLONG i=0;i<JETTON_COUNT;i++)m_lDrawOrder[i]=OrderInfo[i].lIndex;

	//计算位置
	switch(cbDrawCount)
	{
	case 1:
		{
			m_ptDrawJetton[0].x=0;
			m_ptDrawJetton[0].y=0;
			return;
		}
	case 2:
		{
			m_ptDrawJetton[0].x=-m_JettonSize.cx/2-2;
			m_ptDrawJetton[0].y=0;
			m_ptDrawJetton[1].x=m_JettonSize.cx/2+2;
			m_ptDrawJetton[1].y=0;
			return;
		}
	case 3:
		{
			m_ptDrawJetton[0].x=-m_JettonSize.cx/2-1;
			m_ptDrawJetton[0].y=-m_JettonSize.cy/2;
			m_ptDrawJetton[1].x=m_JettonSize.cx/2+1;
			m_ptDrawJetton[1].y=-m_JettonSize.cy/2;
			m_ptDrawJetton[2].x=0;
			m_ptDrawJetton[2].y=m_JettonSize.cy/2+1;
			return;
		}
	case 4:
		{
			m_ptDrawJetton[0].x=-m_JettonSize.cx/2-1;
			m_ptDrawJetton[0].y=-m_JettonSize.cy/2-1;
			m_ptDrawJetton[1].x=m_JettonSize.cx/2+1;
			m_ptDrawJetton[1].y=-m_JettonSize.cy/2-1;
			m_ptDrawJetton[2].x=-m_JettonSize.cx/2-1;
			m_ptDrawJetton[2].y=m_JettonSize.cy/2+1;
			m_ptDrawJetton[3].x=m_JettonSize.cx/2+1;
			m_ptDrawJetton[3].y=m_JettonSize.cy/2+1;
			return;
		}
	case 5:
	case 6:
		{
			m_ptDrawJetton[0].x=-m_JettonSize.cx/2-1;
			m_ptDrawJetton[0].y=-m_JettonSize.cy/2-1;
			m_ptDrawJetton[1].x=m_JettonSize.cx/2+1;
			m_ptDrawJetton[1].y=-m_JettonSize.cy/2-1;
			m_ptDrawJetton[2].x=-m_JettonSize.cx/2-1;
			m_ptDrawJetton[2].y=m_JettonSize.cy/2+1;
			m_ptDrawJetton[3].x=m_JettonSize.cx/2+1;
			m_ptDrawJetton[3].y=m_JettonSize.cy/2+1;
			m_ptDrawJetton[4].x=-m_JettonSize.cx*3/2-2;
			m_ptDrawJetton[4].y=0;
			m_ptDrawJetton[5].x=m_JettonSize.cx*3/2+2;
			m_ptDrawJetton[5].y=0;
			return;
		}	
	case 7:
	case 8:
		{
			m_ptDrawJetton[0].x=-m_JettonSize.cx/2-1;
			m_ptDrawJetton[0].y=-m_JettonSize.cy/2-1;
			m_ptDrawJetton[4].x=-m_JettonSize.cx*3/2-2;
			m_ptDrawJetton[4].y=-m_JettonSize.cy/2-1;
			m_ptDrawJetton[1].x=m_JettonSize.cx/2+1;
			m_ptDrawJetton[1].y=-m_JettonSize.cy/2-1;
			m_ptDrawJetton[5].x=m_JettonSize.cx*3/2+2;
			m_ptDrawJetton[5].y=-m_JettonSize.cy/2-1;
			m_ptDrawJetton[2].x=-m_JettonSize.cx/2-1;
			m_ptDrawJetton[2].y=m_JettonSize.cy/2+1;
			m_ptDrawJetton[6].x=-m_JettonSize.cx*3/2-2;
			m_ptDrawJetton[6].y=m_JettonSize.cy/2+1;
			m_ptDrawJetton[3].x=m_JettonSize.cx/2+1;
			m_ptDrawJetton[3].y=m_JettonSize.cy/2+1;
			m_ptDrawJetton[7].x=+m_JettonSize.cx*3/2+2;
			m_ptDrawJetton[7].y=m_JettonSize.cy/2+1;
			return;
		}	
	}

	return;
}

//整性变字符
LPCTSTR CGoldView::GetGlodString(LONGLONG lGold, TCHAR szString[])
{
	LONGLONG lTemp=(LONGLONG)lGold; 
	if(lGold<0L) return szString;

	//处理小数点
	DWORD dwCharBit=0L;

	//转换字符
	lTemp = (LONGLONG)lGold; 
	DWORD dwNumBit=0L;
	do
	{
		dwNumBit++;
		szString[dwCharBit++]=(TCHAR)(lTemp%10+TEXT('0'));
		if (dwNumBit%3==0) szString[dwCharBit++]=TEXT(',');
		lTemp/=10;
	} while (lTemp>0L);

	//调整字符
	if (szString[dwCharBit-1]==TEXT(',')) szString[dwCharBit-1]=0;
	szString[dwCharBit]=0;

	//尾头交换
	_tcsrev(szString);

	return szString;
}

//艺术字体
void CGoldView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}

//////////////////////////////////////////////////////////////////////////
