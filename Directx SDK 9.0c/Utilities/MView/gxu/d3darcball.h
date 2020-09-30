#pragma once

#ifndef D3DARCBALL_H_
#define D3DARCBALL_H_

/*//////////////////////////////////////////////////////////////////////////////
//
// File: d3darcball.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// Description:
//    This include file contains a simple version of Shoemake's arcball code 
// from GGems IV.
//
//
//////////////////////////////////////////////////////////////////////////////*/


class GXArcBall { // doesn't maintain previous matrix, just current...
public:
	GXArcBall();

	GXArcBall(const int& x, const int& y);
	GXArcBall(D3DXVECTOR3 &center);

	void Reset(); // qDown_ become identity...

	void SetWindow(int w, int h, float r=0.9); // ball info - sets center!

	void BeginDrag(const int& x, const int& y); // start dragging
	void EndDrag();   // button up - qDown_ is now qNow_

	void Mouse(const int& x, const int& y); // right from windows...


	void GetMat(D3DXMATRIX *mat); // gets current rotation matrix...


	void Update();

protected:

	// this function turns a screen point into a normalized
	// point on the sphere...

	D3DXVECTOR3 ScreenToVector(const int& x, const int& y);


	D3DXVECTOR3 m_vCenter; // center of arcball - on screen
	
	D3DXVECTOR3 m_vDown; // button down quaternion
	D3DXVECTOR3 m_vCur;  // current quaternion

	D3DXQUATERNION m_qDown; // quaternion before button down
	D3DXQUATERNION m_qNow;  // composite quaternion for current drag

	int m_iWidth, m_iHeight; // window dimensions
	float m_fRadius;       // ball radius - in screen space

	bool m_bDrag;          // true if during drag

};

// add some helper functions - probably should be in quaternion
// class or something...

// takes two points on unit sphere an angle THETA apart, returns
// quaternion that represents a rotation around cross product by
// 2 * theta

D3DXQUATERNION QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo);

// takes a quaternion and returns 2 points on unit sphere that represent quat

//void QuatToBallPoints(const GXQuaternion &quat, GXVector3 &vFrom, GXVector3 &vTo);

#endif
