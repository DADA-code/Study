/*
* Vector3ƒNƒ‰ƒX‚Ìtest
*
*
*/


#include <gtest/gtest.h>

#include "vector3.hpp"

using namespace ss::math;

class Vector3Test : public ::testing::Test {
protected:

  Vector3Test() {}
  virtual ~Vector3Test() {}

  virtual void SetUp(){}
  virtual void TearDown(){}

  Vector3 vec; 
 
  void Vector3_EXPECT_EQ(const Vector3 &a, const Vector3 &b){
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
  }
};


TEST_F(Vector3Test, CopyConstructorTest){
 
 Vector3 a;
 a.x=0; a.y=1; a.z=-1;
 Vector3 b(a);
 
 Vector3_EXPECT_EQ(a, b);
}

TEST_F(Vector3Test, ConstructorTest1){
  const float x = 0;
  const float y = 1;
  const float z = -1;

  Vector3 a(x, y, z);
  EXPECT_FLOAT_EQ(x, a.x);
  EXPECT_FLOAT_EQ(y, a.y);
  EXPECT_FLOAT_EQ(z, a.z);
}

TEST_F(Vector3Test, OpEqualeTest){
  Vector3 a(0, -1, 1);
  Vector3 b = a;
  Vector3_EXPECT_EQ(a, b);
}

TEST_F(Vector3Test, OpEqEqTest){
  Vector3 a(-1, 0, 1);
  Vector3 b(-1, 0, 1);
  Vector3 c(-1, -1, 1);
  Vector3 d( 1, 0, 1);
  Vector3 f(-1, 0, -1);

  EXPECT_TRUE ( a == b );
  EXPECT_FALSE( a == c );
  EXPECT_FALSE( a == d );
  EXPECT_FALSE( a == f );
}

TEST_F(Vector3Test, OpNotEqTest){
  Vector3 a(-1, 0, 1);
  Vector3 b(-1, 0, 1);
  Vector3 c(-1, -1, 1);
  Vector3 d( 1, 0, 1);
  Vector3 f(-1, 0, -1);

  EXPECT_FALSE ( a != b );
  EXPECT_TRUE  ( a != c );
  EXPECT_TRUE  ( a != d );
  EXPECT_TRUE  ( a != f );
}

TEST_F(Vector3Test, ZeroTest) {
  Vector3 a(200.0f, -50.0f, 3.0f);
  Vector3 b(0.0f, 0.0f, 0.0f);
  Vector3 c(5.0f, -10, 3.0f);

  Vector3 zero(0.0f, 0.0f, 0.0f);
  
  a.zero();
  b.zero();
  c.zero();

  Vector3_EXPECT_EQ(zero, a);
  Vector3_EXPECT_EQ(zero, b);
  Vector3_EXPECT_EQ(zero, c);
}

TEST_F(Vector3Test, OpPlusTest) {
  Vector3 a(1.0f, 0.0f, 10.0f);
  Vector3 b(0.0f, 2.5f, -10.0f);

  Vector3_EXPECT_EQ(Vector3(1.0f, 2.5f, 0.0f), a+b);
}

TEST_F(Vector3Test, OpMinusTest) {
  Vector3 a(1.0f, 0.0f,  10.0f);
  Vector3 b(0.0f, 2.5f, -10.0f);
  Vector3_EXPECT_EQ(Vector3( 1.0f,-2.5f,  20.0f), a-b);
  Vector3_EXPECT_EQ(Vector3(-1.0f, 2.5f, -20.0f), b-a);
}

TEST_F(Vector3Test, OpMultipleTest) {
  Vector3 a(1.0f, 0.0f,  10.0f);
  float b = 5;
  float c = -5;

  Vector3_EXPECT_EQ(Vector3( 5.0f, 0.0f,  50.0f), a*b);
  Vector3_EXPECT_EQ(Vector3( 5.0f, 0.0f,  50.0f), b*a);
  Vector3_EXPECT_EQ(Vector3(-5.0f, 0.0f, -50.0f), c*a);
  Vector3_EXPECT_EQ(Vector3(-5.0f, 0.0f, -50.0f), a*c);
}

TEST_F(Vector3Test, OpDivTest) {
  Vector3 a(1.0f, 0.0f, 10.0f);
  float b = 2.0f;
  float c = 3.0f;

  Vector3_EXPECT_EQ(Vector3(1.0f/2.0f, 0.0f/2.0f, 10.0f/2.0f), a/b);
  Vector3_EXPECT_EQ(Vector3(1.0f/3.0f, 0.0f/3.0f, 10.0f/3.0f), a/c); 
}