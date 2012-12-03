/*
*
*
*
*
*/


#ifndef LIBSS_MATH_QUATERNION_H
#define LIBSS_MATH_QUATERNION_H

class Vector3;
class EulerAngles;

namespace ss {
namespace math {

class Quaternion {
public:
  // ４元数の4つの値
  // 通常これらを直接操作する必要はない
  // しかしながら、これらはpublicにしておく 
  // 直接アクセスを禁止すると、いくつかの操作（例えば、ファイルI/O)が
  // 不必要に複雑になるからである
  float w, x, y, z; 

  void Identity() { w = 1.0f, x = y = z = 0.0f; }

  //四元数を特定の角度にセットアップする
  void SetToRotateAboutX(float theta);
  void SetToRotateAboutY(float theta);
  void SetToRotateAboutZ(float theta);
  void SetToRotateAboutAxis(const Vector3 &axis, float theta);

  // オブジェクト空間<->慣性空間の回転を実行するようにセットアップする
  // 方向はオイラー角形式で与えられる
  void SetToRotateObjectToInternal(const EulerAngles &orientation);
  void SetToRotateInertialToObject(const EulerAngles &orientation);

  // 外積
  Quaternion operator* (const Quaternion &a) const;

  // 代入を伴う乗算：C++の慣習による
  Quaternion &operator *=(const Quaternion &a);

  // 四元数を正規化する
  void normalize();

  // 回転角と軸を取り出し返す
  float GetRotationAngle() const;
  Vector3 GetRotationAxis() const;
};

// 恒等四元数用のグローバル定数
extern const Quaternion g_Quaternion_Identity;

// 四元数の内積
extern float DotProduct( const Quaternion &a, const Quaternion &b);

// 球面線形補間
extern Quaternion Slerp( const Quaternion &p, const Quaternion &q, float t);

// 四元数の結合
extern Quaternion Conjugate( const Quaternion &q);

// 四元数の累乗
extern Quaternion Pow( const Quaternion &q, float exponent);


} // namespace math
} // namespace ss

#endif //LIBSS_MATH_QUATERNION_H