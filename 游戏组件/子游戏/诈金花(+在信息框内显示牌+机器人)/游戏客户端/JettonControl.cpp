#include "StdAfx.h"
#include "GameClient.h"
#include "JettonControl.h"

//////////////////////////////////////////////////////////////////////////

#define X_RADIUS					100									//绘画最大宽
#define	Y_RADIUS					80									//绘画最大高

#define MAX_DRAW					30									//绘画最大筹码数

#define MOVE_STEP_LEN				40									//步长

//////////////////////////////////////////////////////////////////////////


LONGLONG			CJettonControl::m_lJetonIndex[MAX_JETTON_INDEX] = {
	1L,5L,10L,50L,100L,500L,1000L,5000L,10000L,50000L,100000L,500000L,1000000L,5000000L};

//构造函数
CJettonControl::CJettonControl()
{
	//设置变量
	m_lScore=0L;
	m_BenchmarkPos.SetPoint(0,0);

	//动画变量
	m_lScorePending = 0L;
	m_rcDraw.SetRectEmpty();
	m_rcMove.SetRectEmpty();

	//加载位图
	HINSTANCE hInst = AfxGetInstanceHandle();
	m_PngJetton.LoadImage(hInst,TEXT("JETTON_VIEW"));

	m_nJettonHeight = m_PngJetton.GetHeight();
	m_nJettonWidth = m_PngJetton.GetWidth()/MAX_JETTON_INDEX;

	return;
}

//析构函数
CJettonControl::~CJettonControl()
{
}

//重置控件
VOID CJettonControl::ResetControl()
{
	//重置变量
	m_lScore=0L;

	//筹码索引
	m_arJetIndex.RemoveAll();
	m_arJetExcusions.RemoveAll();

	//待决索引
	m_arJetPending.RemoveAll();

	m_arIndexPending.RemoveAll();
	m_arExcusionsPend.RemoveAll();
	m_arStepCount.RemoveAll();
	m_arXStep.RemoveAll();
	m_arYStep.RemoveAll();

	//待决分
	m_lScorePending = 0L;
	m_rcDraw.SetRectEmpty();
	m_rcMove.SetRectEmpty();
}

//计算移动区域
VOID CJettonControl::ComputeMoveRect()
{
	//效验是否有待决动画
	if( m_arExcusionsPend.GetCount() == 0 ) return;

	//提取偏移
	CPoint &ptExcusion = m_arExcusionsPend[0];
	m_rcMove.SetRect(m_BenchmarkPos.x+ptExcusion.x-1,m_BenchmarkPos.y+ptExcusion.y-1,
		m_BenchmarkPos.x+ptExcusion.x+1,m_BenchmarkPos.y+ptExcusion.y+1);
	for( INT_PTR i = 1; i < m_arExcusionsPend.GetCount(); i++ )
	{
		CPoint &pt = m_arExcusionsPend[i];
		CPoint ptCur(m_BenchmarkPos.x+pt.x,m_BenchmarkPos.y+pt.y);
		if( !m_rcMove.PtInRect(ptCur) )
		{
			m_rcMove |= CRect(ptCur.x-1,ptCur.y-1,ptCur.x+1,ptCur.y+1);
		}
	}
}

