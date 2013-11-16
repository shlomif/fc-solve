/*
 * This Base64 implementation is adapted from:
 *
 * http://stackoverflow.com/a/6782480/7709
 *
 * */

#ifndef FC_SOLVE__FCS_BASE64_H
#define FC_SOLVE__FCS_BASE64_H

#include <stdint.h>
#include <stdlib.h>

/* We modified the encoding table to have '_' instead of '/',
 * which interferes with UNIX paths.
 * */

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '_'};
static char decoding_table[256];
static int mod_table[] = {0, 2, 1};

static void build_decoding_table(void)
{
    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

static void base64_encode(
    const unsigned char *data,
    size_t input_length,
    char * encoded_data, /* Should have enough space, we got rid of
    malloc()s to avoid memory fragmentation. */
    size_t *output_length
)
{

    *output_length = 4 * ((input_length + 2) / 3);

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    encoded_data[*output_length] = '\0';

    return;
}


static int base64_decode(
    const char *data,
    size_t input_length,
    unsigned char * decoded_data,
    size_t *output_length
)
{

    if (input_length % 4 != 0) return -1;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    for (int i = 0, j = 0; i < input_length;) {

#define DECODE() ( data[i] == '=' ? 0 & i++ : decoding_table[(size_t)(unsigned char)(data[i++])] )

        uint32_t sextet_a = DECODE();
        uint32_t sextet_b = DECODE();
        uint32_t sextet_c = DECODE();
        uint32_t sextet_d = DECODE();
#undef DECODE

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return 0;
}

#endif /* FC_SOLVE__FCS_BASE64_H */

