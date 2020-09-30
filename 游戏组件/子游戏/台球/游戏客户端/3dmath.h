// 数学库
#ifndef _3DMATH_H_
#define _3DMATH_H_

#pragma warning (disable: 4244)

#include <math.h>

#include <stdio.h>
#include <assert.h>

typedef unsigned int uint;
typedef DWORD               ZQ_COLOR;


const float TWO_PI= 6.28318530717958647692528676655901f ;
const float  PI =3.1415926535897932384626433832795f	;
const float  PI_DIV2=1.5707963267948966192313216916395f ;

#define PI_DIV_180 0.0174532925199432957692369076848861
#define _180_DIV_PI 57.2957795130823208767981548141052
inline float deg_to_rad(float deg) {return (float) (deg * PI_DIV_180);}
inline float rad_to_deg(float rad) {return (float) (rad * _180_DIV_PI);}


inline float sin_deg(float deg) {return (float) sin(deg_to_rad(deg));}
inline float cos_deg(float deg) {return (float) cos(deg_to_rad(deg));}
inline float asin_deg(float x) {return rad_to_deg(asin((float) x));}
inline float acos_deg(float x) {return rad_to_deg(acos((float) x));}
inline float tan_deg(float deg) {return (float) tan(deg_to_rad(deg));}
inline float atan2_deg(float x, float y) {return rad_to_deg((float) atan2(x, y));}
inline float atan_deg(float x) {return rad_to_deg((float) atan(x));}

template<class T>	_inline const T&	Min	(const T& a, const T& b)	{ return a > b ? b : a;	}
template<class T>	_inline const T&	Max	(const T& a, const T& b)	{ return a < b ? b : a;	}

inline float ranged_random(float v1,float v2)
{
	return v1 + (v2-v1)*((float)rand())/((float)RAND_MAX);
}


class Vector3;

//! A 3x3 matrix
class Matrix3
{
public:
	inline Matrix3(float v11, float v21, float v31, // first column
		float v12, float v22, float v32, // 2nd column
		float v13, float v23, float v33  );
	inline Matrix3(const Vector3 & v1, // first column
		const Vector3 & v2, 
		const Vector3 & v3);
	inline Matrix3();
	explicit inline Matrix3(float val);
	inline ~Matrix3();

	inline void set_to(float val);
	inline void orthonormalise();

	inline bool sensible() const; // indicates if all is OK

	float & operator()(const uint i, const uint j) {return data[i + 3*j];}
	const float & operator()(const uint i, const uint j) const {return data[i + 3*j];}

	//! returns pointer to the first element
	inline const float * get_data() {return data;} 
	inline const float * get_data() const {return data;} 
	//! pointer to value returned from get_data
	inline void set_data(const float * d); 

	/// Returns a column - no range checking!
	inline Vector3 get_col(uint i) const;

	// sets a column
	inline void set_col(uint i, const Vector3 & col);

	// operators
	inline Matrix3 & operator+=(const Matrix3 & rhs);
	inline Matrix3 & operator-=(const Matrix3 & rhs);

	inline Matrix3 & operator*=(const float rhs);
	inline Matrix3 & operator/=(const float rhs);

	inline Matrix3 operator+(const Matrix3 & rhs) const;
	inline Matrix3 operator-(const Matrix3 & rhs) const;
	friend Matrix3 operator*(const Matrix3 & lhs, const float rhs);
	friend Matrix3 operator*(const float lhs, const Matrix3 & rhs);
	friend Matrix3 operator*(const Matrix3 & lhs, const Matrix3 & rhs);
	friend Matrix3 transpose(const Matrix3 & rhs);
	friend float trace(const Matrix3 & rhs);  
	friend Vector3 operator*(const Matrix3 & lhs, const Vector3 & rhs);

	inline void show(const char * str = "") const;

private:
	float data[9];
};

//############## Vector3 ################
//! A 3x1 matrix (i.e. a vector)
class Vector3
{
public:
	inline Vector3() {};
	explicit inline Vector3(float val);
	inline Vector3(float x, float y, float z=0) 
	{data[0] = x; data[1] = y; data[2] = z;}
	inline ~Vector3() {};

