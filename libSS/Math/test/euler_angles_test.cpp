/*
*
*
*
*/

#include <gtest/gtest.h>
#include <math.h>

#include "euler_angles.hpp"


using namespace ss::math;


class EulerAnglesTest : public ::testing::Test {
protected:
  EulerAnglesTest() {}
  virtual ~EulerAnglesTest () {}
  virtual void SetUp() {}
  virtual void TearDown() {}

};


//TEST_F(EulerAnglesTest, DefultConstructorTest) {}
//TEST_F(EulerAnglesTest, Constructor1Test){}
//TEST_F(EulerAnglesTest, IdentityTest){}
TEST_F(EulerAnglesTest, CanonizeTest){

}


TEST_F(EulerAnglesTest, FromObjectToInertialQuaternionTest){}
TEST_F(EulerAnglesTest, FromInertialToObjectQuaternionTest){}
TEST_F(EulerAnglesTest, FromObjectToWorldMatrixTest){}
TEST_F(EulerAnglesTest, FromWorldToObjectMatrixTest){}
TEST_F(EulerAnglesTest, FromRotationMatrixTest){}