#ifndef __PROGTEST__
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

#endif /* __PROGTEST__ */

string bite_to_Hex(const unsigned char *data, size_t len) {
    static const char hexDigits[] = "0123456789abcdef";
    string hexStr;
    for (size_t i = 0; i < len; ++i) {
        hexStr += hexDigits[data[i] >> 4];
        hexStr += hexDigits[data[i] & 0x0F];
    }
    return hexStr;
}

bool is_Valid_Hash(const string &hex_Hash, int zeroBits) {
    for (int i = 0; i < zeroBits / 4; ++i) {
        if (hex_Hash[i] != '0')
            return false;
    }
    if (zeroBits % 4 != 0) {
        int required_Zeros = 4 - (zeroBits % 4);
        int value = (hex_Hash[zeroBits / 4] >= 'a') ? hex_Hash[zeroBits / 4] - 'a' + 10 : hex_Hash[zeroBits / 4] - '0';
        if (value >= (1 << required_Zeros))
            return false;
    }
    return true;
}

int findHash(int numberZeroBits, string &outputMessage, string &outputHash) {
    if (numberZeroBits < 0 || numberZeroBits > 512)
        return 0;
    unsigned char data[16];
    unsigned int m_hash_length;
    unsigned char m_hash_buffer[EVP_MAX_MD_SIZE];

    const EVP_MD *md = EVP_get_digestbyname("sha512");
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    while (true) {
        RAND_bytes(data, sizeof(data));
        outputMessage = bite_to_Hex(data, sizeof(data));

        if (!EVP_DigestInit_ex(ctx, md, NULL) ||
            !EVP_DigestUpdate(ctx, data, sizeof(data)) ||
            !EVP_DigestFinal_ex(ctx, m_hash_buffer, &m_hash_length)) {
            EVP_MD_CTX_free(ctx);
            return 0;
        }

        outputHash = bite_to_Hex(m_hash_buffer, m_hash_length);
        if (is_Valid_Hash(outputHash, numberZeroBits)) {
            break;
        }
    }

    EVP_MD_CTX_free(ctx);
    return 1;
}

int findHashEx (int numberZeroBits, string & outputMessage, string & outputHash, string_view hashType) {
    /* TODO or use dummy implementation */
    return 1;
}

#ifndef __PROGTEST__


int main (void) {

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

