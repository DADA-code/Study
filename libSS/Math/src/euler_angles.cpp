#include "euler_angles.hpp"

#include <math.h>

#include "math_util.hpp"

namespace ss { namespace math {

void EulerAngles::Canonize() {

  // 最初にピッチ範囲を-π...πにラップする。
  m_pitch = WrapPi(m_pitch);

  // ここで、正準範囲 -π/2...π/2の外側で行列m_pitchのウラ側をチェックする
  if( m_pitch < -g_PiOver2) {
    m_pitch = -g_Pi - m_pitch;
    m_heading += g_Pi;
    m_bank    += g_Pi;
  } else if ( m_pitch > g_PiOver2) {
    m_pitch   = g_Pi - m_pitch;
    m_heading += g_Pi;
    m_bank    += g_Pi;
  }

  // ここでジンバルロックをチェックする
  // 不正確さのために許容範囲を少し与える
  if( fabs(m_pitch) > g_PiOver2 - 1e-4){
    //  ジンバルロック内にいる
    //  垂直軸に関するすべての回転をヘディングに割り当てる
    m_heading += m_bank;
    m_bank    = 0.0f;
  } else {
    // ジンバルロックにいない
    // バンク角を正準範囲にラップする
    m_bank = WrapPi(m_bank);
  }

  // ヘディングを正準範囲にラップする
  m_heading = WrapPi(m_heading);
}

} // namespace math 
} // namespace ss