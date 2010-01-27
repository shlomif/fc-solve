#!/bin/bash

STABLE_VERSION_DIR="$(bash scripts/get-stable-version-dir.sh)"
VERSION="$(bash get-recent-stable-version.sh)"
BASE_DIR="freecell-solver-$VERSION"

zip_file="$(pwd)/src/win32_build/dynamic/fcs.zip"

(cd "${STABLE_VERSION_DIR}" &&
    bash prepare_package.sh
    tar -xzvf "${BASE_DIR}.tar.gz"
    (cd "${BASE_DIR}" &&
        zip -r "$zip_file" *
    )
)

