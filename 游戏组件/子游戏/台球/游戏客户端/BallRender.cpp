#include "StdAfx.h"
#include ".\ballrender.h"
//
// float  Sta_friction = 0.003f; //球的滑动摩擦系数
// float  Rou_friction = 0.00022f; //球的滚动摩擦系数
// float  Xuan_friction = 0.00014f;  //球的旋切摩擦系数
//
// float  Sta_friction = 0.00299f; //球的滑动摩擦系数
// float  Rou_friction = 0.00030f; //球的滚动摩擦系数
// float  Xuan_friction = 0.0002f;  //球的旋切摩擦系数
 float  dt=1.0f ;
 float  Sta_friction = 0.003f; //球的滑动摩擦系数
 float  Rou_friction = 0.00022f; //球的滚动摩擦系数
 float  Xuan_friction = 0.00014f;  //球的旋切摩擦系数
// float  Sta_friction = 1.003f; //球的滑动摩擦系数
// float  Rou_friction = 0.00025f; //球的滚动摩擦系数
// float  Xuan_friction = 0.00019f;  //球的旋切摩擦系数




Vector3 light;
Vector3  Pos[128][128];
BYTE mapX[128][128];
float diff [128][128];
Vector3 ver[32][32];
short FJC [128][128];

HTEXTURE        texLight=0;
HTEXTURE        texShadow=0;
HTEXTURE        texBall=0;

CHgeSprite*	    pSpriteShadow=NULL;
CHgeSprite*	    pSprite=NULL;
CHgeSprite*     pSpriteLight=NULL;

CBallRender::CBallRender(void)
{	
	Iz =M*banjin*banjin*2/5;
	SENSTA_FRICTION = Sta_friction * 1.f + Sta_friction * M / Iz * banjin * banjin;  //滑动摩擦最小检测阀值
	SENROU_FRICTION = Rou_friction * 1.f;  //滚动摩擦最小检测阀值

	BALL_State=4; 	//球静止
	BALL_bEnable=true ;  //	是否有效
	BALL_V=Vector3(0);
	BALL_Vang=Vector3(0);
	BALL_Orientation=matrix3_identity();	
	BALL_bIsRedraw =false;	
	BALL_bIsInit =false;
	tex = NULL;
	offsetx = 0;
	offsety = 0;
	banJin = 16;
	zhiJin = 32;
	binhole = false;
	pSpriteLight = NULL;
	m_pIGameFrameView = NULL;
}

CBallRender::~CBallRender(void)
{	
//	m_GraphHelper->Free();
}

//应用力
void CBallRender::ApplyForce(Vector3 force,Vector3 colpos)
{
	float  dt=1.0f ;
	apply_impulse (force* dt, colpos);
}

void CBallRender::apply_impulse(Vector3 impulse ,Vector3 colpos)//摩擦力
{
	Vector3 Ra , vatmp ;

	BALL_V+= (impulse/ M);//线速度
	if(impulse.Z!=0.0&&abs(impulse.Z)>1)
	{
		BALL_V.X /= impulse.Z;
		BALL_V.Y /= impulse.Z;
	}
	BALL_V.Z = 0 ; //不能够向下

	Ra= colpos- Ball_Pos;

	vatmp = cross(Ra, impulse) / Iz	;
	BALL_Vang+=vatmp;//角速度

	static Vector3 temp = BALL_V;
	TRACE("BALL_Vang.X=%f,BALL_Vang.Y=%f,BALL_Vang.Z=%f",BALL_Vang.X,BALL_Vang.Y,BALL_Vang.Z);
	TRACE("BALL_V.X=%f,BALL_V.Y=%f,BALL_V.Z=%f",BALL_V.X,BALL_V.Y,BALL_V.Z);
	TRACE("增量：x = %f,y = %f,z = %f",BALL_V.X - temp.X,BALL_V.Y - temp.Y,BALL_V.Z-temp.Z);
	temp = BALL_V;
}

void CBallRender::Reset(int _ballNo,Vector3 _ballPos,bool _enable)
{
	Ball_Pos =_ballPos;
	BallNo =_ballNo;
	BALL_bEnable =_enable;
}