//构造动画
BOOL CJettonControl::ConstructCartoon()
{	
	//如果还有未完成动画,则返回FALSE
	if( m_arIndexPending.GetCount() > 0 ) return FALSE;

	//若无待决筹码,则返回
	if( m_arJetPending.GetCount() == 0 ) return FALSE;

	//清除动画变量
	m_arExcusionsPend.RemoveAll();
	m_arStepCount.RemoveAll();
	m_arXStep.RemoveAll();
	m_arYStep.RemoveAll();
	m_lScorePending = 0L;
	m_rcDraw.SetRectEmpty();
	m_rcMove.SetRectEmpty();

	//提取待决筹码
	LONGLONG lFirstScore = m_arJetPending[0].lScore;
	for( ; ; )
	{
		tagJetPending &jp = m_arJetPending[0];

		//过滤0分筹码,和分数与第一个相反的筹码
		if( jp.lScore == 0L || (lFirstScore>0L&&jp.lScore<0L) || (lFirstScore<0L&&jp.lScore>0L)
			) 
		{
			if( jp.lScore == 0L )
				m_arJetPending.RemoveAt(0);
			break;
		}

		//计算筹码索引
		LONGLONG lJetScore = jp.lScore;
		while( lJetScore != 0 )
		{
			//定义变量
			WORD wIndexPending;
			CPoint ptFrom,ptTo,ptExcusion;
			INT nStepCount,nXStep,nYStep;

			//若是收入筹码
			if( lJetScore > 0L )
			{
				//提取索引
				for( WORD i = MAX_JETTON_INDEX-1; i >= 0; i-- )
				{
					if( lJetScore >= m_lJetonIndex[i] )
					{
						wIndexPending = i;
						lJetScore -= m_lJetonIndex[i];
						break;
					}
				}
				//生成起始点
				ptFrom = jp.ptJetton;

				//生成目标点
				ptTo.x = m_BenchmarkPos.x + (rand()%X_RADIUS)*((rand()%100)>49?1:-1);
				ptTo.y = m_BenchmarkPos.y + (rand()%Y_RADIUS)*((rand()%100)>49?1:-1);

				//生成步数
				if( abs(ptTo.x-ptFrom.x) > abs(ptTo.y-ptFrom.y) )
					nStepCount = abs(ptTo.x-ptFrom.x)/MOVE_STEP_LEN;
				else nStepCount = abs(ptTo.y-ptFrom.y)/MOVE_STEP_LEN;

				//生成X步长
				nXStep = (ptTo.x-ptFrom.x)/nStepCount;

				//生成Y步长
				nYStep = (ptTo.y-ptFrom.y)/nStepCount;

				//生成偏移
				ptExcusion.x = ptFrom.x-m_BenchmarkPos.x;
				ptExcusion.y = ptFrom.y-m_BenchmarkPos.y;
			}
			//否则
			else
			{
				//从库中提取索引
				INT_PTR nStoreIndex = 0;
				if( DistillJetIndex(lJetScore,nStoreIndex) )
				{
					wIndexPending = m_arJetIndex[nStoreIndex];
					lJetScore += m_lJetonIndex[wIndexPending];

					//删除库中筹码索引
					m_arJetIndex.RemoveAt(nStoreIndex);
				}
				else return FALSE;

				//生成偏移
				ptExcusion = m_arJetExcusions[nStoreIndex];

				//删除库中偏移
				m_arJetExcusions.RemoveAt(nStoreIndex);

				//生成起始点
				ptFrom.x = m_BenchmarkPos.x+ptExcusion.x;
				ptFrom.y = m_BenchmarkPos.y+ptExcusion.y;

				//生成目标点
				ptTo = jp.ptJetton;

				//生成步数
				if( abs(ptTo.x-ptFrom.x) > abs(ptTo.y-ptFrom.y) )
					nStepCount = abs(ptTo.x-ptFrom.x)/MOVE_STEP_LEN;
				else nStepCount = abs(ptTo.y-ptFrom.y)/MOVE_STEP_LEN;

				//生成X步长
				nXStep = (ptTo.x-ptFrom.x)/nStepCount;

				//生成Y步长
				nYStep = (ptTo.y-ptFrom.y)/nStepCount;
			}

			//构造动画
			m_arIndexPending.Add(wIndexPending);
			m_arExcusionsPend.Add(ptExcusion);
			m_arStepCount.Add(nStepCount);
			m_arXStep.Add(nXStep);
			m_arYStep.Add(nYStep);
		}

		//累加待决筹码分
		m_lScorePending += jp.lScore;
		
		//删除待决筹码
		m_arJetPending.RemoveAt(0);
		if( m_arJetPending.GetCount() == 0 ) break;
	}

	return m_arIndexPending.GetCount()>0?TRUE:FALSE;
}

