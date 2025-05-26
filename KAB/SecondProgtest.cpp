#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

struct crypto_config
{
    const char * m_crypto_function;
    std::unique_ptr<uint8_t[]> m_key;
    std::unique_ptr<uint8_t[]> m_IV;
    size_t m_key_len;
    size_t m_IV_len;
};

#endif /* _PROGTEST_ */

bool open_files(const string& in_filename, const string& out_filename, FILE** in_file, FILE** out_file) {
    *in_file = fopen(in_filename.c_str(), "rb");
    if (!*in_file) return false;

    *out_file = fopen(out_filename.c_str(), "wb");
    if (!*out_file) {
        fclose(*in_file);
        return false;
    }
    return true;
}

bool initciphCTX(EVP_CIPHER_CTX** ctx, const EVP_CIPHER* ciph, crypto_config& config) {
    *ctx = EVP_CIPHER_CTX_new();
    if (!*ctx) return false;

    bool first = EVP_EncryptInit_ex(*ctx, ciph, nullptr, config.m_key.get(), config.m_IV.get());
    if(!first){
        return false;
    }
    bool init_result = first;
    return init_result;
}
bool decrciphCTX(EVP_CIPHER_CTX** ctx, const EVP_CIPHER* ciph, crypto_config& config) {
    *ctx = EVP_CIPHER_CTX_new();
    if (!*ctx) return false;

    bool second = EVP_DecryptInit_ex(*ctx, ciph, nullptr, config.m_key.get(), config.m_IV.get());
    if(!second){
        return false;
    }
    bool init_result = second;
    return init_result;
}

void clean(FILE* in_file, FILE* out_file, EVP_CIPHER_CTX* ctx) {
    fclose(in_file);
    fclose(out_file);
    EVP_CIPHER_CTX_free(ctx);
}

bool pr_head(FILE* in_file, FILE* out_file) {
    unsigned char header[18];
    if (fread(header, 1, 18, in_file) != 18) return false;
    if (fwrite(header, 1, 18, out_file) != 18) return false;
    return true;
}

bool configure_crypto_parameters(crypto_config &config, const EVP_CIPHER *cipher, bool isEncryption) {
    size_t required_key_length = EVP_CIPHER_key_length(cipher);

    size_t required_iv_length = EVP_CIPHER_iv_length(cipher);
    if (!isEncryption) {
        if (config.m_key == nullptr || config.m_key_len < required_key_length ||
            (required_iv_length != 0 && (config.m_IV == nullptr || config.m_IV_len < required_iv_length))) {
            return false;
        }
    }
    if (config.m_key == nullptr || config.m_key_len < required_key_length) {
        config.m_key.reset(new uint8_t[required_key_length]);
        RAND_bytes(config.m_key.get(), required_key_length);
        config.m_key_len = required_key_length;
    }

    if (required_iv_length != 0) { // Some ciphers do not use an IV.
        if (config.m_IV == nullptr || config.m_IV_len < required_iv_length) {
            config.m_IV.reset(new uint8_t[required_iv_length]);
            RAND_bytes(config.m_IV.get(), required_iv_length);
            config.m_IV_len = required_iv_length;
        }
    }


    return true;
}

