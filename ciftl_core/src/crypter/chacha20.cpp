#pragma once
#include <crc32c/crc32c.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <ciftl_core/crypter/chacha20.h>
#include <ciftl_core/crypter/core_crypter.h>

namespace ciftl
{
    ChaCha20OpenSSLStreamGenerator::ChaCha20OpenSSLStreamGenerator(const ChaCha20OpenSSLStreamGenerator::IVByteArray &iv,
                                                                   const ChaCha20OpenSSLStreamGenerator::KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalChaCha20OpenSSLStreamGenerator(iv, key, mode)
    {
        if (!EVP_EncryptInit_ex(m_ctx, EVP_chacha20(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }


} // namespace ciftl
