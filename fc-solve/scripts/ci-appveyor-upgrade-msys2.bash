#! /bin/bash
# Based on https://github.com/msys2/MSYS2-packages/issues/1967 by @mati865

set -e -x

curl -O http://repo.msys2.org/msys/x86_64/msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz
curl -O http://repo.msys2.org/msys/x86_64/msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz.sig

# pacman-key --verify msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz{.sig,}
pacman-key --verify msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz.sig
pacman -U --noconfirm msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz

if false
then
base_url='http://repo.msys2.org/msys/x86_64/'
packages="libzstd-1.4.4-2-x86_64.pkg.tar.xz pacman-5.2.1-6-x86_64.pkg.tar.xz zstd-1.4.4-2-x86_64.pkg.tar.xz"
for p in $packages
do
    curl "${base_url}$p" -o "$p"
done
pacman -U --noconfirm $packages
rm -f $packages
fi
