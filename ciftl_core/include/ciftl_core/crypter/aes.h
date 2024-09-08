#pragma once
#include <memory>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <ciftl_core/crypter/crypter.h>
#include <ciftl_core/etc/etc.h>

namespace ciftl
{
    constexpr static size_t AES_KEY_LENGTH = 16;
    constexpr static size_t AES_IV_LENGTH = 16;
    constexpr static size_t AES_BLOCK_LENGTH = 16;

    typedef StreamGeneratorForOpenSSL<AES_IV_LENGTH, AES_KEY_LENGTH, AES_BLOCK_LENGTH> OriginalAESStreamGenerator;
    class AESStreamGenerator : public OriginalAESStreamGenerator
    {
    public:
        typedef typename OriginalAESStreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalAESStreamGenerator::KeyByteArray KeyByteArray;

        AESStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~AESStreamGenerator() = default;
    };

    typedef StringCrypter<AESStreamGenerator> AESStringCrypter;
    std::shared_ptr<AESStringCrypter> default_aes_string_crypter();
}
