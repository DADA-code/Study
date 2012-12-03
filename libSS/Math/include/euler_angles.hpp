/**
*
*
*
*
*
*/

#ifndef LIBSS_MATH_EULERANGLES_H
#define LIBSS_MATH_EULERANGLES_H

namespace ss{
namespace math{

//foward declaration
class Quaternion;
class Matrix4x3;
class RotationMatrix;



class EulerAngles {
public:
  float m_heading;
  float m_pitch;
  float m_bank;

  EulerAngles();
  EulerAngles(float h, float p, float b) :m_heading(h), m_pitch(p), m_bank(b) {}

  void Identity() { m_heading = m_pitch = m_bank = 0.0f; }

  // �����I�C���[�p�̂R�̑g�����肷��
  void Canonize();

  //  �l�������I�C���[�e�`���ɕϊ�����
  //  ���͂����l�����́A���̖��O�������悤�ɃI�u�W�F�N�g��Ԃ��犵����ԁA
  //  �܂��́A������Ԃ���I�u�W�F�N�g��Ԃւ̉�]�����s������̂Ƃ���B
  void FromObjectToInertialQuaternion( const Quaternion &q);
  void FromInertialToObjectQuaternion( const Quaternion &q);

  // ���W�ϊ��s����I�C���[�p�`���ɕϊ�����
  // ���͂����s��́A���ꂪ�����悤�ɃI�u�W�F�N�g��Ԃ��烏�[���h��ԁA
  // ���[���h��Ԃ���I�u�W�F�N�g��Ԃւ̍��W�ϊ������s������̂�B
  // ���̍s��́A�������Ă�����̂Ƃ���B
  void FromObjectToWorldMatrix(const Matrix4x3 &m);
  void FromWorldToObjectMatrix(const Matrix4x3 &m);

  //��]�s����I�C���[�p�`���ɕϊ�����
  void FromRotationMatrix(const RotationMatrix &m);
};


const EulerAngles g_EulerAngles_Identity(0.0f, 0.0f, 0.0f);

} // namespace math
} // namespace ss

#endif // LIBSS_MATH_EULERANGLES_H