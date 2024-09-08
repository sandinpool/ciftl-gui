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

    typedef StreamGeneratorForOpenSSL<SM4_IV_LENGTH, SM4_KEY_LENGTH, SM4_BLOCK_LENGTH> OriginalSM4StreamGenerator;
    class SM4StreamGenerator : public OriginalSM4StreamGenerator
    {
    public:
        typedef typename OriginalSM4StreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalSM4StreamGenerator::KeyByteArray KeyByteArray;

        SM4StreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~SM4StreamGenerator() = default;
    };

    typedef StringCrypter<SM4StreamGenerator> SM4StringCrypter;
    std::shared_ptr<SM4StringCrypter> default_sm4_string_crypter();
}
