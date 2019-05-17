#!/bin/bash

vsVersions=(vs2015 vs2017 vs2018 gmake)
vsAvailableSDK=(10.0.16 10.0.15)
vsArgumentsAvailable=(--gfxapi --pfv  --with-linux)

function print_available_windows_sdk_versions()
{
  #DIR /b c:/Program Files (x86)/Windows Kits/10/Include
 
  echo "-----------------------------------------------------------"
  echo "        Available windows sdk versions: "
  echo ""
  dir "c:\Program Files (x86)\Windows Kits\10\Include"
  echo "-----------------------------------------------------------"
}

function add_arguments()
{
 for argument in ${vsArgumentsAvailable[*]}
 do
  echo $argument
 done
 
 read -p 'Introduce arguments you need:' arguments
}

function get_visual_version()
{
  
  id=0
  for var in ${vsVersions[*]}
  do
    echo "$id." $var
    let id++
  done
  
 version=-1
  
  while [ $version -gt ${#vsVersions[*]} ] || [ $version -lt 0 ]
  do
    read -p 'Choose number: ' version
  done
  
  system=${vsVersions[$version]}
}

function get_system()
{
  case "$OSTYPE" in
  linux*)
    [ -f genie ] || wget https://github.com/bkaradzic/bx/raw/master/tools/bin/linux/genie
    chmod +x genie
    system=ninja
    ./genie.exe $arguments $system

    ;;
  darwin*)
    [ -f genie ] || wget https://github.com/bkaradzic/bx/raw/master/tools/bin/darwin/genie
    chmod +x genie
    system=xcode
    ./genie.exe $arguments $system

    ;;
  *)
  echo "windows"
    [ -f genie.exe ] || wget https://github.com/bkaradzic/bx/raw/master/tools/bin/windows/genie.exe
    chmod +x genie.exe
    print_available_windows_sdk_versions
    get_visual_version
    add_arguments
    ./genie.exe $arguments $system
    
    ;;
  *)
    ;;
esac
}



mkdir -p build

get_system
echo ./genie.exe $arguments $system



echo ""
echo "-------------------------------------------------------"
echo "                  Project generated"
echo "-------------------------------------------------------"
echo ""

#rm -f genie.exe