void CBallRender::Init(int _ballNo,Vector3 _ballPos)
{
	HGE* pHge = NULL;
	if (NULL != m_pIGameFrameView)
		pHge = m_pIGameFrameView->GetHge();
	
//	ASSERT(NULL != pHge);
	if (NULL == pHge) return;

	Ball_Pos =_ballPos;//Vector3(rand()%700+20,rand()%500+20,0);	
	BallNo =_ballNo;	
	if (tex)
	{
		BALL_bEnable = true;
		return;
	}

	//tex = m_GraphHelper->Texture_Create(zhiJin,zhiJin);
	tex =  pHge->Texture_Load("TaiQiu\\pic\\mask1.bmp");
	Vector3  ran = NormaliseVector(Vector3(ranged_random(.1f,2),ranged_random(.1f,2),ranged_random(.1f,2)));
	BALL_Orientation = rotation_matrix (ranged_random(1,800), ran);	// 设置每个球的随机方位

	HTEXTURE  texMask;
	texMask = pHge->Texture_Load(("TaiQiu\\pic\\mask1.bmp"));
	if (texBall==0) 
	{	
		texLight = pHge->Texture_Load(("TaiQiu\\pic\\TEX_Specular25.bmp"));	
		texBall = pHge->Texture_Load(("TaiQiu\\pic\\TEX_BALL.bmp"));
		texShadow = pHge->Texture_Create(zhiJin,zhiJin);
		//pSpriteShadow2 = new CHgeSprite (_T("TaiQiu\\pic\\Ball_Shadow.png"));

		pSpriteShadow = new CHgeSprite(texShadow,0,0,zhiJin,zhiJin);
		pSpriteShadow->SetHotSpot(16,16);
		pSpriteShadow->SetColor(0xff000000);

		DWORD *ptex = pHge->Texture_Lock(tex,false);
		DWORD *ptexmask = pHge->Texture_Lock(texMask);
		DWORD *ptexlight = pHge->Texture_Lock(texLight,false);
		DWORD *ptexShadow = pHge->Texture_Lock(texShadow,false);
		for(int i=0;i<zhiJin;i++)
		{
			for(int j=0;j<zhiJin;j++)
			{
				DWORD col = ptexmask[i*zhiJin+j];
				BYTE *p = (BYTE*)&col;
				BYTE c = ((int)*(p)+6*(int)*(p+1)+3*(int)*(p+2))/10;	//设置透明度		
				col = ptex[i*zhiJin+j];
				ptex[i*zhiJin+j] = SETA(ptex[i*zhiJin+j],c);
				//ptexlight[i*zhiJin+j] = SETA(ptexlight[i*zhiJin+j],c);
				col = ptex[i*zhiJin+j];
				//ptex[i*zhiJin+j] = SETG(ptex[i*zhiJin+j],0XFF);//设置绿色？
					//ptex[i*zhiJin+j] = SETR(ptex[i*zhiJin+j],100);//设置绿色？
					//ptex[i*zhiJin+j] = SETB(ptex[i*zhiJin+j],165);//设置绿色？
				p =(BYTE*)&ptexShadow[i*zhiJin+j];
				*(p+3)=c/2;
			}
		}
	/*	for(int i=0;i<25;i++)
		{
			for(int j=0;j<25;j++)
			{
				DWORD col = ptexlight[i*25+j];
				BYTE c = 0;
				if(col==0xff000000)
					c = 0;
				else c = 0xff;
				ptexlight[i*25+j] = SETA(ptexlight[i*25+j],c);
			}
		}*/
		pHge->Texture_Unlock(tex);
		pHge->Texture_Unlock(texMask);
		pHge->Texture_Unlock(texLight);
		pHge->Texture_Unlock(texShadow);	

		light = Vector3(800/2,520/2,-500);

		float X,Y,Z;
		//法线预处理	
		for(int i=0;i<zhiJin;i++)
		{
			for(int j=0;j<zhiJin;j++)
			{
				mapX[i][j] =0;
				X =i-banJin+0.5;
				Y =j-banJin+0.5;
				ver[i][j].X =X;//法线
				ver[i][j].Y =Y;
				Z = banJin*banJin-(X*X + Y*Y);		
				if (Z>=0) 
				{
					ver[i][j].Z = -sqrtf(Z);
					mapX[i][j] = 1;
				}
			}
		}
	}

	DWORD *ptex = pHge->Texture_Lock(tex,false);
	DWORD *ptexmask = pHge->Texture_Lock(texMask);		
	for(int i=0;i<zhiJin;i++)
	{
		for(int j=0;j<zhiJin;j++)
		{
			DWORD col =ptexmask[i*zhiJin+j];
			BYTE*p =(BYTE*)&col;
			BYTE c=((int)*(p)+6*(int)*(p+1)+3*(int)*(p+2))/10;
		/*	DWORD temp = GETA(col);		
			if(col==0xff000000)
				temp = 0;
			else temp = c;*/
			col =ptex[i*zhiJin+j];			
			ptex[i*zhiJin+j] =SETA(ptex[i*zhiJin+j],c);			
			col =ptex[i*zhiJin+j];
		//	ptex[i*zhiJin+j] =SETG(ptex[i*zhiJin+j],0xFF);		
			//	ptex[i*zhiJin+j] = SETR(ptex[i*zhiJin+j],100);//设置绿色？
			//	ptex[i*zhiJin+j] = SETB(ptex[i*zhiJin+j],165);//设置绿色？
		}
	}
	pHge->Texture_Unlock(tex);
	pHge->Texture_Unlock(texMask);
	pHge->Texture_Free(texMask);

	pSprite = new CHgeSprite(tex,0,0,zhiJin,zhiJin);
	pSprite->SetHotSpot(banJin,banJin);	
	pSprite->SetZ(0.6f);

	pSpriteLight = new CHgeSprite(texLight,0,0,25,25);
	pSpriteLight->SetHotSpot(banjin,banjin);	
	pSpriteLight->SetZ(0.6f);

	//int height = m_GraphHelper->Texture_GetHeight(tex,true);
	//CString str; 
	//str.Format("zhiJin=%d,texHeight=%d,spriteHeight=%d,spriteWidth=%d",zhiJin,height,pSprite->GetHeight(),pSprite->GetHeight());
	//AfxMessageBox(str);
}


