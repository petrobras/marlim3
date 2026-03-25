#!/bin/bash

#git diff HEAD
#versao=$(git --git-dir=../.git describe --dirty --always --tags)
data_build=$(date +"%d/%m/%Y %X")
usuario=$(whoami)
versao=$(git --git-dir=../.git describe --abbrev=7 --always --tags 2>&1)

if [ $? -ne 0 ]; then versao=unversioned; fi

cat > ../versao.h << EOF
#define VERSAO "$versao"
#define DATA_BUILD "$data_build"
#define USUARIO "$usuario"
EOF
