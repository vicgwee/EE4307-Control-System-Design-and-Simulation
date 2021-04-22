@echo off
set MATLAB=D:\Program Files\MATLAB\R2020b
"%MATLAB%\bin\win64\gmake" -f AUV.mk  OPTS="-DTID01EQ=0"