void CBallRender::Moveball()
{
	if	(BALL_bEnable==false) return;

	if (BALL_V.X==0&&BALL_V.Y==0&&BALL_V.Z==0
		&&BALL_Vang.X==0&&BALL_Vang.Y==0&&BALL_Vang.Z==0) 
	{
		return ;
	}
	else
	{
		BALL_bIsRedraw =true;
	}

	Vector3 tool_ver, tra_ver, Normaliz;
	tra_ver = cross(BALL_Vang, Vector3(0, 0, banjin));
	tool_ver = (BALL_V+tra_ver) ;	
	
	if ((tra_ver.mag() < SENROU_FRICTION) && (Vector3(BALL_V.X, BALL_V.Y).mag() < SENROU_FRICTION)) 
	{ 
		BALL_State = 3;   //水平方向静止 灰
		BALL_V = Vector3(0, 0, 0);
		BALL_Vang.X = 0;	 BALL_Vang.Y = 0 ;
	}  
	else if (tool_ver.mag() < SENSTA_FRICTION)
	{ 
		TRACE("滚动摩擦");
		BALL_State = 2;   //滚动摩擦 绿
		Normaliz = NormaliseVector(BALL_V);
		apply_impulse (Normaliz*(-M * Rou_friction), Ball_Pos);
		BALL_Vang.X = BALL_V.Y / banjin;
		BALL_Vang.Y = -BALL_V.X / banjin;
	}
	else
	{ 
		TRACE("滑动摩擦");
		BALL_State = 1 ;//滑动摩擦 红
		Normaliz = NormaliseVector(tool_ver);	
		apply_impulse ( Normaliz*(-M * Sta_friction ), (Ball_Pos+Vector3(0, 0, banjin))	);
	}

	//旋切摩擦
	if (fabs(BALL_Vang.Z) < Xuan_friction)
		BALL_Vang.Z = 0 ;
	else
	{
		if (BALL_Vang.Z > 0)
			BALL_Vang.Z  -=  Xuan_friction ;
		else
			BALL_Vang.Z +=  Xuan_friction;
	}

	if ((BALL_State == 3) && (BALL_Vang.Z == 0))  BALL_State = 4; //完全静止 黑

	Ball_Pos+= (BALL_V* dt);
	Matrix3 Mtmp ;
	 Vector3 & rot = BALL_Vang;

	Mtmp=dt * Matrix3(      0,  rot[2], -rot[1],
		              -rot[2],       0,  rot[0],
		               rot[1], -rot[0],       0 ) * BALL_Orientation;
	
	BALL_Orientation += Mtmp;
	BALL_Orientation.orthonormalise();
}

