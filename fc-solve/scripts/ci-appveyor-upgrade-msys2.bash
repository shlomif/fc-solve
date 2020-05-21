#! /bin/bash
# Based on https://github.com/msys2/MSYS2-packages/issues/1967 by @mati865
unrefactored_original()
{
    base_url='http://repo.msys2.org/msys/x86_64'
    curl ${base_url}/libzstd-1.4.4-2-x86_64.pkg.tar.xz -o libzstd-1.4.4-2-x86_64.pkg.tar.xz
    curl ${base_url}/pacman-5.2.1-6-x86_64.pkg.tar.xz -o pacman-5.2.1-6-x86_64.pkg.tar.xz
    curl ${base_url}/zstd-1.4.4-2-x86_64.pkg.tar.xz -o zstd-1.4.4-2-x86_64.pkg.tar.xz
    pacman -U --noconfirm libzstd-1.4.4-2-x86_64.pkg.tar.xz pacman-5.2.1-6-x86_64.pkg.tar.xz zstd-1.4.4-2-x86_64.pkg.tar.xz
    rm libzstd-1.4.4-2-x86_64.pkg.tar.xz pacman-5.2.1-6-x86_64.pkg.tar.xz zstd-1.4.4-2-x86_64.pkg.tar.xz
}
unrefactored_original
exit 0

set -e -x

base_url='http://repo.msys2.org/mingw/x86_64/'
packages="libzstd-1.4.4-2-x86_64.pkg.tar.xz pacman-5.2.1-6-x86_64.pkg.tar.xz zstd-1.4.4-2-x86_64.pkg.tar.xz"
for p in $packages
do
    curl "${base_url}$p" -o "$p"
done
pacman -U --noconfirm $packages
rm -f $packages
