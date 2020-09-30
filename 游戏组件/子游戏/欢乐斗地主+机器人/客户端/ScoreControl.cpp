#include "StdAfx.h"
#include "GameClient.h"
#include "ScoreControl.h"

//////////////////////////////////////////////////////////////////////////////////

//控件变量
#define IDC_BT_CLOSE				100									//进贡扑克

//动画数目
#define SCORE_CARTOON_COUNT			4									//动画数目

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CScoreControl::CScoreControl()
{
	//设置变量
	m_cbAlphaIndex=0;
	m_cbCartoonIndex=0;

	//设置变量
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	return;
}

//析构函数
CScoreControl::~CScoreControl()
{
}

//动画消息
VOID CScoreControl::OnWindowMovie()
{
	//成绩动画
	if ((IsWindowVisible()==true)&&(m_CartoonLapseCount.GetLapseCount(400)>0L))
	{
		m_cbCartoonIndex=(m_cbCartoonIndex+1)%SCORE_CARTOON_COUNT;
	}

	//灰度动画
	if ((IsWindowVisible()==true)&&((m_cbAlphaIndex<110L)&&(m_AlphaLapseCount.GetLapseCount(20)>0L)))
	{
		m_cbAlphaIndex+=5L;
	}

	return;
}

//创建消息
VOID CScoreControl::OnWindowCreate(CD3DDevice * pD3DDevice)
{
	//创建字体
	m_FontScore.CreateFont(100,TEXT("宋体"),0L);

	//关闭按钮
	CRect rcCreate(0,0,0,0);
	m_btClose.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_CLOSE,GetVirtualEngine(),this);
	m_btClose.SetButtonImage(pD3DDevice,TEXT("BT_CLOSE"),TEXT("PNG"),AfxGetInstanceHandle());

	//加载资源
	m_TextureGameScore.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("GAME_SCORE"),TEXT("PNG"));
	m_TextureBombNumber.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("BOMB_NUMBER"),TEXT("PNG"));
	m_TextureWinLoseFlag.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("WIN_LOSE_FLAG"),TEXT("PNG"));
	m_TextureScoreCartoon.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("SCORE_CARTOON"),TEXT("PNG"));
	m_TextureChunTian.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("CHUN_TIAN"),TEXT("PNG"));

	//获取大小
	CSize SizeGameScore;
	SizeGameScore.SetSize(m_TextureGameScore.GetWidth(),m_TextureGameScore.GetHeight());

	//移动窗口
	SetWindowPos(0,0,SizeGameScore.cx,SizeGameScore.cy,0);

	//按钮位置
	CRect rcButton;
	m_btClose.GetWindowRect(rcButton);
	m_btClose.SetWindowPos(SizeGameScore.cx-rcButton.Width()-12,10,0,0,SWP_NOSIZE);
	
	return;
}

//销毁消息
VOID CScoreControl::OnWindowDestory(CD3DDevice * pD3DDevice)
{
	//删除字体
	m_FontScore.DeleteFont();

	//销户资源
	if (m_TextureGameScore.IsNull()==false) m_TextureGameScore.Destory();
	if (m_TextureBombNumber.IsNull()==false) m_TextureBombNumber.Destory();
	if (m_TextureWinLoseFlag.IsNull()==false) m_TextureWinLoseFlag.Destory();
	if (m_TextureScoreCartoon.IsNull()==false) m_TextureScoreCartoon.Destory();
	if (m_TextureChunTian.IsNull()==false) m_TextureChunTian.Destory();
	

	return;
}

//鼠标事件
VOID CScoreControl::OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos)
{
	return;
}

//按钮事件
VOID CScoreControl::OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos)
{
	//关闭窗口
	if (uButtonID==IDC_BT_CLOSE)
	{
		CloseControl();
		return;
	}

	return;
}