	inline void set_to(float val); //!< Set all values to val

	inline bool sensible() const; // indicates if all is OK

	float & operator[](uint i) {return data[i];} //!< unchecked access
	const float & operator[](uint i) const {return data[i];}
	float & operator()(uint i) {return data[i];}
	const float & operator()(uint i) const {return data[i];}

	//! returns pointer to the first element
	inline const float * get_data() {return data;} 
	inline const float * get_data() const {return data;} 
	//! pointer to value returned from get_data
	inline void set_data(const float * d);

	//! calculate the square of the magnitude
	inline float mag2() const {
		return (float) (data[0]*data[0]+data[1]*data[1]+data[2]*data[2]);}
	//! calculate the magnitude
	inline float mag() const {return (float) sqrt(mag2());}
	//! Normalise, and return the result
	inline Vector3 & normalise();



	// operators
	//v1+=v2;
	//v1-=v2;
	inline Vector3 & operator+=(const Vector3 & rhs);
	inline Vector3 & operator-=(const Vector3 & rhs);

	//v1*=u;
	//v1/=u;
	inline Vector3 & operator*=(const float rhs);
	inline Vector3 & operator/=(const float rhs);

	//v1=-v2;
	inline Vector3 operator-() const {return Vector3(-data[0], -data[1], -data[2]);}

	//v3=v1+v2;
	//v3=v1-v2;
	inline Vector3 operator+(const Vector3 & rhs) const;
	inline Vector3 operator-(const Vector3 & rhs) const;

	//v3=v1*v2;
	//v3=v1*u;
	//v3=u*v1;
	//v3=v1/u;
	//u=dot(v1*v2);
	//v3=cross(v1*v2);
	inline float operator*( const Vector3 & rhs);
	friend Vector3 operator*(const Vector3 & lhs, const float rhs);
	friend Vector3 operator*(const float lhs, const Vector3 & rhs);
	friend Vector3 operator/(const Vector3 & lhs, const float rhs);
	friend float dot(const Vector3 & lhs, const Vector3 & rhs);
	friend Vector3 cross(const Vector3 & lhs, const Vector3 & rhs);

	//v3=M*v1;
	//M=rotation_matrix(u,v1);
	friend Vector3 operator*(const Matrix3 & lhs, const Vector3 & rhs);
	friend Matrix3 rotation_matrix(float ang, const Vector3 & dir);

	inline void show(const char * str = "") const;

	// private:
	union {
		struct {float X,Y,Z;};
		// 	struct {float r,g,b;};
		float  data[3];
	};

};

// global operators
//M2=M1*u;
//M2=u*M1;
//M3=M1*M2;
inline  float User_Atn(float X,float Y );
inline Matrix3 operator*(const Matrix3 & lhs, const float rhs);
inline Matrix3 operator*(const float lhs, const Matrix3 & rhs) {return rhs * lhs;}
inline Matrix3 operator*(const Matrix3 & lhs, const Matrix3 & rhs);

inline float operator*(const Vector3 & lhs, const Vector3 & rhs);	 ///
inline  Vector3 operator*(const Vector3 & lhs, const float rhs);
inline Vector3 operator/(const Vector3 & lhs, const float rhs);
inline float dot(const Vector3 & lhs, const Vector3 & rhs);
inline Vector3 cross(const Vector3 & lhs, const Vector3 & rhs);
inline Vector3 operator*(const float lhs, const Vector3 & rhs) {return rhs * lhs;}
inline Matrix3 transpose(const Matrix3 & rhs);

inline float trace(const Matrix3 & rhs);

// matrix * vector
inline Vector3 operator*(const Matrix3 & lhs, const Vector3 & rhs);

// Some useful rotation Matrix3's
// alpha returns a matrix that wil rotate alpha around the x axis (etc)
inline Matrix3 m3alpha(float alpha);
inline Matrix3 m3beta(float beta);
inline Matrix3 m3gamma(float gamma);

inline Vector3 NormaliseVector(const Vector3 & lhs);

//归一化
inline const Matrix3 & matrix3_identity()
{	
	static const Matrix3 result(1, 0, 0,
		0, 1, 0,
		0, 0, 1);
	return result;
}