//提取筹码索引
BOOL CJettonControl::DistillJetIndex( LONGLONG lScore,INT_PTR &nJetIndex )
{
	//效验正负
	if( lScore < 0L ) lScore = -lScore;

	//检查库存
	if( m_arJetIndex.GetCount() == 0 ) return FALSE;

	for( INT_PTR n = 0; n < m_arJetIndex.GetCount(); n++ )
	{
		WORD wIndex = m_arJetIndex[n];
		if( lScore >= m_lJetonIndex[wIndex] )
		{
			nJetIndex = n;
			return TRUE;
		}
	}

	INT_PTR nLastIndex = m_arJetIndex.GetCount()-1;

	//提取最后一个筹码分数
	WORD wJettonIndex = m_arJetIndex[nLastIndex];
	LONGLONG lLastScore = m_lJetonIndex[wJettonIndex];
	CPoint ptLastExcusion = m_arJetExcusions[nLastIndex];

	//从库中删除
	m_arJetIndex.RemoveAt(nLastIndex);
	m_arJetExcusions.RemoveAt(nLastIndex);

	//分解分数,并加入库中
	do
	{
		int i = wJettonIndex-1;
		if( i < 0 ) return FALSE;
		for( ; i >= 0; i-- )
		{
			while( lLastScore >= m_lJetonIndex[i] )
			{
				//加入库中
				m_arJetIndex.Add(i);
				m_arJetExcusions.Add(ptLastExcusion);

				//减去筹码分
				lLastScore -= m_lJetonIndex[i];

				//重新生成偏移
				ptLastExcusion.x = (rand()%X_RADIUS)*((rand()%100)>49?1:-1);
				ptLastExcusion.y = (rand()%Y_RADIUS)*((rand()%100)>49?1:-1);
			}
			if( lLastScore == 0 ) break;
		}

		//检查是否可提取
		wJettonIndex = m_arJetIndex[nLastIndex];
		if( lScore >= m_lJetonIndex[wJettonIndex] ) break;

		//不可提取,则再分解
		nLastIndex = m_arJetIndex.GetCount()-1;
		wJettonIndex = m_arJetIndex[nLastIndex];
		lLastScore = m_lJetonIndex[wJettonIndex];
		ptLastExcusion = m_arJetExcusions[nLastIndex];
		m_arJetIndex.RemoveAt(nLastIndex);
		m_arJetExcusions.RemoveAt(nLastIndex);
	}while(true);

	nJetIndex = nLastIndex;
	return TRUE;
}

//压缩筹码,把小值筹码组合成大值筹码
VOID CJettonControl::CompactJetIndex()
{
	//检查库存
	if( m_arJetIndex.GetCount() == 0 ) return;

	//确定最大压缩筹码值
	LONGLONG lMostScore = m_lJetonIndex[MAX_JETTON_INDEX-1];
	LONGLONG lScore = 0L;

	//定义变量
	CWHArray<WORD> arJetIndex;
	CWHArray<CPoint> arJetExcusions;
	arJetIndex.Copy( m_arJetIndex );
	arJetExcusions.Copy( m_arJetExcusions );

	INT_PTR nCompactCount,nPreCompactCount;
	do
	{
		//提取库存
		nPreCompactCount = arJetIndex.GetCount();
		nCompactCount = 0;
		for( INT_PTR i = 0; i < arJetIndex.GetCount(); i++ )
		{
			//提取筹码值
			WORD wJetIndex = arJetIndex[i];
			lScore += m_lJetonIndex[wJetIndex];
		}
		//删除筹码
		arJetIndex.RemoveAll();
		//删除偏移
		arJetExcusions.RemoveAll();

		if( lScore == 0L ) break;

		INT_PTR n = arJetIndex.GetCount();

		//入库
		for( int i = MAX_JETTON_INDEX-1; i >= 0; i-- )
		{
			while( lScore >= m_lJetonIndex[i] )
			{
				//加入索引库
				arJetIndex.Add(i);

				//减去筹码分
				lScore -= m_lJetonIndex[i];

				//重新生成偏移
				CPoint ptExcusion;
				ptExcusion.x = (rand()%X_RADIUS)*((rand()%100)>49?1:-1);
				ptExcusion.y = (rand()%Y_RADIUS)*((rand()%100)>49?1:-1);

				//添加偏移
				arJetExcusions.Add( ptExcusion );

				//
				nCompactCount++;
			}
			if( lScore == 0 ) break;
		}
	}while( nCompactCount < nPreCompactCount );
	
	//复制入库
	m_arJetIndex.Copy( arJetIndex );
	m_arJetExcusions.Copy( arJetExcusions );
}

