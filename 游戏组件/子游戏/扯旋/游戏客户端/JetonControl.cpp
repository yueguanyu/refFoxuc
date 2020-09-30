#include "StdAfx.h"
#include "Resource.h"
#include "JetonControl.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//构造函数
CJettonControl::CJettonControl()
{
	//设置变量
	m_lCellJetton=0l;
	m_lUserJetton=0l;
	m_lUserHaveCount=0l;
	m_lUserAddJetton=0l;
	m_lMinJetton=0l;
	m_cbFirstIndex=0;
	m_cbMaxLayerCount=10;
	m_UserJettonlPost.SetPoint(0,0);	
	m_AddJettonPost.SetPoint(0,0);	;
	ZeroMemory(m_cbChangeStatus,sizeof(m_cbChangeStatus));
	ZeroMemory(m_AddLayerInfo,sizeof(m_AddLayerInfo));
	ZeroMemory(m_UserLayerInfo,sizeof(m_UserLayerInfo));
	m_lScoreIndex[0]=10000000;
	m_lScoreIndex[1]=1000000;
	m_lScoreIndex[2]=100000;
	m_lScoreIndex[3]=10000;
	m_lScoreIndex[4]=1000;
	m_lScoreIndex[5]=100;
	m_lScoreIndex[6]=10;
	m_lScoreIndex[7]=1;

	//加载资源
	m_ImageJettonBack.LoadFromResource(AfxGetInstanceHandle(),IDB_JETTON_BACK);
	m_ImageBigJetton.LoadFromResource(AfxGetInstanceHandle(),IDB_BIG_JETTON);
	m_ImageSmaJetton.LoadFromResource(AfxGetInstanceHandle(),IDB_SMALL_JETTON);

	//筹码大小
	//CImageHandle ImageBigJetton(&m_ImageBigJetton);
	//CImageHandle ImageSmaJetton(&m_ImageSmaJetton);
	//CImageHandle ImageJettonBack(&m_ImageJettonBack);
	m_BigJettonSize.SetSize(m_ImageBigJetton.GetWidth()/JETTON_COUNT,m_ImageBigJetton.GetHeight());	
	m_SmaJettonSize.SetSize(m_ImageSmaJetton.GetWidth()/JETTON_COUNT,m_ImageSmaJetton.GetHeight());	 
	m_JettonBackSize.SetSize(m_ImageJettonBack.GetWidth()/2,m_ImageJettonBack.GetHeight());	 

	m_pbtDecrease=NULL;
	m_pbtClearJetton=NULL;
	m_pbtAdd=NULL;

	return;
}

//析构函数
CJettonControl::~CJettonControl()
{
	m_pbtDecrease= NULL;
	m_pbtClearJetton=NULL;
	m_pbtAdd=NULL;
}

//设置现有
void CJettonControl::SetUserHaveCount(LONGLONG lHaveCound)
{
	m_lUserHaveCount = lHaveCound;

	return ;
}

//设置按钮
void CJettonControl::SetButton(CSkinButton *btDecrease,CSkinButton *pbtClearJetton,CSkinButton *pbtAdd)
{
	m_pbtDecrease=btDecrease;
	m_pbtClearJetton=pbtClearJetton;
	m_pbtAdd=pbtAdd;

	return ;
}

//更新控件
void CJettonControl::UpdataButton()
{
	//清理按钮
	BOOL bEnable=(m_lUserAddJetton>m_lMinJetton)?TRUE:FALSE;
	m_pbtClearJetton->EnableWindow(bEnable);

	//加注按钮
	m_pbtAdd->EnableWindow(bEnable);

	//减注按钮
	for(BYTE i=0;i<CONTROL_COUNT;i++)
	{
		//LONGLONG lScore = m_lCellJetton*(LONGLONG)pow(10,(CONTROL_COUNT-1-i));
		bEnable=(m_AddLayerInfo[i].cbLayerCount>0 && (m_lUserAddJetton>m_lMinJetton))?TRUE:FALSE;
		m_pbtDecrease[i].EnableWindow(bEnable);
	}

	return ;
}

//绘画位置
void CJettonControl::SetControlPost(CPoint ControlPost,CPoint AddPost)
{
	m_UserJettonlPost=ControlPost;	
	m_AddJettonPost=AddPost;

	return ;
}

//单元层数
void CJettonControl::SetJettonLayer(BYTE cbJettonLayer)
{
	m_cbMaxLayerCount = cbJettonLayer;

	return ;
}

