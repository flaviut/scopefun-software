@echo off

echo ... prepare directory ...
rmdir %cd%\..\source /s /q
mkdir %cd%\..\source

echo ... svn checkout ...
git clone file:///%1 ./../source
rmdir %cd%\..\source\.git /s /q

echo ... 7zip sources ...
7z a -r %cd%\..\source-win.7z %cd%\..\source\*

echo ... directory cleanup ...
rmdir %cd%\..\source /s /q

echo ... finished ...
pause
