#include "euler_angles.hpp"

#include <math.h>

#include "math_util.hpp"

namespace ss { namespace math {

void EulerAngles::Canonize() {

  // �ŏ��Ƀs�b�`�͈͂�-��...�΂Ƀ��b�v����B
  m_pitch = WrapPi(m_pitch);

  // �����ŁA�����͈� -��/2...��/2�̊O���ōs��m_pitch�̃E�������`�F�b�N����
  if( m_pitch < -g_PiOver2) {
    m_pitch = -g_Pi - m_pitch;
    m_heading += g_Pi;
    m_bank    += g_Pi;
  } else if ( m_pitch > g_PiOver2) {
    m_pitch   = g_Pi - m_pitch;
    m_heading += g_Pi;
    m_bank    += g_Pi;
  }

  // �����ŃW���o�����b�N���`�F�b�N����
  // �s���m���̂��߂ɋ��e�͈͂������^����
  if( fabs(m_pitch) > g_PiOver2 - 1e-4){
    //  �W���o�����b�N���ɂ���
    //  �������Ɋւ��邷�ׂẲ�]���w�f�B���O�Ɋ��蓖�Ă�
    m_heading += m_bank;
    m_bank    = 0.0f;
  } else {
    // �W���o�����b�N�ɂ��Ȃ�
    // �o���N�p�𐳏��͈͂Ƀ��b�v����
    m_bank = WrapPi(m_bank);
  }

  // �w�f�B���O�𐳏��͈͂Ƀ��b�v����
  m_heading = WrapPi(m_heading);
}

} // namespace math 
} // namespace ss