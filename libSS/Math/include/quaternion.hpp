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
  // �S������4�̒l
  // �ʏ킱���𒼐ڑ��삷��K�v�͂Ȃ�
  // �������Ȃ���A������public�ɂ��Ă��� 
  // ���ڃA�N�Z�X���֎~����ƁA�������̑���i�Ⴆ�΁A�t�@�C��I/O)��
  // �s�K�v�ɕ��G�ɂȂ邩��ł���
  float w, x, y, z; 

  void Identity() { w = 1.0f, x = y = z = 0.0f; }

  //�l���������̊p�x�ɃZ�b�g�A�b�v����
  void SetToRotateAboutX(float theta);
  void SetToRotateAboutY(float theta);
  void SetToRotateAboutZ(float theta);
  void SetToRotateAboutAxis(const Vector3 &axis, float theta);

  // �I�u�W�F�N�g���<->������Ԃ̉�]�����s����悤�ɃZ�b�g�A�b�v����
  // �����̓I�C���[�p�`���ŗ^������
  void SetToRotateObjectToInternal(const EulerAngles &orientation);
  void SetToRotateInertialToObject(const EulerAngles &orientation);

  // �O��
  Quaternion operator* (const Quaternion &a) const;

  // ����𔺂���Z�FC++�̊��K�ɂ��
  Quaternion &operator *=(const Quaternion &a);

  // �l�����𐳋K������
  void normalize();

  // ��]�p�Ǝ������o���Ԃ�
  float GetRotationAngle() const;
  Vector3 GetRotationAxis() const;
};

// �P���l�����p�̃O���[�o���萔
extern const Quaternion g_Quaternion_Identity;

// �l�����̓���
extern float DotProduct( const Quaternion &a, const Quaternion &b);

// ���ʐ��`���
extern Quaternion Slerp( const Quaternion &p, const Quaternion &q, float t);

// �l�����̌���
extern Quaternion Conjugate( const Quaternion &q);

// �l�����̗ݏ�
extern Quaternion Pow( const Quaternion &q, float exponent);


} // namespace math
} // namespace ss

#endif //LIBSS_MATH_QUATERNION_H