#pragma once
#include <memory>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <ciftl_core/crypter/crypter.h>
#include <ciftl_core/etc/etc.h>

namespace ciftl
{
    constexpr static size_t CHACHA20_KEY_LENGTH = 32;
    constexpr static size_t CHACHA20_IV_LENGTH = 12;

    typedef StreamGeneratorForOpenSSL<CHACHA20_IV_LENGTH, CHACHA20_KEY_LENGTH> OriginalChaCha20StreamGenerator;
    class ChaCha20StreamGenerator final : public OriginalChaCha20StreamGenerator
    {
    public:
        typedef typename OriginalChaCha20StreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalChaCha20StreamGenerator::KeyByteArray KeyByteArray;

        ChaCha20StreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~ChaCha20StreamGenerator() = default;

    };

    typedef StringCrypter<ChaCha20StreamGenerator> ChaCha20StringCrypter;
    std::shared_ptr<ChaCha20StringCrypter> default_chacha20_string_crypter();
}
