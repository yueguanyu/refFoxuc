#include "StdAfx.h"
#include ".\fishdraw.h"
#include "GameLogic.h"

CFishDraw::CFishDraw(void)
{
	m_nBackIndex = 0;
	m_rectFishPond.SetRect(0,0,0,0);
	m_bAddFish = FALSE;
	m_nCannonAlpha = 0;

	//    0
	//1        3
	//    2
	m_ptBullet[0].SetPoint(0,0);
	m_ptBullet[1].SetPoint(0,0);
	m_ptBullet[2].SetPoint(0,0);
	m_ptBullet[3].SetPoint(0,0);

	m_ptCannon[0].SetPoint(0,0);
	m_ptCannon[1].SetPoint(0,0);
	m_ptCannon[2].SetPoint(0,0);
	m_ptCannon[3].SetPoint(0,0);


	ZeroMemory(m_pBulletMove, sizeof(m_pBulletMove));
	ZeroMemory(m_pNetMove, sizeof(m_pNetMove));
	m_BulletCount.Initialization();
}

CFishDraw::~CFishDraw(void)
{
	for ( int i = 0; i < 4; ++i )
	{
		if( m_pBulletMove[i] )
		{
			delete m_pBulletMove[i];
			m_pBulletMove[i] = NULL;
		}

		if( m_pNetMove[i]  )
		{
			delete m_pNetMove[i];
			m_pNetMove[i] = NULL;
		}
	}

	int nIndex = 0;
	while( nIndex < m_ArrayFishDraw.GetCount())
	{
		SafeDelete(m_ArrayFishDraw[nIndex]);
		m_ArrayFishDraw.RemoveAt(nIndex);
		continue;
	}
}

//设置鱼池
VOID CFishDraw::SetFishPond( CRect rectFishPond )
{
	m_rectFishPond = rectFishPond;

	m_ptBullet[0].SetPoint( rectFishPond.left + rectFishPond.Width()/2, rectFishPond.top + 200 );
	m_ptBullet[1].SetPoint( rectFishPond.left + 200, rectFishPond.top + rectFishPond.Height()/2 );
	m_ptBullet[2].SetPoint( rectFishPond.left + rectFishPond.Width()/2, rectFishPond.bottom - 200 );
	m_ptBullet[3].SetPoint( rectFishPond.right - 200 , rectFishPond.top + rectFishPond.Height()/2 );
	
	m_ptCannon[0].SetPoint( rectFishPond.left + rectFishPond.Width()/2, rectFishPond.top + 38 + 80);
	m_ptCannon[1].SetPoint( rectFishPond.left + 45 + 70, rectFishPond.top + rectFishPond.Height()/2 - 6 + 5 );
	m_ptCannon[2].SetPoint( rectFishPond.left + rectFishPond.Width()/2, rectFishPond.bottom - 50 - 60 );
	m_ptCannon[3].SetPoint( rectFishPond.right - 45 - 70, rectFishPond.top + rectFishPond.Height()/2 - 15 + 5  );
}

