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

    typedef StreamGeneratorForOpenSSL<AES128_IV_LENGTH, AES128_KEY_LENGTH, AES128_BLOCK_LENGTH> OriginalAES128StreamGenerator;
    class AES128StreamGenerator : public OriginalAES128StreamGenerator
    {
    public:
        typedef typename OriginalAES128StreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalAES128StreamGenerator::KeyByteArray KeyByteArray;

        AES128StreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~AES128StreamGenerator() = default;
    };

    typedef StringCrypter<AES128StreamGenerator> AES128StringCrypter;
}

namespace ciftl
{
    constexpr static size_t AES192_KEY_LENGTH = 24;
    constexpr static size_t AES192_IV_LENGTH = 16;
    constexpr static size_t AES192_BLOCK_LENGTH = 16;

    typedef StreamGeneratorForOpenSSL<AES192_IV_LENGTH, AES192_KEY_LENGTH, AES192_BLOCK_LENGTH> OriginalAES192StreamGenerator;
    class AES192StreamGenerator : public OriginalAES192StreamGenerator
    {
    public:
        typedef typename OriginalAES192StreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalAES192StreamGenerator::KeyByteArray KeyByteArray;

        AES192StreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~AES192StreamGenerator() = default;
    };

    typedef StringCrypter<AES192StreamGenerator> AES192StringCrypter;
}

namespace ciftl
{
    constexpr static size_t AES256_KEY_LENGTH = 32;
    constexpr static size_t AES256_IV_LENGTH = 16;
    constexpr static size_t AES256_BLOCK_LENGTH = 16;

    typedef StreamGeneratorForOpenSSL<AES256_IV_LENGTH, AES256_KEY_LENGTH, AES256_BLOCK_LENGTH> OriginalAES256StreamGenerator;
    class AES256StreamGenerator : public OriginalAES256StreamGenerator
    {
    public:
        typedef typename OriginalAES256StreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalAES256StreamGenerator::KeyByteArray KeyByteArray;

        AES256StreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~AES256StreamGenerator() = default;
    };

    typedef StringCrypter<AES256StreamGenerator> AES256StringCrypter;
}
