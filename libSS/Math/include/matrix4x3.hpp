/*
*
*
*
*/

#ifndef LIBSS_MATH_MATRIX4X3_H
#define LIBSS_MATH_MATRIX4X3_H

namespace ss {
namespace math {

class Vector3;
class EulerAngles;
class Quaternion;
class RotationMatrix;

class Matrix4x3 {

public:
  float m11, m12, m13;
  float m21, m22, m23;
  float m31, m32, m33;
  float  tx,  ty,  tz;

  void Identity();

  // 行列の平行移動部分を直接アクセスする
  void ZeroTranslation();
  void SetTranslation(const Vector3 &d);
  void SetupTranslation( const Vector3 &d);

  // 親空間 <-> ローカル空間で特定の座標変換を行う行列をセットアップする
  // ローカル空間は、親空間内で指定された位置と方向にあるとする
  // この方向は、オイラー角が回転行列のいざうれかを用いて指定される場合がある
  void SetupLocalToParent( const Vector3 &pos, const EulerAngles &orient);
  void SetupLocalToParent( const Vector3 &pos, const RotationMatrix &orient);
  void SetupParentToLocal( const Vector3 &pos, const EulerAngles &orient);
  void SetupParentToLocal( const Vector3 &pos, const RotationMatrix &orient);

  // 基本軸の周りの回転を行う行列をセットアップ
  void SetupRotate( int axis, float theta);
  
  // 任意の軸の周りにの回転を行う行列をセットアップ
  void SetupRotate( const Vector3 &axis, float theta);

  // 回転を行う行列をセットアップする
  // 角変位は四元数形式とする
  void FromQuaternion( const Quaternion &q);

  // 各軸でスケーリングを実行する行列をセットアップする。
  void SetupScale(const Vector3 &s);

  // 任意の軸にそってスケーリングを実行する行列をセットアップする
  void SetupScaleAlongAxis(const Vector3 &axis, float k);

  // せん断を実行する行列をセットアップする
  void SetupShear(int axis, float s, float t);

  // 原点を通る平面上への投影を実行する行列をセットアップする
  void SetupProject( const Vector3 &n);

  // 基本面に平行な面に関してのリフレクションを実行する行列をセットアップする
  void SetupReflect(int axis, float k = 0.0f);

  // 原点を通る任意の平面に関するリフレクションを実行する行列をセットアップする
  void SetupReflect(const Vector3 &n);
};


// 演算子*は点の座標変換に使われ、また、行列の連結にも使われる
// 左から右への乗算の順番は、座標変換の順番と同じである
Vector3 operator* (const Vector3 &p, const Matrix4x3 &m);
Matrix4x3 operator* (const Matrix4x3 &a, const Matrix4x3 &b);

// 演算子 *= はC++の標準に準拠するためのもの
Vector3   &operator *=(Vector3 &p, const Matrix4x3 &m);
Matrix4x3 &operator *=(const Matrix4x3 &a, const Matrix4x3 &m);

//行列の 3x3の部分の行列式を計算する
float Determinant(const Matrix4x3 &m);

// 逆行列を計算する
Matrix4x3 Inverse(const Matrix4x3 &m);

// 行列から平行移動部分を取り出す
Vector3 GetTranslation( const Matrix4x3 &m);

// ローカル->親、または、親->ローカルへの行列から位置と方向を取り出す
Vector3 GetPositionFromParentToLocalMatrix(const Matrix4x3 &m);
Vector3 GetPositionFromLocalToParentMatrix(const Matrix4x3 &m);


} // namespace math
} // namespace ss

#endif  //  LIBSS_MATH_MATRIX4X3_H