//动画
VOID CFishDraw::OnFishMovie()
{
	if ( m_bAddFish )
	{
		m_nCannonAlpha += 20;
		if ( m_nCannonAlpha > 255)
			m_nCannonAlpha = 255;
	}
	else
	{
		m_nCannonAlpha -= 20;
		if ( m_nCannonAlpha < 0)
			m_nCannonAlpha = 0;
	}

	//添加鱼
	if ( m_bAddFish && m_LapseCount.GetLapseCount( 500 ) > 0 && m_rectFishPond.Width() > 0 && m_rectFishPond.Height() > 0 )
	{
		int nRand =  CGameLogic::GetInstance()->RandomArea(2, 3, 10 );
		switch ( nRand )
		{
		case 0:
			{
				INT		nNumber		= rand()%5 + 3;
				BYTE    cbFishIndex = CGameLogic::GetInstance()->RandomArea(2, 50, 20 );
				CPoint	ptFishMove[FISH_STEP];
				BYTE cbDirection = FishPath(ptFishMove);
				for ( INT y = 0; y < nNumber; ++y )
				{
					CPoint ptOffset;
					INT nTemp = ((rand()%2) == 1) ? -1 : 1;
					if ( cbDirection == 0x01 || cbDirection == 0x03 )
						ptOffset.SetPoint(((y + rand())%30),  ((y + rand())%30) * nTemp);
					else
						ptOffset.SetPoint(((y + rand())%30) * nTemp,  ((y + rand())%30));

					for( INT z = 0; z < FISH_STEP; ++z )
					{
						ptFishMove[z].x += ptOffset.x;
						ptFishMove[z].y += ptOffset.y;
					}


					BYTE  cbMoveTime[FISH_STEP] = { 10, 10, 10, 0, 0 };
					tgaFishDraw* pFishDraw = new tgaFishDraw( 0, cbFishIndex, (m_ArrayFishDraw.GetCount()%10 == 5), 0, 0, GetTickCount(), 3, ptFishMove, cbMoveTime);
					m_ArrayFishDraw.Add(pFishDraw);
				}
			}
			break;
		default:
			{
				CPoint ptMovePos[FISH_STEP];
				BYTE  cbMoveTime[FISH_STEP] = { 10, 10, 10, 0, 0 };
				FishPath(ptMovePos);
				int nFishIndex = CGameLogic::GetInstance()->RandomArea(FISH_MAX, 20, 18, 16, 15, 8, 3);
				tgaFishDraw* pFishDraw = new tgaFishDraw( 0, nFishIndex, (m_ArrayFishDraw.GetCount()%10 == 5), 0, 0, GetTickCount(), 3, ptMovePos, cbMoveTime);
				m_ArrayFishDraw.Add(pFishDraw);
			}
			break;
		}
	}
	
	//鱼移动
	int nIndex = 0;
	while( nIndex < m_ArrayFishDraw.GetCount())
	{
		if ( !m_ArrayFishDraw[nIndex]->Move() )
		{
			SafeDelete(m_ArrayFishDraw[nIndex]);
			m_ArrayFishDraw.RemoveAt(nIndex);
			continue;
		}

		if ( m_ArrayFishDraw[nIndex]->LapseCount.GetLapseCount(120)>0L )
			m_ArrayFishDraw[nIndex]->cbFrameIndex = (m_ArrayFishDraw[nIndex]->cbFrameIndex + 1)%4;

		//开炮
		CRect rectPond(m_rectFishPond);
		rectPond.left += 100;
		rectPond.top += 100;
		rectPond.right -= 100;
		rectPond.bottom -= 100;
		if ( rectPond.PtInRect(CPoint((int)m_ArrayFishDraw[nIndex]->ptPosition.x, (int)m_ArrayFishDraw[nIndex]->ptPosition.y)) && m_BulletCount.GetLapseCount(300) > 0 )
		{
			int nRand =  CGameLogic::GetInstance()->RandomArea(2, 8, 10 );
			nRand = 0;
			if ( nRand == 0 )
			{
				int nCannon =  CGameLogic::GetInstance()->RandomArea(4, 10, 10, 10, 10 );
				for ( int n = 0; n < 4; ++n )
				{
					if ( m_pBulletMove[nCannon] == NULL && m_pNetMove[nCannon] == NULL )
					{
						m_pBulletMove[nCannon] = new CObjectMove( m_ptCannon[nCannon], m_ArrayFishDraw[nIndex]->ptPosition, 1200, 80);
						m_ptBullet[nCannon].SetPoint( (int)m_ArrayFishDraw[nIndex]->ptPosition.x, (int)m_ArrayFishDraw[nIndex]->ptPosition.y );
						break;				
					}
					nCannon = (nCannon + 1)%4;
				}
			}
		}
		nIndex++;
	}

	for ( int i = 0; i < 4; ++i )
	{
		if( m_pBulletMove[i] && m_pBulletMove[i]->Move() && m_pNetMove[i] == NULL )
		{
			m_pNetMove[i] = new CObjectMove( m_pBulletMove[i]->ptPosition, m_pBulletMove[i]->ptPosition, 2000, 60);

			delete m_pBulletMove[i];
			m_pBulletMove[i] = NULL;
		}

		if( m_pNetMove[i]  )
		{
			m_pNetMove[i]->Move();
			if( m_pNetMove[i]->cbFrameIndex >= 12)
			{
				delete m_pNetMove[i];
				m_pNetMove[i] = NULL;
			}
		}
	}
}

