// physics.h: interface for the physics class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(PHYSICS_H)
#define PHYSICS_H

#pragma once

#include "3dmath.h"
#include ".\ballrender.h"


CONST INT NUMWALL=14;
CONST INT NUMPING=12;
CONST INT NUMHOLE=6;
CONST int NUMINFO=95;


#define BALL_NUM 16            //球
class physics  
{	
public:
	physics();
	virtual ~physics();
	struct collision
	{
		Vector3 Pn; //冲量
		float Pt;  //摩擦冲量
		Vector3 tangent_vel; //摩擦速度
		float tangent_speed;  //摩擦速度大小
		float  c ;//常量因子(Ca+Cb)
		float Vn; //相对速度
		float Vn2; //撞击后的相对速度
		Vector3 _N;  //碰撞垂直方向
		Vector3  Ra;  //撞击点到重心向量(刚体a)
		Vector3  Va; //撞击点的速度(刚体a)
		float  Ca;  //常量因子(刚体a)
		Vector3  Rb;  //撞击点到重心向量(刚体b)
		Vector3  Vb; //撞击后撞击点的速度(刚体B)
		float  Cb;  //常量因子(刚体B)
	};
	struct cline
	{
		Vector3  starpnt ;
		Vector3  endpnt ;
		Vector3  Nor    ;
		float d 	;
	};
	struct cPing
	{
		Vector3  pnt ;
		float R ;
	};

	void Allcollision(CBallRender*p_BALL);
	void collision_hole(CBallRender*p_BALL); 

	BOOL process_collision(CBallRender* Boxa,CBallRender* Boxb,Vector3 collpos ,Vector3 colN ,float Extra ) ;
	BOOL process_collision_wall(CBallRender* Boxa,Vector3 collpos ,Vector3 colN ,float Extra ) ;

	void collision_wall(CBallRender*p_BALL);
	void SetOffset(int x,int y)
	{
		m_offsetx = x;
		m_offsety = y;
	}

	POINT	m_ptClientMouse;	
	POINT	m_ptClientMid;
	cline   WALL[NUMWALL];
	cPing   Ping[NUMPING];
	cPing   Hole[NUMHOLE]; //门洞

	float  Elasticity_BallToBall ;     //球与球之间的反弹系数
	float  Elasticity_BallToWall;     //墙对球的反弹系数

	float  Per_Friction_BallToBall ;	 //球对球 瞬间撞击的摩擦系数 
	float  Per_Friction_BallToWall ;    //墙对球 瞬间撞击的摩擦系数 
public:
//	HEFFECT  snd[4];
	//hgeParticleSystem    *par;//一个测试离子
	void     Init(void *pMainWnd);
    bool     Exit();
	void	 InitOneOver(){m_nOverNum =0;m_wFistHitBall =17;m_bHitWithWall=false;};

public:
	BYTE     m_wOverBall[BALL_NUM];                //这杆打进的球（按进洞顺序）
	int      m_nOverNum;                           //进球数  
	BYTE     m_wFistHitBall;                       //第一个碰到的球
	bool     m_bHitWithWall;                       //是否有球碰墙
	void    *m_pMainWnd;
	int      m_offsetx;
	int      m_offsety;
	const static float HoleInfo[NUMINFO];

};

#endif 