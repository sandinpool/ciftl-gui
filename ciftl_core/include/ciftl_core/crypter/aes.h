#pragma once
#include <memory>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <ciftl_core/crypter/crypter.h>
#include <ciftl_core/etc/etc.h>

namespace ciftl
{
    constexpr static size_t AES128_KEY_LENGTH = 16;
    constexpr static size_t AES128_IV_LENGTH = 16;
    constexpr static size_t AES128_BLOCK_LENGTH = 16;

    typedef StreamGeneratorForOpenSSL<AES128_IV_LENGTH, AES128_KEY_LENGTH, AES128_BLOCK_LENGTH> OriginalAES128OpenSSLStreamGenerator;
    class AES128OpenSSLStreamGenerator : public OriginalAES128OpenSSLStreamGenerator
    {
    public:
        typedef typename OriginalAES128OpenSSLStreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalAES128OpenSSLStreamGenerator::KeyByteArray KeyByteArray;

        AES128OpenSSLStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~AES128OpenSSLStreamGenerator() = default;
    };

    typedef StringCrypter<AES128OpenSSLStreamGenerator> AES128OpenSSLStringCrypter;
}

namespace ciftl
{
    constexpr static size_t AES192_KEY_LENGTH = 24;
    constexpr static size_t AES192_IV_LENGTH = 16;
    constexpr static size_t AES192_BLOCK_LENGTH = 16;

    typedef StreamGeneratorForOpenSSL<AES192_IV_LENGTH, AES192_KEY_LENGTH, AES192_BLOCK_LENGTH> OriginalAES192OpenSSLStreamGenerator;
    class AES192OpenSSLStreamGenerator : public OriginalAES192OpenSSLStreamGenerator
    {
    public:
        typedef typename OriginalAES192OpenSSLStreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalAES192OpenSSLStreamGenerator::KeyByteArray KeyByteArray;

        AES192OpenSSLStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~AES192OpenSSLStreamGenerator() = default;
    };

    typedef StringCrypter<AES192OpenSSLStreamGenerator> AES192OpenSSLStringCrypter;
}

namespace ciftl
{
    constexpr static size_t AES256_KEY_LENGTH = 32;
    constexpr static size_t AES256_IV_LENGTH = 16;
    constexpr static size_t AES256_BLOCK_LENGTH = 16;

    typedef StreamGeneratorForOpenSSL<AES256_IV_LENGTH, AES256_KEY_LENGTH, AES256_BLOCK_LENGTH> OriginalAES256OpenSSLStreamGenerator;
    class AES256OpenSSLStreamGenerator : public OriginalAES256OpenSSLStreamGenerator
    {
    public:
        typedef typename OriginalAES256OpenSSLStreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalAES256OpenSSLStreamGenerator::KeyByteArray KeyByteArray;

        AES256OpenSSLStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~AES256OpenSSLStreamGenerator() = default;
    };

    typedef StringCrypter<AES256OpenSSLStreamGenerator> AES256OpenSSLStringCrypter;
}
