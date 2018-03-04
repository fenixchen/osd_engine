set DIST=build\exe.win-amd64-3.6
rd %DIST% /s /q
set TK_LIBRARY=D:\Python36\tcl\tk8.6
set TCL_LIBRARY=D:\Python36\tcl\tcl8.6
d:\python36\python.exe setup.py build
xcopy freetype.dll %DIST%\ /y
xcopy *.ttf %DIST%\ /y
xcopy *.bmp %DIST%\ /y
xcopy D:\Python36\DLLs\tcl*.dll %DIST%\ /y
xcopy D:\Python36\DLLs\tk*.dll %DIST%\ /y
md %DIST%\jpg
xcopy jpg\* %DIST%\jpg\ /y