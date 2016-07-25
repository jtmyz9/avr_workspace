jtagiceii.exe -cUSB -dAT90CAN128 -e -f39423 -F39423 -E255 -G255 -l255 -L255 -pf -vf -if..\dvk90can1_test_v2\output_gcc\dvk90can1_test_v2_gcc.hex -R
IF ERRORLEVEL == 1 GOTO DOS_PAUSE
GOTO END
:DOS_PAUSE
pause
:END
