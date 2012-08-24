@echo off
IarBuild.exe .\test\canary.ewp -clean Debug
IarBuild.exe .\test\canary.ewp -make Debug >> _build.txt
_parse_build.py
echo on