//绘画窗口
VOID CScoreControl::OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos)
{
	//灰化背景
	//CSize SizeWindow=pD3DDevice->GetSizeWindow();
	//pD3DDevice->FillRect(0,0,SizeWindow.cx,SizeWindow.cy,D3DCOLOR_ARGB(m_cbAlphaIndex,0,0,0));

	//绘画背景
	m_TextureGameScore.DrawImage(pD3DDevice,nXOriginPos,nYOriginPos);

	//获取大小
	CSize SizeBombNumber;
	CSize SizeWinLoseFlag;
	CSize SizeScoreCartoon;
	CSize SizeChunTian;
	SizeBombNumber.SetSize(m_TextureBombNumber.GetWidth()/11L,m_TextureBombNumber.GetHeight());
	SizeWinLoseFlag.SetSize(m_TextureWinLoseFlag.GetWidth()/2L,m_TextureWinLoseFlag.GetHeight());
	SizeScoreCartoon.SetSize(m_TextureScoreCartoon.GetWidth()/SCORE_CARTOON_COUNT,m_TextureScoreCartoon.GetHeight()/2L);
	SizeChunTian.SetSize(m_TextureChunTian.GetWidth()/2,m_TextureChunTian.GetHeight());

	//绘画动画
	if ( m_ScoreInfo.wBankerUser < GAME_PLAYER  )
	{
		m_TextureScoreCartoon.DrawImage(pD3DDevice,nXOriginPos+300,nYOriginPos+185,SizeScoreCartoon.cx,SizeScoreCartoon.cy,
			m_cbCartoonIndex*SizeScoreCartoon.cx,m_ScoreInfo.lGameScore[m_ScoreInfo.wBankerUser] > 0 ? 0 : SizeScoreCartoon.cy);
	}


	//绘画信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		TCHAR szUserScore[16]=TEXT("");
		TCHAR szCollectScore[16]=TEXT("");
		_sntprintf(szUserScore,CountArray(szUserScore),SCORE_STRING,m_ScoreInfo.lGameScore[i]);
		_sntprintf(szCollectScore,CountArray(szCollectScore),SCORE_STRING,m_ScoreInfo.lCollectScore[i]);

		//位置计算
		CRect rcNiceName(nXOriginPos+32,nYOriginPos+102+i*81,nXOriginPos+104,nYOriginPos+115+i*81);
		CRect rcGameScore(nXOriginPos+157,nYOriginPos+64+i*81,nXOriginPos+226,nYOriginPos+76+i*81);
		CRect rcCollectScore(nXOriginPos+157,nYOriginPos+85+i*81,nXOriginPos+226,nYOriginPos+97+i*81);

		//用户帐号
		LPCTSTR pszNiceName=m_ScoreInfo.szNickName[i];
		D3DCOLOR crColor=(m_ScoreInfo.wMeChairID==i)?D3DCOLOR_XRGB(153,255,0):D3DCOLOR_XRGB(255,255,255);
		m_FontScore.DrawText(pD3DDevice,pszNiceName,rcNiceName,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS,crColor);

		//绘画头像
		CGameFrameAvatar * pGameFrameAvatar=CGameFrameAvatar::GetInstance();
		pGameFrameAvatar->DrawUserAvatar(pD3DDevice,nXOriginPos+43,nYOriginPos+49+i*81,m_ScoreInfo.wFaceID[i]);

		//绘画信息
		m_FontScore.DrawText(pD3DDevice,szUserScore,rcGameScore,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS,D3DCOLOR_XRGB(255,255,255));
		m_FontScore.DrawText(pD3DDevice,szCollectScore,rcCollectScore,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS,D3DCOLOR_XRGB(255,255,255));

		//炸弹信息
		INT nBombExcursion=SizeBombNumber.cx*(m_ScoreInfo.cbEachBombCount[i]+1);
		m_TextureBombNumber.DrawImage(pD3DDevice,nXOriginPos+302,nYOriginPos+73+i*81,SizeBombNumber.cx,SizeBombNumber.cy,0,0);
		m_TextureBombNumber.DrawImage(pD3DDevice,nXOriginPos+317,nYOriginPos+73+i*81,SizeBombNumber.cx,SizeBombNumber.cy,nBombExcursion,0);

		//输赢标志
		INT nWinLostExcursion=(m_ScoreInfo.lGameScore[i]>0L)?0:SizeWinLoseFlag.cx;
		m_TextureWinLoseFlag.DrawImage(pD3DDevice,nXOriginPos+76,nYOriginPos+40+i*81,SizeWinLoseFlag.cx,SizeWinLoseFlag.cy,nWinLostExcursion,0);
	
		//底分
		DrawNumber(pD3DDevice, &m_TextureBombNumber, TEXT("x0123456789"), m_ScoreInfo.lCellScore, nXOriginPos+116,nYOriginPos+295);
	}

	//春天
	if ( m_ScoreInfo.bChunTian)
	{
		m_TextureChunTian.DrawImage(pD3DDevice,nXOriginPos+250,nYOriginPos+290,SizeChunTian.cx,SizeChunTian.cy,
			0,0);
	}
	else if ( m_ScoreInfo.bFanChunTian )
	{
		m_TextureChunTian.DrawImage(pD3DDevice,nXOriginPos+250,nYOriginPos+290,SizeChunTian.cx,SizeChunTian.cy,
			SizeChunTian.cx,0);
	}

	return;
}

//隐藏窗口
VOID CScoreControl::CloseControl()
{
	//关闭窗口
	if (IsWindowActive()==true)
	{
		//关闭窗口
		DeleteWindow();
		
		//设置数据
		ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));
	}

	return;
}

//设置积分
VOID CScoreControl::SetScoreInfo(tagScoreInfo & ScoreInfo)
{
	//设置变量
	m_ScoreInfo=ScoreInfo;

	//灰化动画
	m_cbAlphaIndex=0;
	m_AlphaLapseCount.Initialization();

	//成绩动画
	m_cbCartoonIndex=0;
	m_CartoonLapseCount.Initialization();

	return;
}

// 绘画数字
void CScoreControl::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);

	// 加载资源
	INT nNumberHeight=ImageNumber->GetHeight();
	INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				ImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