void CBallRender::Move()
{		
	Moveball();		
}
void CBallRender::UpdateBallTex()
{
	HGE* pHge = NULL;
	if (NULL != m_pIGameFrameView)
		pHge = m_pIGameFrameView->GetHge();

	ASSERT(NULL != pHge);
	if (NULL == pHge) return; 

	int i,j;
	float IncX , IncY,lng;
	float X , Y , XYmag2;


	IncX = Ball_Pos.X - (INT)Ball_Pos.X;
	IncY = Ball_Pos.Y  - (INT)Ball_Pos.Y;
	if(IncX!=0||IncY!=0)
	{
		int i=0;
		i++;
	}

	//    法线预处理
	for (i=0;i<33;i++)
	{
		for (j=0;j<33;j++)
		{


			mapX[i][j] = 0 ;
			X =  i - IncX - (banJin-0.5);
			Y = j - IncY - (banJin-0.5);
			Pos[i][j].X =X ;
			Pos[i][j].Y =Y ;

			XYmag2=(X*X + Y*Y); 
			lng = banJin*banJin - XYmag2 ;
			if (lng > 0	)
			{
				Pos[i][j].Z = -sqrtf(lng)	;
				mapX[i][j] = 1 ;
			}


			//    边缘反锯齿
			lng = banJin - sqrtf(XYmag2) ;
			if (fabs(lng) < 0.4714f)
				CALEFJC (X, Y, i, j );
		}
	}
	//'计算漫反射
	Vector3 lightdir;
	lightdir = light-Ball_Pos;
	lightdir.normalise();

	for (i=0;i<zhiJin+1;i++)
	{
		for (j=0;j<zhiJin+1;j++)
		{

			if (mapX[i][j] != 0)
			{
				diff[i][j] = dot(Pos[i][j], lightdir) / banJin	;
				if (diff[i][j] < 0) diff[i][j] = 0 ;
				diff[i][j] = 0.3 + 0.7 * diff[i][j]	;
			}

		}
	}

	Matrix3 Mtmp ;
	Mtmp= transpose (BALL_Orientation);  //反矩阵

	Vector3 pos;
	DWORD *ptex = pHge->Texture_Lock(tex,false);
	DWORD *ptexBall = pHge->Texture_Lock(texBall);  
	
	int tempNo = BallNo-14;
	int offx =BallNo%4;
	int offy =BallNo/4;
	int mPitch =64*16;
	DWORD * inoff =ptexBall+offy*64*4*64+offx*64;	
	float tempNum = 31.5;
	int ballDraw_X, ballDraw_Y;
	ballDraw_X = (INT)Ball_Pos.X - banJin;
	ballDraw_Y = (INT)Ball_Pos.Y - banJin; // '左下角坐标
	for(int i=0;i<zhiJin;i++)
	{
		for(int j=0;j<zhiJin;j++)
		{ 
			//转换点
			pos= Mtmp *Pos[i][j];			
			if (mapX[i][j]!=0) 
			{
				if (pos.Z >0)
				{	
					BYTE ca =GETA(ptex[j*zhiJin+i]);
					GetFitColor((tempNum-pos.X),(tempNum+pos.Y),inoff,mPitch,&ptex[j*zhiJin+i]);
					ptex[j*zhiJin+i] =SETA(ptex[j*zhiJin+i],ca);
				}
				else
				{
					BYTE ca =GETA(ptex[j*zhiJin+i]);					
					GetFitColor((tempNum-pos.X),(tempNum-pos.Y),inoff,mPitch,&ptex[j*zhiJin+i]);
					ptex[j*zhiJin+i] =SETA(ptex[j*zhiJin+i],ca);
				}

				MIX_multself1((BYTE*)&ptex[j*zhiJin+i],(INT)(diff[i][j]*255));  //  '漫反射混合

				
				BYTE*P= ((BYTE*)&ptex[j*zhiJin+i]);

				if (mapX[i][j] == 2)
				{
					//与地面混合
					MIX_APLC((BYTE*)&ptex[j*zhiJin+i],P, FJC[i][j]);
				}
			}			
		}
	}
	pHge->Texture_Unlock(tex);
	pHge->Texture_Unlock(texBall);
}


