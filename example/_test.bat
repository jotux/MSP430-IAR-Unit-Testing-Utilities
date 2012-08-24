@echo off
del _test.txt
del _test.xml
call .\test\canary.cspy.bat .\test\Debug\Exe\canary.d43 >>_test.txt
call _parse_test.py
echo on