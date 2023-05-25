@SET GOOD_WORK=0
@SET IP=172.16.131.37
@SET COUNT=2

call :OneTestStep 5000
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
@set /A COUNTER=COUNTER+1
example.exe %IP% %1%
@if not %errorlevel%==0 goto loop
@set /A PER=PER+10
::@echo Current %PER%
@goto loop

:end
@echo At timeout %1% > _test_.txt
@echo - %PER% %% > _test_.txt
@goto :eof
