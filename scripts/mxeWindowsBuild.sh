#!/bin/bash

# Assumes, that you have installed qt5, libopenal and libvorbisfile via MXE.

MXE_QT5_QMAKE=~/mxe/usr/i686-w64-mingw32.static/qt5/bin/qmake

ZIP=zip
if ! which $ZIP; then
    echo "$ZIP not found."
    exit -1
fi

UNIX2DOS=unix2dos
if ! which $UNIX2DOS; then
    echo "$UNIX2DOS not found."
    exit -1
fi

MAKENSIS=makensis
if ! which $MAKENSIS; then
    echo "$MAKENSIS not found."
    exit -1
fi

LRELEASE=lrelease
if ! which $LRELEASE; then
    echo "$LRELEASE not found."
    exit -1
fi

NUM_CPUS=$(cat /proc/cpuinfo | grep processor | wc -l)

# Package naming

NAME=dustrac
VERSION=${DUSTRAC_RELEASE_VERSION?"is not set."}
ARCH=windows-x86
QT=qt5

# Build

$MXE_QT5_QMAKE && make -j$NUM_CPUS || exit -1

# Update translations

$LRELEASE ./src/game/game.pro && $LRELEASE ./src/editor/editor.pro || exit -1

# Install to packaging dir

PACKAGE_PATH=$NAME-$VERSION-$ARCH-$QT
rm -rf $PACKAGE_PATH
mkdir $PACKAGE_PATH

mkdir -p data/translations
cp -v ./src/game/translations/*.qm data/translations        &&
cp -v ./src/editor/translations/*.qm data/translations      &&
cp -v ./src/game/release/dustrac-game.exe $PACKAGE_PATH     &&
cp -v ./src/editor/release/dustrac-editor.exe $PACKAGE_PATH &&
cp -rv data $PACKAGE_PATH || exit -1

TEXT_FILES="AUTHORS CHANGELOG COPYING README"
cp -v $TEXT_FILES $PACKAGE_PATH || exit -1
cd $PACKAGE_PATH
$UNIX2DOS $TEXT_FILES
cd ..

# Create zip archive

ZIP_ARCHIVE=$PACKAGE_PATH.zip
rm -f $ZIP_ARCHIVE
$ZIP -rv $ZIP_ARCHIVE $PACKAGE_PATH

# Create NSIS installer

cp packaging/windows/dustrac.nsi $PACKAGE_PATH || exit -1
cd $PACKAGE_PATH
$MAKENSIS dustrac.nsi
cd ..
cp $PACKAGE_PATH/*setup.exe . || exit -1

ls -ltr

echo "Done."

