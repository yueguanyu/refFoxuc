#pragma once

//顶层透明窗口
class CWindowTip : public CVirtualWindow
{
	//变量定义
public:
	CPoint									m_ptBenchmark;						//基准位置
	CSize									m_sizeStage;						//舞台大小
	BOOL									m_bKeepOut;							//设置遮挡	
	int										m_nKeepOutAlpha;					//遮罩渐隐读

	CD3DTexture*						m_pImageAnimal[ANIMAL_MAX];			//动物按钮图片
	BOOL									m_bAnimalFlicker[ANIMAL_MAX];		//动物闪动
	CPoint									m_ptAnimalSite[ANIMAL_MAX];			//动物位置


	//图片变量
public:
	//
	CD3DTexture						m_ImageKeepOut;						//遮罩

	//构造函数
public:
	CWindowTip(void);
	~CWindowTip(void);

	//系统事件
protected:
	//动画消息
	virtual VOID OnWindowMovie();
	//创建消息
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);

	//重载函数
protected:
	//鼠标事件
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos);
	//按钮事件
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos);
	//绘画窗口
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos);

	//设置函数
public:
	//设置基准位置
	VOID SetBenchmark(CPoint ptBenchmark) { m_ptBenchmark = ptBenchmark; }
	//设置舞台大小
	VOID SetStage(CSize	sizeStage) { m_sizeStage = sizeStage; }
	//设置遮挡
	VOID SetKeepOut(BOOL bKeepOut) { m_bKeepOut = bKeepOut; }
	//设置图片
	VOID SetImageAnimal(CD3DTexture* pImageAnimal[ANIMAL_MAX]) { CopyMemory(m_pImageAnimal, pImageAnimal, sizeof(m_pImageAnimal)); }
	//设置图片位置
	VOID SetAnimalSite(CPoint ptAnimalSite[ANIMAL_MAX]) { CopyMemory(m_ptAnimalSite, ptAnimalSite, sizeof(m_ptAnimalSite)); }
	//设置闪动
	VOID SetAnimalFlicker( int nAnimalIndex ) { if(nAnimalIndex < ANIMAL_MAX) { m_bAnimalFlicker[nAnimalIndex] = TRUE; } }
	//关闭闪动
	VOID CloseFlicker() { ZeroMemory(m_bAnimalFlicker, sizeof(m_bAnimalFlicker)); }
};