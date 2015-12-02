Homepage: http://asprintf.insanecoding.org/
Articles: http://insanecoding.blogspot.com/

---------------------------------------
Files:
---------------------------------------
asprintf.h - Header file needed to use asprintf() and vasprintf()
asprintf.c - Source for asprint() which wraps to vasprintf(), using one of the implementations below
test.c - Some simple usage examples with asprintf()
vasprintf-c99.c - Straightforward C99 implementation of vasprintf()
vasprintf-c99-loop.c - C99 implementation of vasprintf() that can work as well as can be expected with other threads changing variables
vasprintf-msvc.c - Implementation which works with Microsoft Visual C++ and other Windows compilers
vasprintf-msvc-nofuture.c - Implementation specifically designed for the current incorrectness in Microsoft Visual C++ libraries
vasprintf-novacopy.c - Implementation of vasprintf() for systems without va_copy(), that use simple va_list's and have a proper vsnprintf() implementation
---------------------------------------
Compatability:
---------------------------------------
All these implementations require that an implementation of vsnprintf() be provided.

vasprintf-c99.c - Should work just about everywhere these days except with MSVC if you're not using the latest version. This works with older versions of MSVC too if the commented out definition of va_copy at the top of it is no longer commented out.

vasprintf-c99-loop.c - Should work just about everywhere that the above works, if the vsnprintf() implementation is fully compliant. Since on Windows it's not, use something else, or get a compliant vsnprintf() function. Certain MinGW compilers will replace vsnprintf() with a standards compliant one if __USE_MINGW_ANSI_STDIO is defined before stdio.h is included.

vasprintf-msvc.c - Uses the Windows only _vscprintf() function, and expects that va_list's do not require deep copying.

vasprintf-msvc-nofuture.c - Like the above, but drops some overhead of future-proofing a situation that Microsoft may one day make vsnprintf() standards compliant. Do not use this with MinGW with __USE_MINGW_ANSI_STDIO, as this combination means the future is already here.

vasprintf-novacopy.c - Should work just about everywhere. Note however that while the use of vsnprintf() here works on Windows, according to MSDN, it shouldn't.
---------------------------------------