//设置单元
void CJettonControl::SetCellJetton(LONGLONG lCellJetton)
{
	m_lCellJetton = lCellJetton;

	for (BYTE i=0;i<CountArray(m_lScoreIndex);i++)
	{
		if(m_lScoreIndex[i]==m_lCellJetton)break;
	}

	if(i<CONTROL_COUNT-1)i=CONTROL_COUNT-1;
	m_cbFirstIndex=i-(CONTROL_COUNT-1);

	return ;
}

//最小筹码
void CJettonControl::SetMinJetton(LONGLONG lMinJetton)
{
	m_lMinJetton = lMinJetton;
	m_lUserAddJetton = m_lMinJetton;
	ZeroMemory(m_AddLayerInfo,sizeof(m_AddLayerInfo));
	CountJettonInfo(m_lUserAddJetton,m_AddLayerInfo);

	//更新控件
	UpdataButton();

	return ;
}

//设置筹码
void CJettonControl::SetUserJetton(LONGLONG lUserJetton)
{
	m_lUserJetton = lUserJetton;
	ZeroMemory(m_UserLayerInfo,sizeof(m_UserLayerInfo));
	CountJettonInfo(lUserJetton,m_UserLayerInfo);

	//更新控件
	UpdataButton();

	return ;
}

//设置筹码
void CJettonControl::SetAddJetton(LONGLONG lAddJetton)
{
	m_lUserAddJetton = lAddJetton;	
	ZeroMemory(m_AddLayerInfo,sizeof(m_AddLayerInfo));
	CountJettonInfo(lAddJetton,m_AddLayerInfo);

	//更新控件
	UpdataButton();

	return ;
}

//计算筹码
void CJettonControl::CountJettonInfo(LONGLONG lScore,JettonLayerInfo LayerInfo[CONTROL_COUNT],BYTE cbChangeIndex)
{
	if(lScore<m_lCellJetton)return;

	//筹码数目
	LONGLONG lScoreCount=lScore;
	ZeroMemory(LayerInfo,sizeof(JettonLayerInfo)*CONTROL_COUNT);
	for (BYTE i=m_cbFirstIndex;i<m_cbFirstIndex+CONTROL_COUNT;i++)
	{
		//计算数目
		LONGLONG lCellCount=lScoreCount/m_lScoreIndex[i];

		//插入过虑
		if (lCellCount==0L) continue;

		//保存信息
		LayerInfo[i-m_cbFirstIndex].cbDrawIndex=JETTON_COUNT-i-1;
		LayerInfo[i-m_cbFirstIndex].cbLayerCount=(BYTE)lCellCount;

		//减少数目
		lScoreCount-=lCellCount*m_lScoreIndex[i];
	}

	//过滤绘画
	if(cbChangeIndex==CONTROL_COUNT)return;

	//整理数目
	for(LONGLONG i=0;i<CONTROL_COUNT-1;i++)
	{
		if(i==cbChangeIndex)break;
		if(LayerInfo[i].cbLayerCount>0 && LayerInfo[i+1].cbLayerCount==0)
		{
			LayerInfo[i].cbLayerCount-=1;
			BYTE cbIndex=JETTON_COUNT-1-LayerInfo[i].cbDrawIndex;
			LayerInfo[i+1].cbLayerCount=BYTE(m_lScoreIndex[cbIndex]/m_lScoreIndex[cbIndex+1]);
			LayerInfo[i+1].cbDrawIndex=JETTON_COUNT-(cbIndex+1)-1;
		}
	}

	return ;
}

