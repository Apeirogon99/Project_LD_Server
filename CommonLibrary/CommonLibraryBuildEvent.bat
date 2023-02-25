@echo off
pushd %~dp0
SET COMMON_PLUGIN_PATH=P:\Project_LD\Plugins\CommonLibrary\Source\ThirdParty\CommonLibrary

rem copy include header files
XCOPY /F "./*.h" "%COMMON_PLUGIN_PATH%\include" /Y
XCOPY /F "./*.h" ".\..\Builds\include\common" /Y
IF ERRORLEVEL 1 PAUSE

rem copy lib files
XCOPY ".\..\Builds\release\CommonLibrary.lib" "%COMMON_PLUGIN_PATH%\release\lib" /Y
XCOPY ".\..\Builds\release\CommonLibrary.pdb" "%COMMON_PLUGIN_PATH%\release\bin" /Y
						   
XCOPY ".\..\Builds\debug\CommonLibrary.lib" "%COMMON_PLUGIN_PATH%\debug\lib" /Y
XCOPY ".\..\Builds\debug\CommonLibrary.pdb" "%COMMON_PLUGIN_PATH%\debug\bin" /Y
XCOPY ".\..\Builds\debug\CommonLibrary.idb" "%COMMON_PLUGIN_PATH%\debug\bin" /Y
IF ERRORLEVEL 1 PAUSE