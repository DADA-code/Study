#ifndef   LIBSS_MATH_HPP
#define   LIBSS_MATH_HPP

#include <math.h>

namespace ss{
namespace math{

class Vector3{
public:
  float x, y, z;

  Vector3(){}; // あえて未定義になるように設定
  Vector3(const Vector3 &a): x(a.x), y(a.y), z(a.z) {};
  Vector3(float nx, float ny, float nz): x(nx), y(ny), z(nz) {};

  Vector3& operator= (const Vector3& a) {
    x = a.x; y = a.y; z = a.z;
    return *this;
  }

  bool operator== (const Vector3& a) const {
    return x==a.x && y==a.y && z==a.z;
  }

  bool operator!= (const Vector3& a) const {
    return x!=a.x || y!=a.y || z!=a.z;
  }


//ベクトル操作
  void zero() { x = y = z = 0.0f; }
  Vector3 operator- ()const { return Vector3(-x, -y, -z); }

  Vector3 operator+ (const Vector3& a) const {
    return Vector3(x+a.x, y+a.y, z+a.z);
  }

  Vector3 operator- (const Vector3& a) const {
    return Vector3(x-a.x, y-a.y, z-a.z);
  }

  Vector3 operator* (float a) const {
    return Vector3(x*a, y*a, z*a);
  }

  Vector3 operator /(float a) const {
    float one_over_a = 1.0f / a;
    return Vector3(x * one_over_a, y * one_over_a, z * one_over_a);
  }

  Vector3& operator+= (const Vector3& a) {
    x+=a.x; y+=a.y; z+=a.z;
    return *this;
  }
  
  Vector3& operator-=(const Vector3& a) {
    x-=a.x; y-=a.y; z-=a.z;
    return *this;
  }

  Vector3& operator*=(float a) {
    x *= a; y *= a; z *= a;
    return *this;
  }

  Vector3& operator/=(float a) {
    float one_over_a = 1.0f/a;
    x *= one_over_a; y *= one_over_a; z *= one_over_a;
    return *this;
  }
}; // class Vector3

inline float VectorMag(const Vector3& a) {
  return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

inline Vector3 CrossProduct(const Vector3& a, const Vector3& b){
  return Vector3(a.y*b.z - a.z*b.y,
                 a.z*b.x - a.x*b.z,
                 a.x*b.y - a.y*b.x);
}

inline Vector3 operator *(float k, const Vector3 &v){
  return Vector3(k*v.x, k*v.y, k*v.z);
}

inline float distance(const Vector3 &a, const Vector3 &b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  float dz = a.z - b.z;
  return sqrt(dx*dx + dy*dy + dz*dz);
}


const Vector3 g_ZeroVector;

} // namespace math
} // namespace ss


#endif // LIBSS_MATH_HPP