//点击判断
bool CJettonControl::EstimateHitJetton(CPoint Point)
{
	if(m_lUserJetton+m_lUserAddJetton==0)return false;

	//判断加注
	for(INT i=CONTROL_COUNT-1;i>=0;i--)
	{
		//计算位置
		INT nIndex = CONTROL_COUNT-i-1;
		INT nXSrc = m_UserJettonlPost.x+(m_BigJettonSize.cx+JETTON_SPECE)*nIndex;
		INT nYSrc = m_UserJettonlPost.y+m_BigJettonSize.cy;
		INT nXDest =nXSrc+m_BigJettonSize.cx;
		INT nYDest =0;
		if(m_UserLayerInfo[i].cbLayerCount>0)
		{
			nYDest=m_UserJettonlPost.y-(m_UserLayerInfo[i].cbLayerCount-1)*LAYER_HEIGHT;
		}
		else nYDest=nYSrc-m_JettonBackSize.cy;

		//有效位置
		if(Point.x>=nXSrc && Point.x<=nXDest && Point.y<=nYSrc && Point.y>=nYDest)
		{
			//加注位置
			if(m_UserLayerInfo[i].cbLayerCount>0)
			{
				nIndex = JETTON_COUNT-1-m_UserLayerInfo[i].cbDrawIndex;

				//转换背景
				m_UserLayerInfo[i].cbLayerCount--;
				if(m_UserLayerInfo[i].cbLayerCount==0 && m_cbChangeStatus[i]==FALSE)
				{
					m_cbChangeStatus[i]=TRUE;
				}

				//整理筹码
				m_lUserJetton-=m_lScoreIndex[nIndex];
				m_lUserAddJetton+=m_lScoreIndex[nIndex];
				//if(m_AddLayerInfo[i].cbLayerCount==m_cbMaxLayerCount)
				{
					CountJettonInfo(m_lUserAddJetton,m_AddLayerInfo,CONTROL_COUNT);
				}
				//else 
				//{
				//	m_AddLayerInfo[i].cbLayerCount++;
				//	m_AddLayerInfo[i].cbDrawIndex=m_UserLayerInfo[i].cbDrawIndex;
				//}

				//更新控件
				UpdataButton();
				return true;
			}
			else
			{
				//指定转换
				for(LONGLONG j=i-1;j>=0;j--)
				{
					if(m_UserLayerInfo[j].cbLayerCount==0)continue;
					CountJettonInfo(m_lUserJetton,m_UserLayerInfo,(BYTE)i);

					//更新控件
					UpdataButton();
					return true;
				}

				//换大筹码
				LONGLONG lScore = m_lCellJetton*(LONGLONG)pow(10,(CONTROL_COUNT-1-i));
				if(m_lUserJetton>=lScore)
				{
					CountJettonInfo(m_lUserJetton,m_UserLayerInfo,CONTROL_COUNT);

					//更新控件
					UpdataButton();
					return true;
				}

			}
		}
	}

	//判断减注
	for(INT i=CONTROL_COUNT-1;i>=0;i--)
	{
		//计算位置
		INT nIndex = CONTROL_COUNT-i-1;
		INT nXSrc = m_AddJettonPost.x+(m_SmaJettonSize.cx+JETTON_SPECE)*nIndex;
		INT nYSrc = m_AddJettonPost.y+m_SmaJettonSize.cy;
		INT nXDest =nXSrc+m_SmaJettonSize.cx;
		INT nYDest =m_AddJettonPost.y-(m_AddLayerInfo[i].cbLayerCount-1)*LAYER_HEIGHT;

		//有效位置
		if(Point.x>=nXSrc && Point.x<=nXDest && Point.y<=nYSrc && Point.y>=nYDest)
		{
			return DecreaseJetton(WORD(i));
		}
	}

	return false;
}

//移动判断
bool CJettonControl::EstimateMove(CPoint Point)
{
	if(m_lUserJetton+m_lUserAddJetton==0)return false;

	//判断位置
	for(INT i=CONTROL_COUNT-1;i>=0;i--)
	{
		//计算位置
		INT nIndex = CONTROL_COUNT-i-1;
		INT nXSrc = m_UserJettonlPost.x+(m_BigJettonSize.cx+JETTON_SPECE)*nIndex;
		INT nYSrc = m_UserJettonlPost.y+m_BigJettonSize.cy;
		INT nXDest =nXSrc+m_BigJettonSize.cx;
		INT nYDest =m_UserJettonlPost.y-(m_UserLayerInfo[i].cbLayerCount-1)*LAYER_HEIGHT;

		//有效位置
		if(Point.x>=nXSrc && Point.x<=nXDest && Point.y<=nYSrc && Point.y>=nYDest)
		{
			if(m_UserLayerInfo[i].cbLayerCount==0 && m_cbChangeStatus[i]==FALSE)
			{
				m_cbChangeStatus[i]=TRUE;
				return true;
			}
		}
		else if(m_cbChangeStatus[i]==TRUE)
		{
			m_cbChangeStatus[i]=FALSE;
			return true;
		}
	}

	return false;
}