//加筹码
VOID CJettonControl::AddScore( LONGLONG lScore, CPoint ptFrom )
{
	if( lScore > 0L )
	{
		tagJetPending jp;
		jp.lScore = lScore;
		jp.ptJetton = ptFrom;
		m_arJetPending.Add(jp);
	}
}

//移除筹码
VOID CJettonControl::RemoveScore( LONGLONG lScore,CPoint ptTo )
{
	//如果分数大于0
	if( lScore > 0L )
	{
		tagJetPending jp;

		//取反
		jp.lScore = -lScore;
		jp.ptJetton = ptTo;
		m_arJetPending.Add(jp);
	}
}

//移除所有筹码
VOID CJettonControl::RemoveAllScore( CPoint ptTo )
{
	RemoveScore(m_lScore,ptTo);
}

//开始筹码动画
BOOL CJettonControl::BeginMoveJettons()
{
	//如果待决筹码不为0,最后一个筹码分不为0
	INT_PTR nJetPendCount = m_arJetPending.GetCount();
	if( nJetPendCount > 0 && m_arJetPending[nJetPendCount-1].lScore != 0 )
	{
		//插入空筹码
		tagJetPending jp;
		jp.lScore = 0L;
		jp.ptJetton.SetPoint(0,0);
		m_arJetPending.Add(jp);
	}	

	//构造动画结构
	if( ConstructCartoon() )
	{
		ComputeMoveRect();
		m_rcDraw = m_rcMove;
		return TRUE;
	}

	return FALSE;
}

//筹码动画
BOOL CJettonControl::PlayMoveJettons()
{
	BOOL bMoving = FALSE;
	if( m_arIndexPending.GetCount() > 0 )
	{
		//遍历待决索引
		for( INT_PTR i = 0; i < m_arIndexPending.GetCount(); i++ )
		{
			//如果未完成移动
			INT &nStepCount = m_arStepCount[i];
			if( nStepCount > 0 )
			{
				//调整偏移
				CPoint &pt = m_arExcusionsPend[i];
				INT nXStep = m_arXStep[i];
				INT nYStep = m_arYStep[i];
				pt.x += nXStep;
				pt.y += nYStep;
				//减少步数
				nStepCount--;
				//设置移动状态
				bMoving = TRUE;
			}
		}

		//如果有移动
		if( bMoving )
			//计算移动区域
			ComputeMoveRect();

		//如果已完成所有移动
		if( !bMoving )
		{
			//如果是收入动画
			if( m_lScorePending > 0L )
			{
				//添加筹码索引入库
				m_arJetIndex.Append(m_arIndexPending);

				//添加偏移入库
				m_arJetExcusions.Append(m_arExcusionsPend);
			}

			//更新库存筹码分数
			m_lScore += m_lScorePending;

			//删除动画相关变量
			m_arExcusionsPend.RemoveAll();
			m_arIndexPending.RemoveAll();
			m_arStepCount.RemoveAll();
			m_arXStep.RemoveAll();
			m_arYStep.RemoveAll();

			//判断是否还有请求动画
			if( !BeginMoveJettons() )
			{
				//如果筹码数大于MAX_DRAW,则压缩筹码
				if( m_arJetIndex.GetCount() >= MAX_DRAW )
				{
					CompactJetIndex();
				}
				return FALSE;
			}
			else return TRUE;
		}
	}
	return bMoving;
}

