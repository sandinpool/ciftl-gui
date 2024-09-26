#pragma once
#include <crc32c/crc32c.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <ciftl_core/crypter/chacha20.h>
#include <ciftl_core/crypter/core_crypter.h>

namespace ciftl
{
    typedef typename ChaCha20OpenSSLStreamGenerator::KeyByteArray KeyByteArray;
    typedef typename ChaCha20OpenSSLStreamGenerator::IVByteArray IVByteArray;

    ChaCha20OpenSSLStreamGenerator::ChaCha20OpenSSLStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalChaCha20OpenSSLStreamGenerator(iv, key, mode)
    {
        if (!EVP_EncryptInit_ex(m_ctx, EVP_chacha20(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
} // namespace ciftl