//创建
VOID CFishDraw::OnFishCreate( CD3DDevice * pD3DDevice )
{
	//载入背景
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImgaeBack[0].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_BACK_I"),TEXT("FISH"));
	m_ImgaeBack[1].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_BACK_II"),TEXT("FISH"));
	m_ImgaeBack[2].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_BACK_III"),TEXT("FISH"));
	m_ImgaeBack[3].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_BACK_IV"),TEXT("FISH"));
	m_ImgaeBack[4].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_BACK_V"),TEXT("FISH"));
	m_ImgaeBack[5].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_BACK_VI"),TEXT("FISH"));
	m_ImgaeBack[6].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_BACK_VII"),TEXT("FISH"));

	m_ImgaeFish[0].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_1"),TEXT("FISH"), 4, 1);
	m_ImgaeFish[1].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_2"),TEXT("FISH"), 4, 1);
	m_ImgaeFish[2].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_3"),TEXT("FISH"), 4, 1);
	m_ImgaeFish[3].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_4"),TEXT("FISH"), 4, 1);
	m_ImgaeFish[4].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_5"),TEXT("FISH"), 4, 1);
	m_ImgaeFish[5].LoadImage(pD3DDevice,hResInstance,TEXT("FISH_6"),TEXT("FISH"), 4, 1);

	m_ImgaeGun.LoadImage(pD3DDevice,hResInstance,TEXT("FISH_GUN"),TEXT("FISH"), 4, 1);
	m_ImgaeCannon.LoadImage(pD3DDevice,hResInstance,TEXT("FISH_CANNON"),TEXT("FISH"), 2, 1);
	
	m_ImgaeBullet.LoadImage(pD3DDevice,hResInstance,TEXT("FISH_BULLET"),TEXT("FISH"));
	m_ImgaeNet.LoadImage(pD3DDevice,hResInstance,TEXT("FISH_NET"),TEXT("FISH"), 4, 3);

	//X背景
	for ( int i = 0 ; i < CountArray(m_ImgaeWater); ++i )
	{
		TCHAR szResource[64] = {0};
		_sntprintf(szResource, CountArray(szResource), TEXT("WATER_%d"),i + 1);
		m_ImgaeWater[i].LoadImage( pD3DDevice, hResInstance, szResource, TEXT("FISH") );
	}
}

