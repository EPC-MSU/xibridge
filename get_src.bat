@set URL="https://github.com/EPC-MSU/xibridge.git"
@set DISTARCH="."
@set XIBRIDGEVER=v.1.0.3

call :SRC


:SUCCESS
@echo SUCCESS
@goto :eof

:FAIL
@echo FAIL
exit /B 1


:SRC
rmdir /S /Q %DISTARCH%\xibridge
mkdir %DISTARCH%\xibridge
git clone --recursive %URL% %DISTARCH%/xibridge
@if not %errorlevel% == 0 goto FAIL
cd %DISTARCH%\xibridge
git checkout %XIBRIDGEVER%
@if not %errorlevel% == 0 goto FAIL
git submodule update --init --recursive
@if not %errorlevel% == 0 goto FAIL
git submodule update --recursive
@if not %errorlevel% == 0 goto FAIL

rmdir /S /Q ".git"
del .gitignore
del .gitmodules

@goto :eof
