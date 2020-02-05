#!/bin/bash

### INSTALL GLAD ###

tmp_dir=$(mktemp -d -p .)
cd $tmp_dir
git clone -q https://github.com/Dav1dde/glad.git
python -m glad --generator=c --profile=core --out-path=../lib/GL 
cd ..
rm -rf $tmp_dir

### INSTALL GLFW ###

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    sudo apt install libglfw3
elif [[ "$OSTYPE" == "darwin"* ]]; then
    brew install glfw
else
    echo "GLFW: Sorry os is not supported yet"
fi

### INSTALL GLM ###

if [ ! -d lib/glm ]; then
    git clone -q https://github.com/g-truc/glm.git lib/glm
else
    cd lib/glm && git pull -q && cd -
fi