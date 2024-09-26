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

    typedef StreamGeneratorForOpenSSL<CHACHA20_IV_LENGTH, CHACHA20_KEY_LENGTH> OriginalChaCha20OpenSSLStreamGenerator;
    class ChaCha20OpenSSLStreamGenerator final : public OriginalChaCha20OpenSSLStreamGenerator
    {
    public:
        typedef typename OriginalChaCha20OpenSSLStreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalChaCha20OpenSSLStreamGenerator::KeyByteArray KeyByteArray;

        ChaCha20OpenSSLStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode = StreamGeneratorMode::Medium);

        ~ChaCha20OpenSSLStreamGenerator() = default;

    };

    typedef StringCrypter<ChaCha20OpenSSLStreamGenerator> ChaCha20OpenSSLStringCrypter;
}