//减少筹码
bool CJettonControl::DecreaseJetton(WORD wControlID)
{
	ASSERT(wControlID<CONTROL_COUNT);
	if(wControlID>=CONTROL_COUNT)return false;

	if(m_AddLayerInfo[wControlID].cbLayerCount>0)
	{
		INT nIndex = JETTON_COUNT-1-m_AddLayerInfo[wControlID].cbDrawIndex;

		//减少筹码
		if(m_lUserAddJetton-m_lScoreIndex[nIndex]>=m_lMinJetton)
		{
			m_AddLayerInfo[wControlID].cbLayerCount--;

			m_lUserJetton+=m_lScoreIndex[nIndex];
			m_lUserAddJetton-=m_lScoreIndex[nIndex];
			if(m_UserLayerInfo[wControlID].cbLayerCount==m_cbMaxLayerCount)
			{
				CountJettonInfo(m_lUserJetton,m_UserLayerInfo,CONTROL_COUNT);
			}
			else 
			{
				m_UserLayerInfo[wControlID].cbLayerCount++;
				m_UserLayerInfo[wControlID].cbDrawIndex=m_AddLayerInfo[wControlID].cbDrawIndex;
			}
		}
		//最少筹码
		else
		{
			LONGLONG lTemp=m_lUserAddJetton-m_lMinJetton;
			m_lUserJetton+=lTemp;
			m_lUserAddJetton=m_lMinJetton;
			ZeroMemory(m_AddLayerInfo,sizeof(m_AddLayerInfo));
			ZeroMemory(m_UserLayerInfo,sizeof(m_UserLayerInfo));
			CountJettonInfo(m_lUserJetton,m_UserLayerInfo);
			CountJettonInfo(m_lUserAddJetton,m_AddLayerInfo);
		}

		//更新控件
		UpdataButton();

		return true;
	}

	return false;
}

//清理加注
void CJettonControl::ClearAddJetton()
{
	//清理加注
	m_lUserJetton = m_lUserJetton+m_lUserAddJetton-m_lMinJetton;
	m_lUserAddJetton = m_lMinJetton;
	ZeroMemory(m_AddLayerInfo,sizeof(m_AddLayerInfo));
	ZeroMemory(m_UserLayerInfo,sizeof(m_UserLayerInfo));
	CountJettonInfo(m_lUserJetton,m_UserLayerInfo);
	CountJettonInfo(m_lUserAddJetton,m_AddLayerInfo);

	//更新控件
	UpdataButton();

	return;
}

//艺术字体
void CJettonControl::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
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

