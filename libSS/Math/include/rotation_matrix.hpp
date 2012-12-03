/*
*
*
*
*/

#ifndef LIBSS_MATH_ROTATIONMATRIX_H
#define LIBSS_MATH_ROTATIONMATRIX_H

namespace ss {
namespace math {
  
class Vector3;
class EulerAngles;
class Quaternion;


class RotationMatrix {
public:
  float m11, m12, m13;
  float m21, m22, m23;
  float m31, m32, m33;

  // 恒等行列を設定する
  void Identity();

  // 特定の向きを持つ行列をセットアップする
  void setup(const EulerAngles &orientation);

  // 四元数から行列をセットアップする
  // この四元数は、指定された座標変換の向きで回転を実行するものとする
  void FromInertialToObjectQuaternion( const Quaternion &q);
  void FromObjectToInertialQuaternion( const Quaternion &q);

  // 回転を実行する
  Vector3 InertialToObject(const Vector3 &v) const;
  Vector3 ObjectToInertial(const Vector3 &v) const;

};

}  // namespace ss
}  // namespace math

#endif  // LIBSS_MATH_ROTATIONMATRIX_H



