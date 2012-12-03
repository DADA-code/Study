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

  // 正準オイラー角の３つの組を決定する
  void Canonize();

  //  四元数をオイラー各形式に変換する
  //  入力される四元数は、その名前が示すようにオブジェクト空間から慣性空間、
  //  または、慣性空間からオブジェクト空間への回転を実行するものとする。
  void FromObjectToInertialQuaternion( const Quaternion &q);
  void FromInertialToObjectQuaternion( const Quaternion &q);

  // 座標変換行列をオイラー角形式に変換する
  // 入力される行列は、それが示すようにオブジェクト空間からワールド空間、
  // ワールド空間からオブジェクト空間への座標変換を実行するものつる。
  // この行列は、直交しているものとする。
  void FromObjectToWorldMatrix(const Matrix4x3 &m);
  void FromWorldToObjectMatrix(const Matrix4x3 &m);

  //回転行列をオイラー角形式に変換する
  void FromRotationMatrix(const RotationMatrix &m);
};


const EulerAngles g_EulerAngles_Identity(0.0f, 0.0f, 0.0f);

} // namespace math
} // namespace ss

#endif // LIBSS_MATH_EULERANGLES_H