/*
*
*
*
*/


#include <gtest/gtest.h>
#include <math.h>

#include "math_util.hpp" // テスト用関数

using namespace ss::math;

// floatテストの誤差をグローバルに定義する
const float g_Float_Test_Delta = 0.00001f;

TEST(MathUtilTest, SinCosTest) {


  float val_cos = 0.0f;
  float val_sin = 0.0f;
  float theta   = 0.0f;

  theta = g_PiOver2 * 0.5f;
  SinCos( &val_sin, &val_cos, theta);
  EXPECT_FLOAT_EQ(sqrt(2.0f)*0.5f, val_cos);
  EXPECT_FLOAT_EQ(sqrt(2.0f)*0.5f, val_sin);

  theta = g_PiOver2;
  SinCos( &val_sin, &val_cos, theta);
  EXPECT_NEAR(0.0f, val_cos, g_Float_Test_Delta); 
  EXPECT_NEAR(1.0f, val_sin, g_Float_Test_Delta);

  theta =  -g_PiOver2 * 0.5f;
  SinCos( &val_sin, &val_cos, theta);
  EXPECT_FLOAT_EQ( sqrt(2.0f)*0.5f, val_cos);
  EXPECT_FLOAT_EQ(-sqrt(2.0f)*0.5f, val_sin);
}

TEST(MathUtilTest, WrapPiTest){

    int n = 3;
    for(int i=-n; i<=n ; ++i){
      EXPECT_NEAR( 0.0f, WrapPi(g_2Pi*i + 0.0f), g_Float_Test_Delta);
      EXPECT_NEAR( 1.0f, WrapPi(g_2Pi*i + 1.0f), g_Float_Test_Delta);
      EXPECT_NEAR(-1.0f, WrapPi(g_2Pi*i - 1.0f), g_Float_Test_Delta);
    }
    
}

TEST(MathUtilTest, SafeAcosTest){
  //  範囲内のチェック
  EXPECT_FLOAT_EQ(0.0f, SafeAcos(1.0f));
  EXPECT_FLOAT_EQ(g_Pi, SafeAcos(-1.0f));
  EXPECT_NEAR(g_PiOver2, SafeAcos(0.0f), g_Float_Test_Delta);
  EXPECT_FLOAT_EQ(acos(0.25f), SafeAcos(0.25f));
  EXPECT_FLOAT_EQ(acos(-0.25f), SafeAcos(-0.25f));
  EXPECT_FLOAT_EQ(acos(-0.75f), SafeAcos(-0.75f));

  //  範囲外のチェック
  EXPECT_FLOAT_EQ(0.0f, SafeAcos(10.0f));
  EXPECT_FLOAT_EQ(g_Pi,  SafeAcos(-10.0f));
  

}