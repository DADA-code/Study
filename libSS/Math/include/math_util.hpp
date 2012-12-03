#ifndef LIBSS_MATH_MATHUTIL_HPP
#define LIBSS_MATH_MATHUTIL_HPP

#include <math.h>

namespace ss {
namespace math {

// pi�Ƃ����邩�̔{���p�̃O���[�o���萔��p��
const float g_Pi      = 3.14159265f;
const float g_2Pi     = g_Pi * 2.0f;
const float g_PiOver2 = g_Pi * 0.5f;
const float g_1OverPi = 1.0f / g_Pi;
const float g_1Over2Pi = 1.0f / g_2Pi;


// TODO: extern�ɂ���ׂ����H

// �K�؂ɂQ�΂̔{���������邱�ƂŊp�x��-��...�΂͈̔͂Ƀ��b�v����B
inline float WrapPi(float theta){
  float ans = theta;
  ans += g_Pi;
  ans -= floor(ans * g_1Over2Pi) * g_2Pi;
  ans -= g_Pi;
  return ans;
}

// ���S�ȋt�O�p�֐�
// x > 1.0f  : return 0.0f;
// x < -1.0f : return g_Pi;
inline float SafeAcos(float x) {
  if( x < -1.0f) return g_Pi;
  if( x >  1.0f) return 0.0f; 
  return acos(x);
}




inline void SinCos(float *return_sin, float *return_cos, float theta) {
  // �ȒP���̂��߂ɁA�P�ɒʏ�̎O�p�֐���p����
  // ���ӂ��ׂ����Ƃ́A�v���b�g�t�H�[���ɂ���Ă͂��悢���@��
  // ���݂���ꍇ������Ƃ������Ƃł���  

  *return_sin = sin(theta) ;
  *return_cos = cos(theta) ;
}


} // namespace math
} // namespace ss


#endif //LIBSS_MATH_MATHUTIL_HPP