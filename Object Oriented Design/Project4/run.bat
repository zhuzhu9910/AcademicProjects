echo off
echo ////////////////////////////////////////////////
echo Please check my code for the requirements 1 - 3.
echo Just Watch the demonstrtion for requirements 5, 6. 
echo ////////////////////////////////////////////////////////////////
echo The server and client will automatically process the file
echo test.cpp and test.h. The server will copy and rename the 
echo file like aly_ + oringianl name. Then it will inspect them
echo and display the result in the server end and client end. The
echo server will also create logs for the result. Their names will
echo be original name + .log
echo ////////////////////////////////////////////////////////////////
pause


start "" "debug\ServerCtrlExecutive.exe" "100"
sleep 3000
start "" "debug\ClientCtrlExecutive.exe" "127.0.0.1" "test.cpp" "test.h"
