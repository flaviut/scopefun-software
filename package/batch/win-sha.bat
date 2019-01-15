echo scopefun-win32.exe
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "& Get-FileHash './../scopefun-win32.exe' -Algorithm SHA256 | Format-List" > ./../scopefun-win32.sha
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "Get-Content './../scopefun-win32.sha' | select -First 4 | select -Last 2 > ./../scopefun-win32.txt
del  /Q %cd%\..\scopefun-win32.sha"

echo scopefun-win64.exe
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "& Get-FileHash './../scopefun-win64.exe' -Algorithm SHA256 | Format-List" > ./../scopefun-win64.sha
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "Get-Content './../scopefun-win64.sha' | select -First 4 | select -Last 2 > ./../scopefun-win64.txt
del  /Q %cd%\..\scopefun-win64.sha

echo scopefun-linux32.deb
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "& Get-FileHash './../scopefun-linux32.deb' -Algorithm SHA256 | Format-List" > ./../scopefun-linux32.sha
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "Get-Content './../scopefun-linux32.sha' | select -First 4 | select -Last 2 > ./../scopefun-linux32.txt
del  /Q %cd%\..\scopefun-linux32.sha

echo scopefun-linux64.deb
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "& Get-FileHash './../scopefun-linux64.deb' -Algorithm SHA256 | Format-List" > ./../scopefun-linux64.sha
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "Get-Content './../scopefun-linux64.sha' | select -First 4 | select -Last 2 > ./../scopefun-linux64.txt
del  /Q %cd%\..\scopefun-linux64.sha

echo scopefun-mac64.mpkg.zip
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "& Get-FileHash './../scopefun-mac64.mpkg.zip' -Algorithm SHA256 | Format-List" > ./../scopefun-mac64.sha
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "Get-Content './../scopefun-mac64.sha' | select -First 4 | select -Last 2 > ./../scopefun-mac64.txt
del  /Q %cd%\..\scopefun-mac64.sha

echo scopefun-mac32.mpkg.zip
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "& Get-FileHash './../scopefun-mac32.mpkg.zip' -Algorithm SHA256 | Format-List" > ./../scopefun-mac32.sha
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "Get-Content './../scopefun-mac32.sha' | select -First 4 | select -Last 2 > ./../scopefun-mac32.txt
del  /Q %cd%\..\scopefun-mac32.sha

echo source-linux.7z
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "& Get-FileHash './../source-linux.7z' -Algorithm SHA256 | Format-List" > ./../source-linux.sha
PowerShell -NoProfile -NoLogo -ExecutionPolicy unrestricted -Command "Get-Content './../source-linux.sha' | select -First 4 | select -Last 2 > ./../source-linux.txt
del  /Q %cd%\..\source-linux.sha

pause