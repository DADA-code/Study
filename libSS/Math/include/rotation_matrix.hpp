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

  // �P���s���ݒ肷��
  void Identity();

  // ����̌��������s����Z�b�g�A�b�v����
  void setup(const EulerAngles &orientation);

  // �l��������s����Z�b�g�A�b�v����
  // ���̎l�����́A�w�肳�ꂽ���W�ϊ��̌����ŉ�]�����s������̂Ƃ���
  void FromInertialToObjectQuaternion( const Quaternion &q);
  void FromObjectToInertialQuaternion( const Quaternion &q);

  // ��]�����s����
  Vector3 InertialToObject(const Vector3 &v) const;
  Vector3 ObjectToInertial(const Vector3 &v) const;

};

}  // namespace ss
}  // namespace math

#endif  // LIBSS_MATH_ROTATIONMATRIX_H