//停止筹码动画
BOOL CJettonControl::FinishMoveJettons()
{	
	BOOL bMoveJetton = FALSE;

	//如果正在动画,或还有待决筹码
	while( m_arIndexPending.GetCount() > 0 || ConstructCartoon() )
	{
		//遍历待决索引
		for( INT_PTR i = 0; i < m_arIndexPending.GetCount(); i++ )
		{
			//如果有未完成移动
			INT nStepCount = m_arStepCount[i];
			if( nStepCount > 0 )
			{
				//调整偏移
				CPoint &pt = m_arExcusionsPend[i];
				pt.x += m_arXStep[i]*nStepCount;
				pt.y += m_arYStep[i]*nStepCount;
			}
		}

		//如果是收入筹码
		if( m_lScorePending > 0L )
		{
			//添加入库
			m_arJetIndex.Append(m_arIndexPending);
			m_arJetExcusions.Append(m_arExcusionsPend);
		}

		//更新库存筹码分数
		m_lScore += m_lScorePending;

		//删除动画变量
		m_arExcusionsPend.RemoveAll();
		m_arIndexPending.RemoveAll();
		m_arStepCount.RemoveAll();
		m_arXStep.RemoveAll();
		m_arYStep.RemoveAll();

		//
		bMoveJetton = TRUE;
	}

	//如果筹码数大于MAX_DRAW,则压缩筹码
	if( m_arJetIndex.GetCount() >= MAX_DRAW )
	{
		CompactJetIndex();
	}

	return bMoveJetton;
}

//基准位置
VOID CJettonControl::SetBenchmarkPos(INT nXPos, INT nYPos)
{

	//调整绘画区域
	if( !m_rcDraw.IsRectEmpty() )
		m_rcDraw.InflateRect(abs(m_BenchmarkPos.x-nXPos),abs(m_BenchmarkPos.y-nYPos));
	if( !m_rcMove.IsRectEmpty() )
		m_rcMove.OffsetRect(nXPos-m_BenchmarkPos.x,nYPos-m_BenchmarkPos.y);

	//设置变量
	m_BenchmarkPos.SetPoint(nXPos,nYPos);

	return;
}

//绘画控件
VOID CJettonControl::DrawJettonControl(CDC * pDC)
{
	//绘画判断 
	if (m_lScore<=0L&&m_arIndexPending.GetCount()==0) return;

	INT nXPos,nYPos;
	INT_PTR i;

	//绘画已添加筹码,只画最后MAX_DRAW
	if( m_arJetIndex.GetCount() > MAX_DRAW )
		i = m_arJetIndex.GetCount()-MAX_DRAW;
	else i = 0;

	for( ; i < m_arJetIndex.GetCount(); i++ )
	{
		WORD wJetIndex = m_arJetIndex[i];
		nXPos = m_BenchmarkPos.x + m_arJetExcusions[i].x - m_nJettonWidth/2;
		nYPos = m_BenchmarkPos.y + m_arJetExcusions[i].y - m_nJettonHeight/2;

		//绘画筹码
		m_PngJetton.DrawImage(pDC,nXPos,nYPos,m_nJettonWidth,m_nJettonHeight,m_nJettonWidth*wJetIndex,0);
	}

	//绘画移动中的筹码
	if( m_arIndexPending.GetCount() > 0 )
	{
		//绘画筹码,只画最后MAX_DRAW
		if( m_arIndexPending.GetCount() > MAX_DRAW )
			i = m_arIndexPending.GetCount() - MAX_DRAW;
		else i = 0;

		for( ; i < m_arIndexPending.GetCount(); i++ )
		{
			WORD wJetIndex = m_arIndexPending[i];
			nXPos = m_BenchmarkPos.x + m_arExcusionsPend[i].x - m_nJettonWidth/2;
			nYPos = m_BenchmarkPos.y + m_arExcusionsPend[i].y - m_nJettonHeight/2;

			//绘画筹码
			m_PngJetton.DrawImage(pDC,nXPos,nYPos,m_nJettonWidth,m_nJettonHeight,m_nJettonWidth*wJetIndex,0);
		}

		//去除绘画区域
		m_rcDraw = m_rcMove;
	}

	return;
}

//是否在动画
BOOL CJettonControl::IsPlayMoving()
{
	return m_arIndexPending.GetCount()>0;
}

//获取更新绘画区域
VOID CJettonControl::GetDrawRect( CRect &rc )
{
	rc = m_rcMove | m_rcDraw;
	rc.InflateRect(m_nJettonWidth/2,m_nJettonHeight/2);
	return;
}

//////////////////////////////////////////////////////////////////////////