//绘画
VOID CFishDraw::OnFishDraw( CD3DDevice * pD3DDevice, INT nWidth, INT nHeight )
{

	m_ImgaeBack[m_nBackIndex].DrawImage(pD3DDevice, m_rectFishPond.left, m_rectFishPond.top);
	
	//绘画鱼
	for ( INT i = 0 ; i < m_ArrayFishDraw.GetCount(); ++i )
	{	
		if( !m_ArrayFishDraw[i]->bWhetherToCreate)
			continue; 

		INT nImageHeight = m_ImgaeFish[m_ArrayFishDraw[i]->nFishType].GetHeight();
		INT nImageWidth = m_ImgaeFish[m_ArrayFishDraw[i]->nFishType].GetWidth();

		FLOAT fAngle = (FLOAT)atan2((m_ArrayFishDraw[i]->ptPosition.y - m_ArrayFishDraw[i]->ptLast.y) , (m_ArrayFishDraw[i]->ptLast.x - m_ArrayFishDraw[i]->ptPosition.x));
		fAngle += D3DX_PI/2;

		CPoint ptSite((int)m_ArrayFishDraw[i]->ptPosition.x, (int)m_ArrayFishDraw[i]->ptPosition.y);
		ptSite.x += m_rectFishPond.left;
		ptSite.y += m_rectFishPond.top;
		BYTE cbFrameIndex = m_ArrayFishDraw[i]->cbFrameIndex;
		m_ImgaeFish[m_ArrayFishDraw[i]->nFishType].DrawImage(pD3DDevice,CPoint(nImageWidth/2,nImageHeight/2), fAngle, 'z',ptSite.x - nImageWidth/2, ptSite.y - nImageHeight/2, m_ArrayFishDraw[i]->cbFrameIndex%4);
	}

	static INT nIndexC[22] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
	INT nIndex = nIndexC[(GetTickCount()/100)%22];
	m_ImgaeWater[nIndex].DrawImage(pD3DDevice, CPoint(0,0), 0.f, 'z', m_rectFishPond.left, m_rectFishPond.top, m_rectFishPond.Width(), m_rectFishPond.Height(), 0, 0, m_ImgaeWater[nIndex].GetWidth(), m_ImgaeWater[nIndex].GetHeight(), (BYTE)255);

	if ( m_nCannonAlpha <= 0 )
		return;

	if( m_nBackIndex == 0 )
		m_ImgaeBack[1].DrawImage(pD3DDevice, m_rectFishPond.left, m_rectFishPond.top, m_nCannonAlpha);

	//角度
	FLOAT fAngle[4];
	fAngle[0] = (FLOAT)atan2((FLOAT)(m_ptBullet[0].y - m_ptCannon[0].y), (FLOAT)(m_ptCannon[0].x - m_ptBullet[0].x)) + D3DX_PI/2*3 + D3DX_PI;
	fAngle[1] = -D3DX_PI/2 + (FLOAT)atan2((FLOAT)(m_ptBullet[1].y - m_ptCannon[1].y) , (FLOAT)(m_ptCannon[1].x - m_ptBullet[1].x)) + D3DX_PI;
	fAngle[2] = (FLOAT)atan2((FLOAT)(m_ptBullet[2].y - m_ptCannon[2].y) , (FLOAT)(m_ptCannon[2].x - m_ptBullet[2].x)) + D3DX_PI/2;
	fAngle[3] = D3DX_PI/2 +(FLOAT)atan2((FLOAT)(m_ptBullet[3].y - m_ptCannon[3].y) , (FLOAT)(m_ptCannon[3].x - m_ptBullet[3].x));

	//子弹
	for ( int i = 0; i < 4; ++i )
	{
		if( m_pBulletMove[i] )
		{
			m_ImgaeBullet.DrawImage(pD3DDevice, CPoint(m_ImgaeBullet.GetWidth()/2, m_ImgaeBullet.GetHeight()/2),  fAngle[i], 'z',(int)m_pBulletMove[i]->ptPosition.x - m_ImgaeBullet.GetWidth()/2, (int)m_pBulletMove[i]->ptPosition.y - m_ImgaeBullet.GetHeight()/2, (BYTE)m_nCannonAlpha);
		}
	}

	//绘画炮台	
	m_ImgaeGun.DrawImage(pD3DDevice, CPoint(m_ImgaeGun.GetWidth()/2, 32), fAngle[0], 'z',m_rectFishPond.left + m_rectFishPond.Width()/2 - m_ImgaeGun.GetWidth()/2, m_rectFishPond.top - 80 + 90, (BYTE)m_nCannonAlpha, 0);
	m_ImgaeGun.DrawImage(pD3DDevice, CPoint(m_ImgaeGun.GetWidth()/2, 32), fAngle[1], 'z',m_rectFishPond.left + 70, m_rectFishPond.top + m_rectFishPond.Height()/2 - m_ImgaeGun.GetHeight() + m_ImgaeGun.GetWidth()/2 - 13, (BYTE)m_nCannonAlpha, 2);
	m_ImgaeGun.DrawImage(pD3DDevice, CPoint(m_ImgaeGun.GetWidth()/2, 32), fAngle[2], 'z',m_rectFishPond.left + m_rectFishPond.Width()/2 - m_ImgaeGun.GetWidth()/2, m_rectFishPond.bottom - m_ImgaeGun.GetHeight() - 80, (BYTE)m_nCannonAlpha, 1);
	m_ImgaeGun.DrawImage(pD3DDevice, CPoint(m_ImgaeGun.GetWidth()/2, 32), fAngle[3], 'z',m_rectFishPond.right - 80 - 70, m_rectFishPond.top + m_rectFishPond.Height()/2 - m_ImgaeGun.GetHeight() + m_ImgaeGun.GetWidth()/2 - 13 , (BYTE)m_nCannonAlpha, 3);

	//渔网
	for ( int i = 0; i < 4; ++i )
	{
		if( m_pNetMove[i]  )
		{
			m_ImgaeNet.DrawImage(pD3DDevice, (int)m_pNetMove[i]->ptPosition.x - m_ImgaeNet.GetWidth()/2, (int)m_pNetMove[i]->ptPosition.y - m_ImgaeNet.GetHeight()/2, (BYTE)m_nCannonAlpha, m_pNetMove[i]->cbFrameIndex);
		}
	}
	
}

