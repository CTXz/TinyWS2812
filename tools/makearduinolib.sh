#!/bin/bash

# Platforms
declare -a PLATFORMS=(
        "AVR"
        "ARDUINO_AVR"
)

# cd into project root
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$DIR/.."
cd "$PROJECT_ROOT"

TMP_LIB_DIR="/tmp/TinyWS2812"
rm -rf "$TMP_LIB_DIR"
mkdir "$TMP_LIB_DIR"
cp -r * "$TMP_LIB_DIR/"

cd "$TMP_LIB_DIR"

mv include/* src/
rm -r include

find examples/ -name "*.c" -print0 | while read -d $'\0' file
do
        example_path="$(echo $file | sed 's/\.c//g')"
        mkdir "$example_path" 
        mv "$file" "$example_path/$(basename -- "$file" | sed 's/\.c/.ino/g')"
done

find src/ -name "*.c" -print0 | while read -d $'\0' file
do
        mv "$file" "$(echo $file | sed 's/\.c/.cpp/g')"
done

i=0
for platform in "${PLATFORMS[@]}"; do
        ((i++))
        echo -e "$i. $platform"
done

prompt=true
while "$prompt"; do
        read -r -p "Please select a target platform from the list above (1-$i): " answer
        if (( answer > 0 && answer <= i )); then
                prompt=false
        else
                echo "Invalid input"
        fi
done
((answer--))

sed -i "1s/^/#define WS2812_TARGET_PLATFORM_${PLATFORMS[$answer]}\n/" src/*.h src/*.cpp
sed -i "/name=/s/$/ \(${PLATFORMS[$answer]}\)/" library.properties

cd "$TMP_LIB_DIR/../"
zip -r "$PROJECT_ROOT/TinyWS2812(${PLATFORMS[$answer]}).zip" "TinyWS2812" >> /dev/null 2>&1
rm -r "$TMP_LIB_DIR"

echo "makearduinolib: Arduino library (target platform: ${PLATFORMS[$answer]}) successfully created!"