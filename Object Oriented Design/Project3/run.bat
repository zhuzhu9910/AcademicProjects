echo off
Debug\Project3.exe TestCode/Test/Test/ mylog.log  TestCode/test.h TestCode/test.cpp

echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo compile generated test project files
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

devenv TestCode/Test/Test.sln /Rebuild debug

echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo Run annotated test project
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TestCode\Test\Debug\Test.exe
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo Please go to see the log file
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

pause


