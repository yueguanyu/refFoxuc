// PhraseSoundWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "PhraseSoundWnd.h"
#include ".\phrasesoundwnd.h"

//宏定义
///////////////////////////////////////////////////////////////////

#define IDC_PAGE_UP						108            //上一页
#define IDC_PAGE_DOWN					109            //下一页

#define ITEM_WIDTH                      176            //子项宽度
#define ITEM_HEIGHT                     22             //子项高度
#define ITEM_DISS                       1              //纵向间距
#define TOP_DISS                        13              //顶部间距 

///////////////////////////////////////////////////////////////////


// CPhraseSoundWnd

//Message Map
BEGIN_MESSAGE_MAP(CPhraseSoundWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOVE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()

	ON_BN_CLICKED(IDC_PAGE_UP, OnBnClickSoundPageUp)
	ON_BN_CLICKED(IDC_PAGE_DOWN, OnBnClickSoundPageDown)
END_MESSAGE_MAP()


//构造函数
CPhraseSoundWnd::CPhraseSoundWnd()
{
	//初始化变量
	m_cbCurrPageIndex=0;
	m_cbCountPerPage=10;
	m_cbCurrSelected=255;
	m_cbLastSelected=255;
	m_cbCurrItemType=0;

	m_BrushNormal.CreateSolidBrush(RGB(39,63,99));
    m_BrushSelected.CreateSolidBrush(RGB(59,93,139));

	//加载资源
	HINSTANCE hInstance= AfxGetInstanceHandle();
	m_ImageBack.SetLoadInfo(IDB_SOUND_BACK,hInstance,true);
	//m_ImageNormal.LoadImage(hInstance,"ITEM_NORMAL");
	//m_ImageSelected.LoadImage(hInstance,"ITEM_SELECTED");

}

//析构函数
CPhraseSoundWnd::~CPhraseSoundWnd()
{
//	ASSERT(m_PhraseSoundList!=NULL);
	if(m_PhraseSoundList.IsEmpty()) return;

	for(int i=0;i<m_PhraseSoundList.GetCount();i++)
		delete m_PhraseSoundList[i];
	m_PhraseSoundList.RemoveAll();
}


//添加子项
VOID  CPhraseSoundWnd::AddPhraseSoundItem(LPCTSTR lpItemText,BYTE cbItemType)
{
	//新建子项
	tagPhraseSoundItem *  PhraseSoundItem = new tagPhraseSoundItem();
    PhraseSoundItem->bDisplay=true;
	PhraseSoundItem->bSelected=false;
	PhraseSoundItem->cbItemType=cbItemType;
	CopyMemory(PhraseSoundItem->DisplayText,lpItemText,sizeof(PhraseSoundItem->DisplayText));

    return AddPhraseSoundItem(PhraseSoundItem);
}

//添加子项
VOID  CPhraseSoundWnd::AddPhraseSoundItem(tagPhraseSoundItem * pPhraseSoundItem)
{
	//子项验证
	ASSERT(pPhraseSoundItem!=NULL);

	m_PhraseSoundList.Add(pPhraseSoundItem);
}

//删除子项
VOID  CPhraseSoundWnd::DeletePhraseSoundItem(LPCTSTR lpItemText)
{
	/*if(lpItemText==NULL) return;
	for(int i=0;i<m_PhraseSoundList.)*/
}

//移除子项
VOID  CPhraseSoundWnd::RemovePhraseSoundItem(const tagPhraseSoundItem * pPhraseSoundItem)
{

}

//坐标转换
tagPhraseSoundItem *  CPhraseSoundWnd::TransPointToPhraseSoundItem(const CPoint  point)
{
	for(int i=0;i<m_PhraseSoundList.GetCount();i++)
	{
		tagPhraseSoundItem * pPhraseSoundItem = (tagPhraseSoundItem *)m_PhraseSoundList[i];
		if((pPhraseSoundItem->crDisplayRect.PtInRect(point)) && (pPhraseSoundItem->bDisplay==TRUE))
		{
			UpdateItemsSelectedStatus(true,i);
			m_cbLastSelected=m_cbCurrSelected;
			m_cbCurrSelected=i;
			return  pPhraseSoundItem;
		}
	}
	return NULL;
}

//获取选中文本
VOID CPhraseSoundWnd::GetSelectedText(CString & csWindowText,int nSelectIndex) 
{
	csWindowText.Empty();

	if(nSelectIndex!=255) 
		csWindowText=((tagPhraseSoundItem *)m_PhraseSoundList.GetAt(nSelectIndex))->DisplayText;
};


//更新状态
VOID  CPhraseSoundWnd::UpdateItemsDisplayStatus(bool bShow,int nStartIndex,int nEndIndex)
{
	if(m_PhraseSoundList.GetCount()==0) return;

	for(int i=0;i<m_PhraseSoundList.GetCount();i++)
	{
		tagPhraseSoundItem * pPhraseSoundItem = (tagPhraseSoundItem *)m_PhraseSoundList[i];
		ASSERT(pPhraseSoundItem!=NULL);
		if(i >=nStartIndex && i <=nEndIndex)
			pPhraseSoundItem->bDisplay=bShow;
		else
			pPhraseSoundItem->bDisplay=!bShow;
	}

}

//更新状态
VOID  CPhraseSoundWnd::UpdateItemsSelectedStatus(bool bSelected,int nSlectedIndex)
{
//	ASSERT(m_PhraseSoundList!=NULL);
	if(m_PhraseSoundList.GetCount()==0) return;

	//设置子项选中状态
	BYTE cbItemCount=0;
	for(int i=0;i<m_PhraseSoundList.GetCount();i++)
	{
		tagPhraseSoundItem * pPhraseSoundItem = (tagPhraseSoundItem *)m_PhraseSoundList[i];
		ASSERT(pPhraseSoundItem!=NULL);
		if(pPhraseSoundItem->bDisplay==TRUE)
		{
			if(cbItemCount>=m_cbCountPerPage) break;
			if(i ==nSlectedIndex)
				pPhraseSoundItem->bSelected=bSelected;
			else
				pPhraseSoundItem->bSelected=!bSelected;
			++cbItemCount;
		}
	}
}

//调整区域
VOID  CPhraseSoundWnd::AdjustItemDisplayRect()
{
//	ASSERT(m_PhraseSoundList!=NULL);
	if(m_PhraseSoundList.GetCount()==0) return;

	//调整子项区域
	BYTE cbItemCount=0;
	BYTE cbItemTypeStart=255;
	CRect ClientRect;
	GetClientRect(&ClientRect);
	CRect rcItemDislplay((ClientRect.Width() - ITEM_WIDTH)/2,TOP_DISS,(ClientRect.Width() - ITEM_WIDTH)/2+ITEM_WIDTH,TOP_DISS+ITEM_HEIGHT);
	for(int i=0;i<m_PhraseSoundList.GetCount();i++)
	{
		//设置区域
		tagPhraseSoundItem * pPhraseSoundItem = (tagPhraseSoundItem *)m_PhraseSoundList.GetAt(i);
		ASSERT(pPhraseSoundItem!=NULL);
		if(pPhraseSoundItem->cbItemType==m_cbCurrItemType)
		{
			if(cbItemTypeStart==255) cbItemTypeStart=i;
		    pPhraseSoundItem->crDisplayRect=rcItemDislplay;

			//子项递增
			++cbItemCount;

			//重新调整区域
			if(i >0 && cbItemCount > 0 && cbItemCount%m_cbCountPerPage==0) 
				rcItemDislplay.OffsetRect(0,-(ITEM_HEIGHT+ITEM_DISS) * (m_cbCountPerPage-1)); 
			else
	    		//区域偏移
		    	rcItemDislplay.OffsetRect(0,ITEM_HEIGHT+ITEM_DISS);
		}
	}
    UpdateItemsDisplayStatus(TRUE,cbItemTypeStart,__min(cbItemTypeStart+__min(m_cbCountPerPage-1,cbItemCount-1),m_PhraseSoundList.GetCount()));
	UpdateButtonsStatus(cbItemCount<=m_cbCountPerPage);
}


// CPhraseSoundWnd 消息处理程序

//下页按钮
VOID CPhraseSoundWnd::OnBnClickSoundPageDown()
{
	BYTE cbItemCount=0;
	BYTE cbCurrTypeStartIndex=255;
	for(int i=0;i<m_PhraseSoundList.GetCount();i++)
	{
		tagPhraseSoundItem * pPhraseSoundItem = (tagPhraseSoundItem *)m_PhraseSoundList.GetAt(i);
		ASSERT(pPhraseSoundItem!=NULL);
		if(pPhraseSoundItem->cbItemType==m_cbCurrItemType)
		{
			if(cbCurrTypeStartIndex==255) cbCurrTypeStartIndex=i;
			++cbItemCount;
		}
    }

	++m_cbCurrPageIndex;

	//计算当前页首尾索引
	BYTE cbStartIndex=cbCurrTypeStartIndex+m_cbCurrPageIndex * m_cbCountPerPage;
	BYTE cbEndIndex=cbStartIndex+__min(m_cbCountPerPage-1,cbItemCount-cbStartIndex-1);

	//选中失效
	if(m_cbCurrSelected!=255) ((tagPhraseSoundItem *)m_PhraseSoundList.GetAt(m_cbCurrSelected))->bSelected=false;
	m_cbCurrSelected=255;

	//更新显示状态
	UpdateItemsDisplayStatus(true,cbStartIndex,cbEndIndex);
	
	//更新按钮状态
	UpdateButtonsStatus(cbEndIndex>=cbCurrTypeStartIndex+cbItemCount-1);
    
	//计算更新区域
	CRect crUpdateRect(12,10,190,242);
    
	//更新界面
	InvalidateRect(crUpdateRect,false);
}

//上页按钮
VOID CPhraseSoundWnd::OnBnClickSoundPageUp()
{
	//第一页没有向上
	if(m_cbCurrPageIndex==0) return;

	BYTE cbItemCount=0;
	BYTE cbCurrTypeStartIndex=255;
	for(int i=0;i<m_PhraseSoundList.GetCount();i++)
	{
		tagPhraseSoundItem * pPhraseSoundItem = (tagPhraseSoundItem *)m_PhraseSoundList.GetAt(i);
		ASSERT(pPhraseSoundItem!=NULL);
		if(pPhraseSoundItem->cbItemType==m_cbCurrItemType)
		{
			if(cbCurrTypeStartIndex==255) cbCurrTypeStartIndex=i;
			++cbItemCount;
		}
	}
	--m_cbCurrPageIndex;
	BYTE cbStartIndex=cbCurrTypeStartIndex+m_cbCurrPageIndex * m_cbCountPerPage;
	BYTE cbEndIndex=cbStartIndex+m_cbCountPerPage-1;

	//选中失效
	if(m_cbCurrSelected!=255) ((tagPhraseSoundItem *)m_PhraseSoundList.GetAt(m_cbCurrSelected))->bSelected=false;
	m_cbCurrSelected=255;

	//更新显示状态
	UpdateItemsDisplayStatus(true,cbStartIndex,cbEndIndex);
	
	//更新按钮状态
	UpdateButtonsStatus(cbEndIndex>=cbCurrTypeStartIndex+cbItemCount-1);

	//计算更新区域
	CRect crUpdateRect(12,10,190,242);
    
	//更新界面
	InvalidateRect(crUpdateRect,false);
}

//更新按钮状态
VOID  CPhraseSoundWnd::UpdateButtonsStatus(bool lLastPage)
{
	if(m_cbCurrPageIndex==0)
	{
		m_btPageUp.EnableWindow(false);
		if(lLastPage==TRUE)
		    m_btPageDown.EnableWindow(false);
		else
			m_btPageDown.EnableWindow(true);
	}
	else
	{
		m_btPageUp.EnableWindow(true);
		if(lLastPage==TRUE)
		    m_btPageDown.EnableWindow(false);
		else
			m_btPageDown.EnableWindow(true);
	}
}

//计算联合区域
VOID  CPhraseSoundWnd::ClacUniteRect(CRect & UpdateRect,int nStartIndex,int nEndIndex)
{
	//清空区域
	UpdateRect.SetRectEmpty();

	tagPhraseSoundItem * pPhraseSoundItem=NULL;
    for(int i=0;i<m_PhraseSoundList.GetCount();i++)
	{
		if(i>=nStartIndex && i< nEndIndex)
		{
			pPhraseSoundItem = (tagPhraseSoundItem *)m_PhraseSoundList[i];
			UpdateRect.UnionRect(UpdateRect,pPhraseSoundItem->crDisplayRect);
		}
	}
}

void CPhraseSoundWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_ImageBack.AlphaDrawImage(&dc,0,0,RGB(255,0,255));

	//设置上下文设备
	SetBkMode(dc,TRANSPARENT);

	//绘制子项
	tagPhraseSoundItem * pPhraseSoundItem=NULL;
	BYTE cbDisplayerCount=0;
	CFont Newfont;
	CFont * Oldfont;
	Newfont.CreatePointFont(100,TEXT("宋体"));
	Oldfont = dc.SelectObject(&Newfont);
	for(int i=0;i<m_PhraseSoundList.GetCount();i++)
	{
		if(cbDisplayerCount>=m_cbCountPerPage) break;
		pPhraseSoundItem = (tagPhraseSoundItem *)m_PhraseSoundList[i];
		if(pPhraseSoundItem->bDisplay==TRUE)
		{
			if(pPhraseSoundItem->bSelected==TRUE)
			{
				dc.FillRect(pPhraseSoundItem->crDisplayRect,&m_BrushSelected);
				//m_ImageSelected.DrawImage(&dc,pPhraseSoundItem->crDisplayRect.left,pPhraseSoundItem->crDisplayRect.top);
			}
			else
			{
				dc.FillRect(pPhraseSoundItem->crDisplayRect,&m_BrushNormal);
				//m_ImageNormal.DrawImage(&dc,pPhraseSoundItem->crDisplayRect.left,pPhraseSoundItem->crDisplayRect.top);
			}
				
			SetTextColor(dc,RGB(255,255,0));
			DrawText(dc,pPhraseSoundItem->DisplayText,strlen(pPhraseSoundItem->DisplayText),pPhraseSoundItem->crDisplayRect,DT_LEFT|DT_VCENTER| DT_SINGLELINE          |DT_END_ELLIPSIS);		

			++cbDisplayerCount;
		}
	}
	dc.SelectObject(Oldfont);
	Newfont.DeleteObject();    
	return;
}

void CPhraseSoundWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	tagPhraseSoundItem * pPhraseSoundItem;
	if((pPhraseSoundItem=TransPointToPhraseSoundItem(point))!=NULL)
	{
		CRect crUpdate(0,0,0,0);
		if(m_cbLastSelected!=255)
		  crUpdate.UnionRect(crUpdate,((tagPhraseSoundItem *)m_PhraseSoundList[m_cbLastSelected])->crDisplayRect);
      	crUpdate.UnionRect(crUpdate,pPhraseSoundItem->crDisplayRect);
		
		if(m_cbLastSelected!=m_cbCurrSelected)
		  InvalidateRect(crUpdate,false);

	}         
	CWnd::OnMouseMove(nFlags, point);
}

void CPhraseSoundWnd::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

	//重画窗体
	//Invalidate();
}

BOOL CPhraseSoundWnd::OnEraseBkgnd(CDC* pDC)
{
	//if(!(m_ImageBack.IsNull))
	//{
	   //m_ImageBack.AlphaDrawImage(pDC,0,0,RGB(255,0,255));
	//}
	return CWnd::OnEraseBkgnd(pDC);
}

int CPhraseSoundWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;

	//上一页按钮
	CRect  PageUpRect(30, 254, 25+205/5, 254+18);
	HINSTANCE hInstance=AfxGetInstanceHandle();
	HDWP hDwp=BeginDeferWindowPos(32);
	m_btPageUp.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,PageUpRect,this,IDC_PAGE_UP);
	m_btPageUp.SetButtonImage(IDB_PAGE_UP,hInstance,false);
	m_btPageUp.EnableWindow(false);
	m_btPageUp.ShowWindow(SW_SHOW);

	//下一页按钮
	CRect  PageDownRect(115, 254, 25+205/5, 254+18);
	m_btPageDown.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,PageDownRect,this,IDC_PAGE_DOWN);
	m_btPageDown.SetButtonImage(IDB_PAGE_DOWN,hInstance,false);
	m_btPageDown.EnableWindow(false);
	m_btPageDown.ShowWindow(SW_SHOW);

	//添加语音
	//男声
	AddPhraseSoundItem(_T("快点些"),1);
	AddPhraseSoundItem(_T("快点来啊"),1);
	AddPhraseSoundItem(_T("快点啊！老板娘"),1);
	AddPhraseSoundItem(_T("快点啊老板娘"),1);
	AddPhraseSoundItem(_T("快点啊，都要天亮了"),1);
	AddPhraseSoundItem(_T("快点啊！我要去逛街了"),1);
	AddPhraseSoundItem(_T("快带出牌啊！天都要亮了"),1);
	AddPhraseSoundItem(_T("快点啊！我还要去逛街呢"),1);
	AddPhraseSoundItem(_T("来来来，坐下啊 不要浪费时间"),1);

	//女声
	AddPhraseSoundItem(_T("快点来啊"),2);	
	AddPhraseSoundItem(_T("快点啊，美女"),2);
	AddPhraseSoundItem(_T("快点啊，我还有事的"),2);
	AddPhraseSoundItem(_T("快点打出来啊，天都要亮了"),2);
	AddPhraseSoundItem(_T("快点快点，要来在打下"),2);
	AddPhraseSoundItem(_T("快点坐下来啊，这里三缺一不要浪费时间"),2);


	SetWindowPos(NULL,0,0,200,276,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);

	//调整区域
	AdjustItemDisplayRect();

	return 0;
}

void CPhraseSoundWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	tagPhraseSoundItem * pPhraseSoundItem;
	if((pPhraseSoundItem=TransPointToPhraseSoundItem(point))!=NULL)
	{
		AfxGetMainWnd()->SendMessage(WM_USER+110,m_cbCurrSelected,0);
		ShowWindow(FALSE);
	}
	//CWnd::OnLButtonDblClk(nFlags, point);
}
