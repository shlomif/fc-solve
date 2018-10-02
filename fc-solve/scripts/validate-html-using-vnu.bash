#!/bin/bash
set -e
echo "1..1"
dir=../dest-xh
rm -fr "$dir"
cp -R ../wml/dest "$dir"
find "$dir" -name '*.html' | grep -v jquery-ui | (while read fn
do
    mv "$fn" "${fn%%.html}.xhtml"
done)
if java -jar build/dist/vnu.jar --Werror --skip-non-html "$dir"
then
    echo "ok 1"
else
    echo "not ok 1"
fi
