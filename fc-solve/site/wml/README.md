# How to build the fc-solve site.

## Dependencies:

1. [Latemp](https://bitbucket.org/shlomif/latemp) and
[Website Meta Language](https://bitbucket.org/shlomif/website-meta-language) .
2. [Emscripten](https://kripken.github.io/emscripten-site/)
3. [CMake](https://cmake.org/)
4. [GNU Make](https://www.gnu.org/software/make/)
5. Some [CPAN](http://metacpan.org/) Modules.
6. The [TypeScript](http://www.typescriptlang.org/) tsc compiler.
7. Possibly other stuff.

## Build procedure.

1. `./gen-helpers.pl`
2. `make`
3. `make test`
4. `make PROD=1`

## Assets.

The assets (downloads/tarballs/etc.) for this site are kept in a
separate repository: http://github.com/shlomif/fc-solve-site-assets