//绘画筹码
void CJettonControl::DrawJettonControl(CDC * pDC)
{
	//绘画判断
	if (m_lUserJetton==0L && m_lUserAddJetton==0L) return;

	//加载资源
	//CImageHandle ImageBigJetton(&m_ImageBigJetton);
	//CImageHandle ImageSmaJetton(&m_ImageSmaJetton);
	//CImageHandle ImageJettonBack(&m_ImageJettonBack);

	//筹码背景
	for(BYTE i=0;i<CONTROL_COUNT;i++)
	{
		if(m_UserLayerInfo[i].cbLayerCount>0)continue;

		LONGLONG lScore = m_lCellJetton*(LONGLONG)pow(10,(CONTROL_COUNT-1-i));
		if(m_lUserJetton>=lScore)
		{
			//绘画位置
			INT nIndex = CONTROL_COUNT-i-1;
			INT nXPost=m_UserJettonlPost.x+(m_JettonBackSize.cx+1)*nIndex;
			INT nYPost=m_UserJettonlPost.y-m_JettonBackSize.cy/2+6;

			//绘画背景
			m_ImageJettonBack.TransDrawImage(pDC,nXPost,nYPost,m_JettonBackSize.cx,
				m_JettonBackSize.cy,m_JettonBackSize.cx*m_cbChangeStatus[i],0,RGB(255,0,255));
		}
	}

	//绘画变量
	for(BYTE k=0;k<2;k++)
	{
		//筹码信息
		LONGLONG lScore=(k==0)?(m_lUserJetton):(m_lUserAddJetton);
		CSize *pJettonSize =(k==0)?(&m_BigJettonSize):(&m_SmaJettonSize);
		CPoint *pDrawPoint =(k==0)?(&m_UserJettonlPost):(&m_AddJettonPost);
		CBitImage *pSkinImage =(k==0)?(&m_ImageBigJetton):(&m_ImageSmaJetton);
		JettonLayerInfo LayerInfo[CONTROL_COUNT];
		if(k==0)CopyMemory(LayerInfo,m_UserLayerInfo,sizeof(LayerInfo));
		else CopyMemory(LayerInfo,m_AddLayerInfo,sizeof(LayerInfo));

		//绘画筹码
		for(INT i=CONTROL_COUNT-1;i>=0;i--)
		{
			if (LayerInfo[i].cbLayerCount==0L) continue;

			//首绘位置
			BYTE cbScoreIndex=LayerInfo[i].cbDrawIndex;
			INT nIndex = CONTROL_COUNT-i-1;
			INT nXPost=pDrawPoint->x+(pJettonSize->cx+JETTON_SPECE)*nIndex;
			INT nYPost=pDrawPoint->y;

			//绘画筹码
			LONGLONG lLayerCount=__min(LayerInfo[i].cbLayerCount,m_cbMaxLayerCount);
			for (INT j=0;j<lLayerCount;j++)
			{
				pSkinImage->TransDrawImage(pDC,nXPost,nYPost-LAYER_HEIGHT*j,pJettonSize->cx,
					pJettonSize->cy,cbScoreIndex*pJettonSize->cx,0,RGB(255,0,255));
			}
		}

		//if(k==1 && lScore-m_lMinJetton<=0)continue;

		//创建字体
		CFont ViewFont;
		ViewFont.CreateFont(-13,0,0,0,400,0,0,0,134,3,2,1,1,TEXT("宋体"));
		CFont *pOldFont=pDC->SelectObject(&ViewFont);
		pDC->SetTextAlign(TA_LEFT);
		int iBkMode = pDC->SetBkMode( TRANSPARENT );

		//绘画数目
		TCHAR szBuffer[64]=TEXT("");
		if(k==0)
		{
			INT nXPost=pDrawPoint->x+155;
			INT nYPost =pDrawPoint->y+5;
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("余簸:%I64d"),lScore);
			DrawTextString(pDC,szBuffer,RGB(255,255,0),RGB(0,0,0),nXPost,nYPost);
		}
		else
		{
			INT nXPost=pDrawPoint->x+5;
			INT nYPost=pDrawPoint->y+35;
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("最少簸:%I64d,加簸:%I64d"),m_lMinJetton,lScore-m_lMinJetton);
			DrawTextString(pDC,szBuffer,RGB(255,255,0),RGB(0,0,0),nXPost,nYPost);
		}

		//释放资源
		pDC->SetBkMode(iBkMode);
		pDC->SelectObject(pOldFont);
		ViewFont.DeleteObject();	

		//绘画筹码
		for(INT i=CONTROL_COUNT-1;i>=0;i--)
		{
			//绘画层数
			if(LayerInfo[i].cbLayerCount>m_cbMaxLayerCount)
			{
				//创建字体
				CFont ViewFont;
				ViewFont.CreateFont(-13,0,0,0,400,0,0,0,134,3,2,1,1,TEXT("宋体"));
				CFont *pOldFont=pDC->SelectObject(&ViewFont);
				pDC->SetTextAlign(TA_LEFT);

				//绘画数目
				TCHAR szBuffer[64]=TEXT("");
				INT nIndex = CONTROL_COUNT-i-1;
				INT nXPost=pDrawPoint->x+(pJettonSize->cx+JETTON_SPECE)*nIndex;
				INT nYPost=pDrawPoint->y;
				_sntprintf(szBuffer,sizeof(szBuffer),TEXT("＊%d"),LayerInfo[i].cbLayerCount);
				DrawTextString(pDC,szBuffer,RGB(255,255,0),RGB(0,0,0),nXPost,nYPost);

				//释放资源
				pDC->SelectObject(pOldFont);
				ViewFont.DeleteObject();	
			}
		}
	}

	//现有注数
	if(m_lUserHaveCount>0)
	{
		//创建字体
		CFont ViewFont;
		ViewFont.CreateFont(-13,0,0,0,400,0,0,0,134,3,2,1,1,TEXT("宋体"));
		CFont *pOldFont=pDC->SelectObject(&ViewFont);
		pDC->SetTextAlign(TA_LEFT);
		int iBkMode = pDC->SetBkMode( TRANSPARENT );

		//绘画数目
		TCHAR szBuffer[64]=TEXT("");
		INT nXPost=m_UserJettonlPost.x+155;
		INT nYPost =m_UserJettonlPost.y-10;
		_sntprintf(szBuffer,sizeof(szBuffer),TEXT("现簸:%I64d"),m_lUserHaveCount);
		DrawTextString(pDC,szBuffer,RGB(255,255,0),RGB(0,0,0),nXPost,nYPost);


		//释放资源
		pDC->SetBkMode(iBkMode);
		pDC->SelectObject(pOldFont);
		ViewFont.DeleteObject();	
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
