#!/bin/bash

#################################################################################
# 
# Copyright (c) 2009, Ron Iovine, All rights reserved.  
#  
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Ron Iovine nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY Ron Iovine ''AS IS'' AND ANY EXPRESS OR 
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
# IN NO EVENT SHALL Ron Iovine BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE. 
#
#################################################################################

if [ $# -ge 1 ]
then
  if [ "$1" != "-l" -o $# -gt 2 ]
  then
    echo
    echo "Usage: install.sh [-l [<shellEnvFile>]]"
    echo
    echo "  This install script will either install this package on a system"
    echo "  wide basis or will setup a local install environment.  A system"
    echo "  install must be done as 'root' and will copy all libraries, binaries,"
    echo "  include files, conf files, and manpages to their customary system"
    echo "  locations.  A local install will not copy/move any files.  It will"
    echo "  only setup a local environment by writing environment variables into"
    echo "  your shell env file (i.e. .bashrc) that will allow use of the package"
    echo "  features from a local directory."
    echo
    echo "  where:"
    echo "    -l           - install locally, omit for system install"
    echo "    shellEnvFile - name of shell environment file, defaults to .bashrc"
    echo
    exit 1
  else
    # local install
    if [ $# -eq 2 ]
    then
      shellEnvFile=$2
    else
      shellEnvFile="/home/$(whoami)/.bashrc"
    fi
    if [ ! -e "$shellEnvFile" ]
    then
      echo "ERROR: Shell env file: $shellEnvFile, not found, enter desired shellEnvFile"
      exit 1
    fi    
    localDir=$(pwd)
    if [ ! -d "$localDir" ]
    then
      echo "ERROR: Local directory: $localDir, not found, enter valid directory"
      exit 1
    fi    
    if [ ! -e "$localDir/install.sh" ]
    then
      echo "ERROR: This script must be run from same directory where it resides"
      exit 1
    fi
    if [ -e "$PSHELL_INSTALL" ]
    then
      if [ "$PSHELL_INSTALL" == "$localDir" ]
      then
        echo "INFO: Local install environment: $localDir already setup"
        exit 1
      else
        while true; do
          read -p "INFO: Replace existing install environment: $PSHELL_INSTALL with: $localDir? [y/n]: " yn
          case $yn in
            [Yy]* ) break;;
            [Nn]* ) exit;;
            * ) echo "Please answer yes or no.";;
          esac
        done
        echo "INFO: May need to prune env file: $shellEnvFile of redundant environment variables"
      fi
    fi    
    echo export PSHELL_INSTALL=$localDir >> $shellEnvFile
    echo export PSHELL_BATCH_DIR=\$PSHELL_INSTALL/batch >> $shellEnvFile
    echo export PSHELL_CONFIG_DIR=\$PSHELL_INSTALL/config >> $shellEnvFile
    echo export PATH=\$PSHELL_INSTALL/bin:\$PSHELL_INSTALL/utils:\$PATH >> $shellEnvFile
    echo export MANPATH=\$PSHELL_INSTALL/man:\$MANPATH >> $shellEnvFile
    echo export LD_LIBRARY_PATH=\$PSHELL_INSTALL/lib:\$LD_LIBRARY_PATH >> $shellEnvFile
    echo "INFO: Local install environment setup as: $localDir"
    echo "      Re-source your enviromnent with 'source $shellEnvFile'"
  fi
else
  # system wide install
  if [ $(whoami) != "root" ]
  then
    echo "ERROR: Must be 'root' to do system install of this package"
    exit 1
  fi

  if [ ! -e "$(pwd)/install.sh" ]
  then
    echo "ERROR: This script must be run from same directory where it resides"
    exit 1
  fi

  binDir="/usr/bin"
  includeDir="/usr/include"
  confDir="/etc/pshell"
  man1Dir="/usr/share/man/man1"
  man3Dir="/usr/share/man/man3"
  if [ -d "/usr/lib64" ]
  then
    libDir="/usr/lib64"
  else
    libDir="/usr/lib"
  fi
  echo "Installing pshell files..."
  echo "Installing libs..."
  if [ ! -d $libDir/pshell ]
  then
    echo "Creating lib directory: $libDir"
    mkdir $libDir/pshell
  fi
  echo "Copying libpshell-server-full.so to $libDir/pshell"
  cp lib/libpshell-server-full.so $libDir/pshell/.
  echo "Copying libpshell-server-full.a to $libDir/pshell"
  cp lib/libpshell-server-full.a $libDir/pshell/.
  echo "Copying libpshell-server-stub.so to $libDir/pshell"
  cp lib/libpshell-server-stub.so $libDir/pshell/.
  echo "Copying libpshell-server-stub.a to $libDir/pshell"
  cp lib/libpshell-server-stub.a $libDir/pshell/.
  echo "Copying libpshell-control.so to $libDir/pshell"
  cp lib/libpshell-control.so $libDir/pshell/.
  echo "Copying libpshell-control.a to $libDir/pshell"
  cp lib/libpshell-control.a $libDir/pshell/.
  echo "Setting softlink libpshell-server.so to pshell/libpshell-server-full.so"
  if [ -e "$libDir/libpshell-server.so" ]
  then
    rm -f $libDir/libpshell-server.so
  fi
  ln -s $libDir/pshell/libpshell-server-full.so $libDir/libpshell-server.so
  echo "Setting softlink libpshell-control.so to pshell/libpshell-control.so"
  if [ -e "$libDir/libpshell-control.so" ]
  then
    rm -f $libDir/libpshell-control.so
  fi
  ln -s $libDir/pshell/libpshell-control.so $libDir/libpshell-control.so
  echo "Installing bins..."
  echo "Copying pshell to $binDir"
  cp bin/pshell $binDir/.
  chmod +x $binDir/pshell
  echo "Installing includes..."
  echo "Copying PshellServer.h to $includeDir"
  cp include/PshellServer.h $includeDir/.
  echo "Copying PshellControl.h to $includeDir"
  cp include/PshellControl.h $includeDir/.
  echo "Copying TraceFilter.h to $includeDir"
  cp include/TraceFilter.h $includeDir/.
  echo "Copying TraceLog.h to $includeDir"
  cp include/TraceLog.h $includeDir/.
  echo "Installing conf files..."
  if [ ! -d $confDir ]
  then
    echo "Creating conf directory: $confDir"
    mkdir $confDir
  fi
  echo "Copying pshell-server.conf to $confDir"
  cp config/pshell-server.conf $confDir/.
  echo "Copying pshell-client.conf to $confDir"
  cp config/pshell-client.conf $confDir/.
  echo "Copying pshell-control.conf to $confDir"
  cp config/pshell-control.conf $confDir/.
  chmod 666 $confDir/*
  echo "Installing manpages..."
  echo "Copying pshell.1 to $man1Dir"
  cp man/man1/pshell.1 $man1Dir/.
  gzip -f $man1Dir/pshell.1
  echo "Copying PshellControl.3 to $man3Dir"
  cp man/man3/PshellControl.3 $man3Dir/.
  gzip -f $man3Dir/PshellControl.3
  echo "Copying PshellServer.3 to $man3Dir"
  cp man/man3/PshellServer.3 $man3Dir/.
  gzip -f $man3Dir/PshellServer.3
  echo "Copying TraceFilter.3 to $man3Dir"
  cp man/man3/TraceFilter.3 $man3Dir/.
  gzip -f $man3Dir/TraceFilter.3
fi