bool encrypt_data(const string &in_filename, const string &out_filename, crypto_config &config) {
    FILE *in_file, *out_file;
    EVP_CIPHER_CTX *ctx;

    const EVP_CIPHER *ciph = EVP_get_cipherbyname(config.m_crypto_function);
    if (!ciph) return false;

    if (!open_files(in_filename, out_filename, &in_file, &out_file)) return false;


    if (!configure_crypto_parameters(config, ciph, true)) {
        clean(in_file, out_file, nullptr);
        return false;
    }

    if (!initciphCTX(&ctx, ciph, config) || !pr_head(in_file, out_file)) {
        clean(in_file, out_file, ctx);
        return false;
    }

    const unsigned int my_buf = 4096;
    unsigned char input_Buf[my_buf];
    unsigned char output_Buf[my_buf + EVP_MAX_BLOCK_LENGTH];
    int outLen;

    while (true) {
        size_t read_bytes = fread(input_Buf, 1, my_buf, in_file);
        if (read_bytes == 0) break;

        if (!EVP_EncryptUpdate(ctx, output_Buf, &outLen, input_Buf, read_bytes)) {
            clean(in_file, out_file, ctx);
            return false;
        }
        if (fwrite(output_Buf, 1, outLen, out_file) != static_cast<size_t>(outLen)) {
            clean(in_file, out_file, ctx);
            return false;
        }
    }

    if (!EVP_EncryptFinal_ex(ctx, output_Buf, &outLen)) {
        clean(in_file, out_file, ctx);
        return false;
    }
    if (fwrite(output_Buf, 1, outLen, out_file) != static_cast<size_t>(outLen)) {
        clean(in_file, out_file, ctx);
        return false;
    }

    clean(in_file, out_file, ctx);
    return true;
}
bool decrypt_data(const string &in_filename, const string &out_filename, crypto_config &config) {
    FILE *in_file, *out_file;
    EVP_CIPHER_CTX *ctx;
    const EVP_CIPHER *ciph = EVP_get_cipherbyname(config.m_crypto_function);
    if (!ciph) return false;

    if (!open_files(in_filename, out_filename, &in_file, &out_file)) return false;


    if (!configure_crypto_parameters(config, ciph, false)) {
        clean(in_file, out_file, nullptr);
        return false;
    }

    if (!decrciphCTX(&ctx, ciph, config) || !pr_head(in_file, out_file)) {
        clean(in_file, out_file, ctx);
        return false;
    }

    const unsigned int my_buf = 4096;
    unsigned char input_Buf[my_buf];
    unsigned char output_Buf[my_buf + EVP_MAX_BLOCK_LENGTH];
    int outLen;

    while (true) {
        size_t read_bytes = fread(input_Buf, 1, my_buf, in_file);
        if (read_bytes == 0) break;

        if (!EVP_DecryptUpdate(ctx, output_Buf, &outLen, input_Buf, read_bytes)) {
            clean(in_file, out_file, ctx);
            return false;
        }
        if (fwrite(output_Buf, 1, outLen, out_file) != static_cast<size_t>(outLen)) {
            clean(in_file, out_file, ctx);
            return false;
        }
    }

    if (!EVP_DecryptFinal_ex(ctx, output_Buf, &outLen)) {
        clean(in_file, out_file, ctx);
        return false;
    }
    if (fwrite(output_Buf, 1, outLen, out_file) != static_cast<size_t>(outLen)) {
        clean(in_file, out_file, ctx);
        return false;
    }

    clean(in_file, out_file, ctx);
    return true;
}


#ifndef __PROGTEST__

bool compare_files(const std::string &file1, const std::string &file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);
    if (!f1 || !f2) {
        std::cerr << "Failed to open files for comparison." << std::endl;
        return false;
    }

    std::istreambuf_iterator<char> it1(f1), it2(f2), end;
    return std::equal(it1, end, it2);
}

int main ( void )
{
    crypto_config config {nullptr, nullptr, nullptr, 0, 0};

    config.m_crypto_function = "AES-128-ECB";
    config.m_key = std::make_unique<uint8_t[]>(16);
    memset(config.m_key.get(), 0, 16);
    config.m_key_len = 16;

    assert( encrypt_data  ("homer-simpson.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "homer-simpson_enc_ecb.TGA") );

    assert( decrypt_data  ("homer-simpson_enc_ecb.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "homer-simpson.TGA") );

    assert( encrypt_data  ("UCM8.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "UCM8_enc_ecb.TGA") );

    assert( decrypt_data  ("UCM8_enc_ecb.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "UCM8.TGA") );

    assert( encrypt_data  ("image_1.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "ref_1_enc_ecb.TGA") );

    assert( encrypt_data  ("image_2.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "ref_2_enc_ecb.TGA") );

    assert( decrypt_data ("image_3_enc_ecb.TGA", "out_file.TGA", config)  &&
            compare_files("out_file.TGA", "ref_3_dec_ecb.TGA") );

    assert( decrypt_data ("image_4_enc_ecb.TGA", "out_file.TGA", config)  &&
            compare_files("out_file.TGA", "ref_4_dec_ecb.TGA") );

    config.m_crypto_function = "AES-128-CBC";
    config.m_IV = std::make_unique<uint8_t[]>(16);
    config.m_IV_len = 16;
    memset(config.m_IV.get(), 0, 16);

    assert( encrypt_data  ("UCM8.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "UCM8_enc_cbc.TGA") );

    assert( decrypt_data  ("UCM8_enc_cbc.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "UCM8.TGA") );

    assert( encrypt_data  ("homer-simpson.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "homer-simpson_enc_cbc.TGA") );

    assert( decrypt_data  ("homer-simpson_enc_cbc.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "homer-simpson.TGA") );

    assert( encrypt_data  ("image_1.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "ref_5_enc_cbc.TGA") );

    assert( encrypt_data  ("image_2.TGA", "out_file.TGA", config) &&
            compare_files ("out_file.TGA", "ref_6_enc_cbc.TGA") );

    assert( decrypt_data ("image_7_enc_cbc.TGA", "out_file.TGA", config)  &&
            compare_files("out_file.TGA", "ref_7_dec_cbc.TGA") );

    assert( decrypt_data ("image_8_enc_cbc.TGA", "out_file.TGA", config)  &&
            compare_files("out_file.TGA", "ref_8_dec_cbc.TGA") );
    return 0;
}

#endif /* _PROGTEST_ */
