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

  // �s��̕��s�ړ������𒼐ڃA�N�Z�X����
  void ZeroTranslation();
  void SetTranslation(const Vector3 &d);
  void SetupTranslation( const Vector3 &d);

  // �e��� <-> ���[�J����Ԃœ���̍��W�ϊ����s���s����Z�b�g�A�b�v����
  // ���[�J����Ԃ́A�e��ԓ��Ŏw�肳�ꂽ�ʒu�ƕ����ɂ���Ƃ���
  // ���̕����́A�I�C���[�p����]�s��̂������ꂩ��p���Ďw�肳���ꍇ������
  void SetupLocalToParent( const Vector3 &pos, const EulerAngles &orient);
  void SetupLocalToParent( const Vector3 &pos, const RotationMatrix &orient);
  void SetupParentToLocal( const Vector3 &pos, const EulerAngles &orient);
  void SetupParentToLocal( const Vector3 &pos, const RotationMatrix &orient);

  // ��{���̎���̉�]���s���s����Z�b�g�A�b�v
  void SetupRotate( int axis, float theta);
  
  // �C�ӂ̎��̎���ɂ̉�]���s���s����Z�b�g�A�b�v
  void SetupRotate( const Vector3 &axis, float theta);

  // ��]���s���s����Z�b�g�A�b�v����
  // �p�ψʂ͎l�����`���Ƃ���
  void FromQuaternion( const Quaternion &q);

  // �e���ŃX�P�[�����O�����s����s����Z�b�g�A�b�v����B
  void SetupScale(const Vector3 &s);

  // �C�ӂ̎��ɂ����ăX�P�[�����O�����s����s����Z�b�g�A�b�v����
  void SetupScaleAlongAxis(const Vector3 &axis, float k);

  // ����f�����s����s����Z�b�g�A�b�v����
  void SetupShear(int axis, float s, float t);

  // ���_��ʂ镽�ʏ�ւ̓��e�����s����s����Z�b�g�A�b�v����
  void SetupProject( const Vector3 &n);

  // ��{�ʂɕ��s�ȖʂɊւ��Ẵ��t���N�V���������s����s����Z�b�g�A�b�v����
  void SetupReflect(int axis, float k = 0.0f);

  // ���_��ʂ�C�ӂ̕��ʂɊւ��郊�t���N�V���������s����s����Z�b�g�A�b�v����
  void SetupReflect(const Vector3 &n);
};


// ���Z�q*�͓_�̍��W�ϊ��Ɏg���A�܂��A�s��̘A���ɂ��g����
// ������E�ւ̏�Z�̏��Ԃ́A���W�ϊ��̏��ԂƓ����ł���
Vector3 operator* (const Vector3 &p, const Matrix4x3 &m);
Matrix4x3 operator* (const Matrix4x3 &a, const Matrix4x3 &b);

// ���Z�q *= ��C++�̕W���ɏ������邽�߂̂���
Vector3   &operator *=(Vector3 &p, const Matrix4x3 &m);
Matrix4x3 &operator *=(const Matrix4x3 &a, const Matrix4x3 &m);

//�s��� 3x3�̕����̍s�񎮂��v�Z����
float Determinant(const Matrix4x3 &m);

// �t�s����v�Z����
Matrix4x3 Inverse(const Matrix4x3 &m);

// �s�񂩂畽�s�ړ����������o��
Vector3 GetTranslation( const Matrix4x3 &m);

// ���[�J��->�e�A�܂��́A�e->���[�J���ւ̍s�񂩂�ʒu�ƕ��������o��
Vector3 GetPositionFromParentToLocalMatrix(const Matrix4x3 &m);
Vector3 GetPositionFromLocalToParentMatrix(const Matrix4x3 &m);


} // namespace math
} // namespace ss

#endif  //  LIBSS_MATH_MATRIX4X3_H