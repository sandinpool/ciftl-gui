#pragma once
#include <memory>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <ciftl_core/crypter/crypter.h>
#include <ciftl_core/etc/etc.h>

namespace ciftl
{
    constexpr static size_t SM4_KEY_LENGTH = 16;
    constexpr static size_t SM4_IV_LENGTH = 16;
    constexpr static size_t SM4_BLOCK_LENGTH = 16;

    typedef StreamGeneratorForOpenSSL<SM4_IV_LENGTH, SM4_KEY_LENGTH, SM4_BLOCK_LENGTH> OriginalSM4OpenSSLStreamGenerator;
    class SM4OpenSSLStreamGenerator : public OriginalSM4OpenSSLStreamGenerator
    {
    public:
        typedef typename OriginalSM4OpenSSLStreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalSM4OpenSSLStreamGenerator::KeyByteArray KeyByteArray;

        SM4OpenSSLStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~SM4OpenSSLStreamGenerator() = default;
    };

    typedef StringCrypter<SM4OpenSSLStreamGenerator> SM4OpenSSLStringCrypter;
}