//鱼路径
BYTE CFishDraw::FishPath( CPoint* pFishMove )
{
	if ( pFishMove == NULL || m_rectFishPond.Width() == 0 || m_rectFishPond.Height() == 0 )
		return 0;

	INT nEdge = rand()%4;
	INT NDW	  = m_rectFishPond.Width();
	INT NDH	  = 500;
	INT NOL	  = 200;
	//开始位置
	INT nBeginPos[4][2] = { { NDW + NOL					, rand()% NDH	},
	{ rand() % NDW				, NDH + NOL		},
	{ -NOL						, rand() % NDH	},
	{ rand() % NDW				, -NOL			} };

	//中间位置 情况一
	INT nAmongPos[4][2] = { { (rand() % (NDW/3*2))		, rand()% NDH				},
	{ rand() % NDW				, (rand() % (NDH/3*2))		},
	{ (rand()%(NDW/3*2)) + NDW/3, rand() % NDH				},
	{ rand() % NDW				, (rand() % (NDH/3*2)) + NDH/3} };
	//中间位置 情况二
	INT nAmongPosTwo[4][2] = { { (rand() % (NDW/2))		, rand()% NDH				},
	{ rand() % NDW				, (rand() % (NDH/2))		},
	{ (rand()%(NDW/2)) + NDW/2, rand() % NDH				},
	{ rand() % NDW				, (rand() % (NDH/2)) + NDH/2} };

	//结束位置
	INT nEndPos[4][2] = {	{ -NOL						, rand() % NDH	},
	{ rand() % NDW				, -NOL			},
	{ NDW + NOL					, rand()% NDH	},
	{ rand() % NDW				, NDH + NOL		} };

	int nType = rand()%3;

	// 三点弧形
	if ( nType == 0 )
	{
		pFishMove[0].SetPoint( nBeginPos[nEdge][0], nBeginPos[nEdge][1] ) ;
		pFishMove[1].SetPoint( nAmongPosTwo[nEdge][0], nAmongPosTwo[nEdge][1] ) ;
		pFishMove[2].SetPoint( (nAmongPosTwo[nEdge][0] - nBeginPos[nEdge][0]) * 2 + nBeginPos[nEdge][0] , (nAmongPosTwo[nEdge][1] - nBeginPos[nEdge][1]) * 2 + nBeginPos[nEdge][1] );
	}
	// 三点圆圈
	else if ( nType == 1)
	{
		pFishMove[0].SetPoint( nBeginPos[nEdge][0], nBeginPos[nEdge][1] ) ;
		pFishMove[1].SetPoint( nAmongPos[nEdge][0], nAmongPos[nEdge][1] ) ;
		pFishMove[2].SetPoint( nBeginPos[nEdge][0], nBeginPos[nEdge][1] ) ;
	}
	// 随机三点圆圈
	else if ( nType == 2 )
	{
		pFishMove[0].SetPoint( nBeginPos[nEdge][0], nBeginPos[nEdge][1] ) ;
		pFishMove[1].SetPoint( rand()%900 + 100, rand()%600 + 100 ) ;
		pFishMove[2].SetPoint( nBeginPos[nEdge][0], nBeginPos[nEdge][1] ) ;
	}


	return nEdge;
}