inline Matrix3 rotation_matrix(float ang, const Vector3 & dir);

// converts a rotation matrix into a rotation of degrees about axis
inline void calculate_rot_from_matrix(const Matrix3 & matrix, Vector3 & axis, float & degrees);


inline Matrix3::Matrix3() {};
inline Matrix3::~Matrix3() {};

inline void Matrix3::set_to(float val)
{
	data[0] = val;
	data[1] = val;
	data[2] = val;
	data[3] = val;
	data[4] = val;
	data[5] = val;
	data[6] = val;
	data[7] = val;
	data[8] = val;
}

inline Matrix3::Matrix3(float val) {set_to(val);}

inline Matrix3::Matrix3(float v11, float v21, float v31, // first column
						float v12, float v22, float v32, // 2nd column
						float v13, float v23, float v33  )
{
	data[0] = v11;
	data[1] = v21;
	data[2] = v31;

	data[3] = v12;
	data[4] = v22;
	data[5] = v32;

	data[6] = v13;
	data[7] = v23;
	data[8] = v33;
}
inline Matrix3::Matrix3(const Vector3 & v1, // first column
						const Vector3 & v2, 
						const Vector3 & v3)
{
	data[0] = v1[0];
	data[1] = v1[1];
	data[2] = v1[2];

	data[3] = v2[0];
	data[4] = v2[1];
	data[5] = v2[2];

	data[6] = v3[0];
	data[7] = v3[1];
	data[8] = v3[2];
}

inline void Matrix3::set_data(const float * d)
{
	memcpy(data, d, 9*sizeof(float));
}

inline Vector3 Matrix3::get_col(uint i) const
{
	const uint o = i*3; 
	return Vector3(data[o], data[o+1], data[o+2]);
}

inline void Matrix3::set_col(uint i, const Vector3 & col)
{
	const uint o = i*3; 
	data[o]   = col[0];
	data[o+1] = col[1];
	data[o+2] = col[2];
}

inline bool Matrix3::sensible() const
{
	for (unsigned i = 0 ; i < 9 ; ++i)
	{
		if (!((data[i] < 1e8) && (data[i] > -1e8)))
			return false;
	}
	return true;
}

inline void Matrix3::show(const char * str) const
{
	//  uint i, j;
	//  TRACE("%s Matrix3::this = 0x%x \n", str, (int) this);
	//  for (i = 0 ; i < 3 ; i++)
	//  {
	//    for (j = 0 ; j < 3 ; j++)
	//    {
	//      TRACE("%4f ", operator()(i, j));
	//    }
	//    TRACE("\n");
	//  }
}

inline Matrix3 & Matrix3::operator+=(const Matrix3 & rhs)
{
	for (uint i = 9 ; i-- != 0 ;)
		data[i] += rhs.data[i];
	return *this;
}

inline Matrix3 & Matrix3::operator-=(const Matrix3 & rhs)
{
	for (uint i = 9 ; i-- != 0 ;)
		data[i] -= rhs.data[i];
	return *this;
}

inline Matrix3 & Matrix3::operator*=(const float rhs)
{
	for (uint i = 9 ; i-- != 0 ;)
		data[i] *= rhs;
	return *this;
}

inline Matrix3 & Matrix3::operator/=(const float rhs)
{
	const float inv_rhs = 1.0f/rhs;
	for (uint i = 9 ; i-- != 0 ;)
		data[i] *= inv_rhs;
	return *this;
}

inline Matrix3 Matrix3::operator+(const Matrix3 & rhs) const
{
	return Matrix3(*this) += rhs;
}

inline Matrix3 Matrix3::operator-(const Matrix3 & rhs) const
{
	return Matrix3(*this) -= rhs;
}

// global operators

inline Matrix3 operator*(const Matrix3 & lhs, const float rhs)
{
	Matrix3 result;

	for (uint i = 9 ; i-- != 0 ; )
		result.data[i] = rhs * lhs.data[i];
	return result;
}

