/* REFERENCE: https://stackoverflow.com/questions/1537964/visual-c-equivalent-of-gccs-attribute-packed */

#undef PACKED
#if defined(__GNUC__)
#define PACKED() __attribute__ ((__packed__))
#elif defined(_MSC_VER)
#define PACKED()
#pragma __pragma(pack(push, 1))
#endif

