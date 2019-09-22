@echo off
@setlocal EnableDelayedExpansion enableextensions
@cls
@cd %~dp0
@SET PROGRAM_NAME=%~dp0
@for /D %%a in ("%PROGRAM_NAME:~0,-1%.txt") do @SET PROGRAM_NAME=%%~na

@SET EABI=arm-none-eabi

@SET LIB_BIP_PATH="..\libbip"
@SET LIB_BIP="%LIB_BIP_PATH%\libbip.a" 

@SET GCC_OPT=-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fno-math-errno -I "%LIB_BIP_PATH%" -c -Os -Wa,-R -Wall -fpie -pie -fpic -mthumb -mlittle-endian  -ffunction-sections -fdata-sections
@SET LD_OPT=-lm -lc -EL -N -Os --cref -pie --gc-sections 

@SET AS=%EABI%-as
@SET LD=%EABI%-ld
@SET OBJCOPY=%EABI%-objcopy
@SET GCC=%EABI%-gcc
@SET NM=%EABI%-nm

if exist label.txt (
set /p LABEL=< label.txt
) else (
SET LABEL = %PROGRAM_NAME%
)

@call :echoColor 0F "====================================" 1
@call :echoColor 0F "Наименование проекта: "
@echo %PROGRAM_NAME%
@call :echoColor 0F "Компилятор: "
@echo %COMPILER%
@call :echoColor 0F "====================================" 1
@echo.	

@call :echoColor 0F "Начинаем сборку..." 1
@SET PARTNAME=%PROGRAM_NAME%
@call :echoColor 0B "Компиляция "
@call :echoColor 0E "%PARTNAME%" 1

@SET n=1
@for  %%f in (*.c) do ( 
@	SET FILES_TO_COMPILE=!FILES_TO_COMPILE! %%~nf.o
@call :EchoN "%n%.	%%~nf.c"
!GCC! !GCC_OPT! -o %%~nf.o %%~nf.c
@if errorlevel 1 goto :error
@call :echoColor 0A "...OK" 1
@SET /a n=n+1)
@SET /a n=n-1
@call :echoColor 0B "Итого: "
@call :echoColor 0E "%n%" 1

@call :echoColor 0B "Сборка..."
%LD% -Map %PARTNAME%.map -o %PROGRAM_NAME%.elf %FILES_TO_COMPILE% %LD_OPT% %LIB_BIP%
@if errorlevel 1 goto :error

if exist label.txt (
%OBJCOPY%  %PROGRAM_NAME%.elf --add-section .elf.label=label.txt
)

@call :EchoN "%PROGRAM_NAME%" > name.txt
%OBJCOPY%  %PROGRAM_NAME%.elf --add-section .elf.name=name.txt
if exist name.txt del name.txt
@if errorlevel 1 goto :error

@call :echoColor 0A "OK" 1
@call :echoColor 0B "Сборка окончена." 1

:done_

@call :echoColor 0A "Готово." 1 
pause 
@goto :EOF

:error
@call :echoColor 4e ОШИБКА! 1
@endlocal & @SET ERROR=ERROR
@pause
@goto :EOF

::===========================================================
:: A function prints text in first parameter without CRLF 
:EchoN
    
@    <nul set /p strTemp=%~1
@    exit /b 0
::===========================================================
::	Вывод заданной строки заданным цветом
::	3 параметр если не пустой задает перевод строки
::  0 = Черный 	8 = Серый
::  1 = Синий 	9 = Светло-синий
::  2 = Зеленый A = Светло-зеленый
::  3 = Голубой B = Светло-голубой
::  4 = Красный C = Светло-красный
::  5 = Лиловый D = Светло-лиловый
::  6 = Желтый 	E = Светло-желтый
::  7 = Белый 	F = Ярко-белый
:echoColor [Color] [Text] [\n]
 @ if not defined BS for /F "tokens=1 delims=#" %%i in ('"prompt #$H#& echo on& for %%j in (.) do rem"') do set "BS=%%i"
 @ if not exist foo set /p .=.<nul>foo
 @ set "regex=%~2" !
 @ set "regex=%regex:"="%"
 @ findstr /a:%1 /prc:"\." "%regex%\..\foo" nul
 @ set /p .=%BS%%BS%%BS%%BS%%BS%%BS%%BS%%BS%%BS%<nul
 @ if "%3" neq "" echo.
 @exit /b 0
::===========================================================
====================================================