inline Matrix3 operator*(const Matrix3 & lhs, const Matrix3 & rhs)
{
	static Matrix3 out; // avoid ctor/dtor

	for (uint oj = 3 ; oj-- != 0 ;)
	{
		for (uint oi = 3 ; oi-- != 0 ;)
		{
			out(oi, oj) =
				lhs(oi, 0)*rhs(0, oj) +
				lhs(oi, 1)*rhs(1, oj) +
				lhs(oi, 2)*rhs(2, oj);
		}
	}
	return out;
}

//########################################################################
// 
//                       Vector3
//
//########################################################################

inline void Vector3::set_to(float val)
{
	data[0] = val;
	data[1] = val;
	data[2] = val;
}

inline Vector3::Vector3(float val) {set_to(val);}

inline Vector3 & Vector3::normalise()
{
	const float m2 = mag2();
	if (m2 > 0.0f)
	{
		const float inv_mag = 1.0f/sqrt(m2);
		data[0] = data[0] * inv_mag;
		data[1] = data[1] * inv_mag;
		data[2] = data[2] * inv_mag;
		return *this;
	}
	else
	{
		//   TRACE("magnitude = %f in normalise()\n", sqrt(m2));
		data[0] = 1.0f; 
		data[1] = data[2] = 0.0f;
		return *this;
	}
}
inline Vector3 NormaliseVector(const Vector3 & lhs)
{	 
	Vector3 result=lhs;
	result.normalise();
	return result ; 
}

inline void Vector3::set_data(const float * d)
{
	memcpy(data, d, 3*sizeof(float));  
}

inline Vector3 & Vector3::operator+=(const Vector3 & rhs)
{
	data[0] += rhs.data[0];
	data[1] += rhs.data[1];
	data[2] += rhs.data[2];
	return *this;
	//   return 	 Vector3 (0);
}

inline Vector3 & Vector3::operator-=(const Vector3 & rhs)
{
	data[0] -= rhs.data[0];
	data[1] -= rhs.data[1];
	data[2] -= rhs.data[2];
	return *this;
}

inline Vector3 & Vector3::operator*=(const float rhs)
{
	data[0] *= rhs;
	data[1] *= rhs;
	data[2] *= rhs;
	return *this;
}

inline Vector3 & Vector3::operator/=(const float rhs)
{
	const float inv_rhs = 1.0f/rhs;
	data[0] *= inv_rhs;
	data[1] *= inv_rhs;
	data[2] *= inv_rhs;
	return *this;
}

inline Vector3 Vector3::operator+(const Vector3 & rhs) const
{
	return Vector3(data[0] + rhs.data[0], 
		data[1] + rhs.data[1], 
		data[2] + rhs.data[2]);
}

inline float Vector3::operator*( const Vector3 & rhs)
{
	return (data[0] * rhs.data[0]+ 
		data[1] * rhs.data[1]+ 
		data[2] * rhs.data[2]);
}


inline Vector3 Vector3::operator-(const Vector3 & rhs) const
{
	return Vector3(data[0] - rhs.data[0], 
		data[1] - rhs.data[1], 
		data[2] - rhs.data[2]);
}

inline bool Vector3::sensible() const
{
	//  for (unsigned i = 3 ; i-- != 0 ;)
	//  {
	//    if (!is_finite(data[i]))
	//      return false;
	//   }
	return true;
}

inline void Vector3::show(const char * str) const
{
	//  uint i;
	//  TRACE("%s Vector3::this = 0x%x \n", str, (int) this);
	//  for (i = 0 ; i < 3 ; i++)
	//  {
	//    TRACE("%4f ", data[i]);
	//  }
	//   TRACE("\n");
}

// Helper for orthonormalise - projection of v2 onto v1
static inline Vector3 proj(const Vector3 & v1, const Vector3 & v2)
{
	return dot(v1, v2) * v1 / v1.mag2();
}

