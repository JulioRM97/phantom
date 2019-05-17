@echo off
setlocal enabledelayedexpansion

if EXIST bin (
  del /q bin
)

mkdir bin

for /f %%G in ('dir /b "Phantom\assets\shaders"') do (
  
  set outname=CLog_%%~nG.txt
  echo !outname!
  
  set filename = %%~nG
  set filextension = %%~xG
  
  if /i "%%~xG"== ".frag" (
  
   echo ---------------------------------- >> bin/!outname!
   echo [Compiling frag shader for %%~nG] >> bin/!outname!
   
   set frag=%%~nG_frag.spv
   external\VulkanSDK\windows\1.1.97.0\Bin\glslangValidator.exe -V -v -o !frag! Phantom\assets\shaders\%%G >> bin/!outname!
   
   
   echo [Finished compiling frag shader] >> bin/!outname!
   echo ---------------------------------- >> bin/!outname!
   
   move /Y !frag! bin >nul
   
  ) ELSE (
  
   echo [Compiling vert shader for %%~nG] >> bin/!outname!
   
   set vert=%%~nG_vert.spv
   external\VulkanSDK\windows\1.1.97.0\Bin\glslangValidator.exe -V -v -o !vert! Phantom\assets\shaders\%%G >> bin/!outname!
   
   echo [Finished compiling vert shader] >> bin/!outname!
   echo ---------------------------------- >> bin/!outname!
   move /Y !vert! bin >nul
   
  )
  
)
pause