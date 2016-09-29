#!/bin/sh

if [ ! -f ~/.aaaa/donothexedit.me ]; then
  echo "No config found!"
  if [ ! -d ~/.aaaa ]; then
    echo "Creating directory ~/.aaaa"
    mkdir -p ~/.aaaa/
  else
    echo "Directory exists"
  fi
  echo "Copying new config from OPK"
  cp ./donothexedit.me ~/.aaaa/
fi

echo "Starting AAAA"
env LD_LIBRARY_PATH=. ./aaaa