void CBallRender::Render()
{	
	if (BALL_bEnable==false) {
		return ;
	}
	if (BALL_bIsInit==false) 
	{
		UpdateBallTex();
		BALL_bIsInit =true;
	}
	else if (BALL_bIsRedraw) {
		UpdateBallTex();
		BALL_bIsRedraw =false;
	}
	float PntX , PntY;
	float H, dx, dy;
	H = -light.Z;
	dx = Ball_Pos.X - light.X;
	dy = Ball_Pos.Y - light.Y;
	PntX = dx * (5 + H) / H + light.X;
	PntY = dy * (5 + H) / H + light.Y;
	
	//画阴影
	pSpriteShadow->SetBlendMode(BLEND_DEFAULT);
	pSpriteShadow->SetTexture(texShadow);
	//pSpriteShadow->Render(PntX+offsetx,PntY+offsety);
	pSpriteShadow->SetColor(ARGB(255,255,255,255));
	pSpriteShadow->RenderEx(PntX+offsetx,PntY+offsety,0,0.9,0.9);
	pSpriteShadow->SetColor(ARGB(125,0,0,0));
	pSpriteShadow->RenderEx(Ball_Pos.X+offsetx,Ball_Pos.Y+offsety,0,0.88,0.88);
 

	//画球
	pSprite->SetBlendMode(BLEND_DEFAULT);
	pSprite->SetTexture(tex);
//	pSprite->Render(Ball_Pos.X+offsetx,Ball_Pos.Y+offsety);
	pSprite->RenderEx(Ball_Pos.X+offsetx,Ball_Pos.Y+offsety,0,0.8,0.8);
	//
	////画高亮
	//pSpriteLight->SetTexture(texLight);
	//pSpriteLight->SetBlendMode(BLEND_MODE_COLORMUL );	
	//pSpriteLight->Render(Ball_Pos.X+offsetx,Ball_Pos.Y+offsety);
	 

}
 void CBallRender::RenderEx()
{	
	if (BALL_bEnable==false) {
		return ;
	}
	if (BALL_bIsInit==false) 
	{
		UpdateBallTex();
		BALL_bIsInit =true;
	}
	else if (BALL_bIsRedraw) {
		UpdateBallTex();
		BALL_bIsRedraw =false;
	}
	float PntX , PntY;
	float H, dx, dy;
	H = -light.Z;
	dx = Ball_Pos.X - light.X;
	dy = Ball_Pos.Y - light.Y;
	PntX = dx * (banJin + H) / H + light.X;
	PntY = dy * (banJin + H) / H + light.Y;
	////
	////画阴影
	//pSpriteShadow->SetBlendMode(BLEND_MODE_DEFAULT);
	//pSpriteShadow->SetTexture(texShadow);
	//pSpriteShadow->Render(PntX+offsetx,PntY+offsety);
	//画球
	pSprite->SetBlendMode(BLEND_DEFAULT);
	pSprite->SetTexture(tex);
	pSprite->RenderEx(Ball_Pos.X+offsetx,Ball_Pos.Y+offsety,3.1415926,0.8,0.8);
////	画高亮
	//pSprite->SetTexture(texLight);
	//pSprite->SetBlendMode(BLEND_MODE_COLORMUL );	
	//pSprite->Render(Ball_Pos.X+offsetx,Ball_Pos.Y+offsety);

}

bool  CBallRender:: IsStop()
{
	return BALL_State == 4;
}
bool  CBallRender:: MouseMoveIn(float x,float y)
{
	x = x - offsetx;
	y = y - offsety;
	float xLength = x - this->Ball_Pos.X;
	float yLength = y - Ball_Pos.Y;
	float length = sqrt(xLength*xLength + yLength*yLength);
	return length<banjin-1;
}

 void  CBallRender::CALEFJC	(FLOAT Xf, FLOAT Yf ,INT Ni ,INT Nj)
{
	int i,j;
	Vector3 Toolver(0,0,0);
	FLOAT Xs, Ys;
	int Numin=0;// '落在里面

	for (i=0;i<3;i++)
	{
		for (j=0;j<3;j++)
		{

			Xs = Xf + (FLOAT)i *0.33333f  - 0.33333f	;
			Ys = Yf + (FLOAT)j * 0.33333f - 0.33333f	;
			if (sqrtf(Xs* Xs + Ys*Ys) < banJin)
			{
				Toolver += CALEPOS(Xs, Ys);
				Numin = Numin + 1 ;
			}

		}
	}

	if (Numin > 0)
	{
		Toolver.normalise();
		Toolver *= banJin ;
		Pos[Ni][Nj] = Toolver ;
		mapX[Ni][Nj] = 2	;
		FJC[Ni][Nj] = Numin;
	}

}

void CBallRender::SetView(IUnknownEx* pIUnknownEx)
{
	 if (NULL != pIUnknownEx)
	 {
		 m_pIGameFrameView = /*GET_OBJECTPTR_INTERFACE*/QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITQView);
		 ASSERT(m_pIGameFrameView != NULL);
	 }
}