inline void Matrix3::orthonormalise()
{
	Vector3 u1(operator()(0, 0), operator()(1, 0), operator()(2, 0));
	Vector3 u2(operator()(0, 1), operator()(1, 1), operator()(2, 1));
	Vector3 u3(operator()(0, 2), operator()(1, 2), operator()(2, 2));

	Vector3 w1 = u1.normalise();

	Vector3 w2 = (u2 - proj(w1, u2)).normalise();
	Vector3 w3 = (u3 - proj(w1, u3) - proj(w2, u3)).normalise();

	operator()(0, 0) = w1[0];
	operator()(1, 0) = w1[1];
	operator()(2, 0) = w1[2];

	operator()(0, 1) = w2[0];
	operator()(1, 1) = w2[1];
	operator()(2, 1) = w2[2];

	operator()(0, 2) = w3[0];
	operator()(1, 2) = w3[1];
	operator()(2, 2) = w3[2];

	if (sensible() == false)
	{
		*this = Matrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));
	}
}


// global operators

inline Vector3 operator*(const Vector3 & lhs, const float rhs)
{
	return Vector3(lhs.data[0] * rhs,
		lhs.data[1] * rhs,
		lhs.data[2] * rhs);
}

inline Vector3 operator/(const Vector3 & lhs, const float rhs)
{
	const float inv_rhs = 1.0f/rhs;
	return Vector3(lhs.data[0] * inv_rhs,
		lhs.data[1] * inv_rhs,
		lhs.data[2] * inv_rhs);
}

//inline float operator*(const Vector3 & lhs, const Vector3 & rhs)
//{
//  return (lhs.data[0] * rhs.data[0] +
//          lhs.data[1] * rhs.data[1] +
//          lhs.data[2] * rhs.data[2]);
//}


inline float dot(const Vector3 & lhs, const Vector3 & rhs)
{
	return (lhs.data[0] * rhs.data[0] +
		lhs.data[1] * rhs.data[1] +
		lhs.data[2] * rhs.data[2]);
}

inline Vector3 cross(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs[1]*rhs[2] - lhs[2]*rhs[1],
		lhs[2]*rhs[0] - lhs[0]*rhs[2],
		lhs[0]*rhs[1] - lhs[1]*rhs[0]);
}

// matrix * vector
inline Vector3 operator*(const Matrix3 & lhs, const Vector3 & rhs)
{
	return Vector3(
		lhs(0,0) * rhs[0] +
		lhs(0,1) * rhs[1] +
		lhs(0,2) * rhs[2],
		lhs(1,0) * rhs[0] +
		lhs(1,1) * rhs[1] +
		lhs(1,2) * rhs[2],
		lhs(2,0) * rhs[0] +
		lhs(2,1) * rhs[1] +
		lhs(2,2) * rhs[2]);
}

inline Matrix3 transpose(const Matrix3 & rhs)
{
	return Matrix3(rhs(0, 0), rhs(0, 1), rhs(0, 2),
		rhs(1, 0), rhs(1, 1), rhs(1, 2),
		rhs(2, 0), rhs(2, 1), rhs(2, 2) );
}

inline float trace(const Matrix3 & rhs)
{
	return rhs(0,0) + rhs(1,1) + rhs(2,2);
}

// Some useful rotation Matrix3's
inline Matrix3 m3alpha(float alpha)
{
	Matrix3 result(0);
	float s = (float) sin_deg(alpha);
	float c = (float) cos_deg(alpha);

	result(0,0) = 1;
	result(1,1) = c;
	result(2,2) = c;
	result(2,1) = s;
	result(1,2) = -s;

	return result;
}

inline Matrix3 m3beta(float beta)
{
	Matrix3 result(0);
	float s = (float) sin_deg(beta);
	float c = (float) cos_deg(beta);

	result(1,1) = 1;
	result(2,2) = c;
	result(0,0) = c;
	result(0,2) = s;
	result(2,0) = -s;

	return result;
}

inline Matrix3 m3gamma(float gamma)
{
	Matrix3 result(0);
	float s = (float) sin_deg(gamma);
	float c = (float) cos_deg(gamma);

	result(2,2) = 1;
	result(0,0) = c;
	result(1,1) = c;
	result(1,0) = s;
	result(0,1) = -s;

	return result;
}

inline Vector3 RotateZ( Vector3 & ver1,float ang)
{
	FLOAT X , Y;
	X = ver1.X * cosf(ang) - ver1.Y * sinf(ang);
	Y = ver1.X * sinf(ang) + ver1.Y * cosf(ang);
	return Vector3 (X,Y) ;  

}

