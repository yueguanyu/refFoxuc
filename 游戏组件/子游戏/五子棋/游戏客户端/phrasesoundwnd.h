#pragma once

#include "Stdafx.h"
#include "Resource.h"

//结构定义
//////////////////////////////////////////////////////////////////
struct  tagPhraseSoundItem
{
	BOOL                              bDisplay;                //是否显示
	BOOL                              bSelected;               //是否选中 
	BYTE                              cbItemType;              //子项类型
	CRect                             crDisplayRect;           //显示区域
	TCHAR                             DisplayText[128];        //显示文本
public:
	tagPhraseSoundItem()
	{
		//初始化变量
		crDisplayRect.SetRectEmpty();
		ZeroMemory(DisplayText,sizeof(DisplayText));
		bDisplay=false;
		bSelected=false;
	}
	~tagPhraseSoundItem()
	{
		crDisplayRect.SetRectEmpty();
		ZeroMemory(DisplayText,sizeof(DisplayText));
	}
	
};
//////////////////////////////////////////////////////////////////


// CPhraseSoundWnd

//语音短语窗体
class CPhraseSoundWnd : public CWnd
{
	//控制变量
private:
	CPtrArray                        m_PhraseSoundList;        //语音列表
	BYTE                             m_cbCurrPageIndex;        //当前页数
	BYTE                             m_cbCountPerPage;         //页显示数
	BYTE                             m_cbLastSelected;         //上次选中
	BYTE                             m_cbCurrSelected;         //当前选中
	BYTE                             m_cbCurrItemType;         //子项类型

	//资源变量
private:
	CSkinImage                       m_ImageBack;              //窗体背景
	CSkinButton                      m_btPageUp;               //上页按钮
	CSkinButton                      m_btPageDown;             //下页按钮
	//CPngImage                        m_ImageNormal;            //普通状态
//	CPngImage                        m_ImageSelected;          //选中状态
	CBrush                           m_BrushNormal;            //普通画刷
	CBrush                           m_BrushSelected;          //选中画刷  

public:
	//构造函数
	CPhraseSoundWnd();
	//析构函数
	virtual ~CPhraseSoundWnd();

	//功能函数
public:
	//添加子项
	VOID  AddPhraseSoundItem(LPCTSTR lpItemText,BYTE cbItemType);
	//添加子项
	VOID  AddPhraseSoundItem(tagPhraseSoundItem * pPhraseSoundItem);
	//删除子项
	VOID  DeletePhraseSoundItem(LPCTSTR lpItemText);
	//移除子项
	VOID  RemovePhraseSoundItem(const tagPhraseSoundItem * pPhraseSoundItem);
	//坐标转换
    tagPhraseSoundItem *  TransPointToPhraseSoundItem(const CPoint  point);
	//更新状态
	VOID  UpdateItemsDisplayStatus(bool bShow,int nStartIndex,int nEndIndex);
	//更新状态
	VOID  UpdateItemsSelectedStatus(bool bSelected,int nSlectedIndex);
	//调整区域
	VOID  AdjustItemDisplayRect();
	//设置子项类型
	VOID  SetItemType(BYTE cbItemType) { m_cbCurrItemType=cbItemType; AdjustItemDisplayRect(); };
	//更新按钮状态
	VOID  UpdateButtonsStatus(bool lLastPage);
	//计算联合区域
	VOID  ClacUniteRect(CRect & UpdateRect,int nStartIndex,int nEndIndex);
	//获取选中文本
	VOID GetSelectedText(CString & csWindowText,int nSelectIndex);

protected:
	DECLARE_MESSAGE_MAP()
public:
	//绘制界面
	afx_msg void OnPaint();
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//窗体移动
	afx_msg void OnMove(int x, int y);
	//绘制背景
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//下页按钮
	VOID OnBnClickSoundPageDown();
    //上页按钮
	VOID OnBnClickSoundPageUp();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


