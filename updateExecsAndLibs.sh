#!/bin/bash

cd $(dirname $0)

for i in libs/*; do
  if [[ -d $i && -e $i/wyokiserver ]];then
    echo Moving $i/wyokiserver to $i/libwyokiserver.so;
    mv $i/wyokiserver $i/libwyokiserver.so;
  fi
done
cp -frv nativeMethods/libs/* libs
echo Done.