inline Matrix3 rotation_matrix(float ang, const Vector3 & dir)
{
	// from page 32(45) of glspec.dvi
	Matrix3 uut(dir[0]*dir[0], dir[1]*dir[0], dir[2]*dir[0],
		dir[0]*dir[1], dir[1]*dir[1], dir[2]*dir[1],
		dir[0]*dir[2], dir[1]*dir[2], dir[2]*dir[2]);

	//    uut.set(0,0, dir[0]*dir[0]);
	//    uut.set(0,1, dir[0]*dir[1]);
	//    uut.set(0,2, dir[0]*dir[2]);

	//    uut.set(1,0, dir[1]*dir[0]);
	//    uut.set(1,1, dir[1]*dir[1]);
	//    uut.set(1,2, dir[1]*dir[2]);

	//    uut.set(2,0, dir[2]*dir[0]);
	//    uut.set(2,1, dir[2]*dir[1]);
	//    uut.set(2,2, dir[2]*dir[2]);

	Matrix3 s(0, dir[2], -dir[1],
		-dir[2], 0, dir[0],
		dir[1], -dir[0], 0);

	//    s.set(0,1, -dir[2]);
	//    s.set(0,2,  dir[1]);

	//    s.set(1,0,  dir[2]);
	//    s.set(1,2, -dir[0]);

	//    s.set(2,0, -dir[1]);
	//    s.set(2,1,  dir[0]);

	return (uut + (float) cos_deg(ang) * 
		(matrix3_identity() - uut) + (float) sin_deg(ang) * s);
}

// converts a rotation matrix into a rotation of degrees about axis
void calculate_rot_from_matrix(const Matrix3 & matrix, Vector3 & axis, float & degrees)
{
	float factor = (trace(matrix) - 1.0f) * 0.5f;
	if (factor > 1.0f)
		factor = 1.0f;
	else if (factor < -1.0f)
		factor = -1.0f;
	degrees = acos_deg(factor);

	if (degrees == 0.0f)
	{
		axis = Vector3(1.0f, 0.0f, 0.0f);
		return;
	}
	else if (degrees == 180.0f)
	{
		if ( (matrix(0, 0) > matrix(1, 1)) && (matrix(0, 0) > matrix(2, 2)) )
		{
			axis[0] = 0.5f * sqrt(matrix(0, 0) - matrix(1, 1) - matrix(2, 2) + 1.0f);
			axis[1] = matrix(0, 1) / (2.0f * axis[0]);
			axis[2] = matrix(0, 2) / (2.0f * axis[0]);
		}
		else if (matrix(1, 1) > matrix(2, 2))
		{
			axis[1] = 0.5f * sqrt(matrix(1, 1) - matrix(0, 0) - matrix(2, 2) + 1.0f);
			axis[0] = matrix(0, 1) / (2.0f * axis[1]);
			axis[2] = matrix(1, 2) / (2.0f * axis[1]);
		}
		else
		{
			axis[2] = 0.5f * sqrt(matrix(2, 2) - matrix(0, 0) - matrix(1, 1) + 1.0f);
			axis[0] = matrix(0, 2) / (2.0f * axis[2]);
			axis[2] = matrix(1, 2) / (2.0f * axis[2]);
		}
	}
	else
	{
		axis[0] = matrix(2, 1) - matrix(1, 2);
		axis[1] = matrix(0, 2) - matrix(2, 0);
		axis[2] = matrix(1, 0) - matrix(0, 1);
	}
	axis.normalise();
}



//反正切值Atn
inline float User_Atn(float X,float Y ) 
{
	if (X == 0)
	{
		if (Y > 0)
			return PI_DIV2;
		else 
			return PI_DIV2 * 3;
	}

	float vel; 
	vel = atanf(fabs(Y / X));

	if ((Y >= 0) && (X > 0))
		return vel;
	else if ((Y >= 0) && (X < 0))
		return PI - vel;
	else if ((Y < 0) && (X < 0)) 
		return PI + vel;
	else  
		return TWO_PI - vel;

}



#endif //_3DMATH_H_