set path=%path%;C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;C:\Windows\Microsoft.NET\Framework\v4.0.30319
set GTEST_ROOT=%GTEST_ROOT%;C:\gtest-1.6.0

msbuild Math.sln /p:Configuration=Debug /t:Clean,Build
Debug\math_test.exe --gtest_output=xml:../../test_result/test_vc10_d.xml

cd ../../
cppcheck.exe -Iinclude --enable=all src --xml  2> cppcheck_result.xml

set PATH=%PATH%;C:\Program Files (x86)\CCCC
dir /b /s | cccc -