@echo off

echo ... timestamp ...
call %cd%\win-version.bat

echo ... clean data files ...
del /Q %cd%\..\..\bin\data\state\*.*

echo ... prepare directory ...
rmdir %cd%\..\..\package\scopefun-win\bin\ /s /q
mkdir %cd%\..\..\package\scopefun-win\bin\
mkdir %cd%\..\..\package\scopefun-win\bin\data
mkdir %cd%\..\..\package\scopefun-win\bin\driver
mkdir %cd%\..\..\package\scopefun-win\bin\locale

echo ... copy data files ...
xcopy /Y /E %cd%\..\..\bin\data\*.*          %cd%\..\..\package\scopefun-win\bin\data
xcopy /Y /E %cd%\..\..\bin\driver\*.*        %cd%\..\..\package\scopefun-win\bin\driver
xcopy /Y /E %cd%\..\..\bin\locale\*.*        %cd%\..\..\package\scopefun-win\bin\locale


echo ... copy exe ...
xcopy /Y %cd%\..\..\bin\oscilloscope32r.exe  %cd%\..\..\package\scopefun-win\bin
ren %cd%\..\..\package\scopefun-win\bin\oscilloscope32r.exe oscilloscope.exe

echo ... package ...
compil32 /cc "%cd%\..\..\package\scopefun-win\innosetup.iss"

del /Q %cd%\..\scopefun-win32.exe
ren %cd%\..\scopefun-win.exe scopefun-win32.exe

echo ... finished ...
pause
