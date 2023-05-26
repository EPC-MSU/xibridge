@SET IP=172.16.131.37
@SET COUNT=10
@echo Test started > _test_.txt
call :OneTestStep 4000
::call :OneTestStep 3000
::call :OneTestStep 4000
::call :OneTestStep 6000
::call :OneTestStep 6500
::call :OneTestStep 7000
::call :OneTestStep 15000
@goto :eof

:: One test step function
:OneTestStep
@set COUNTER=0
@set PER=0
@goto loop

:loop
@if %COUNTER%==%COUNT% goto end
@set /A TM=%1%/1000
@set /A COUNTER=COUNTER+1
@timeout %TM%
example.exe %IP% %1%
@if not %errorlevel%==0 goto loop
@set /A PER=PER+10
::@echo Current %PER%
@goto loop

:end
@echo At timeout %1% >> _test_.txt
@echo - %PER% %% >> _test_.txt
@goto :eof
