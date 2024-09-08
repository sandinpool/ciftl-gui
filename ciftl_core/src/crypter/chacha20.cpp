#pragma once
#include <crc32c/crc32c.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <ciftl_core/crypter/chacha20.h>
#include <ciftl_core/crypter/core_crypter.h>

namespace ciftl
{
    std::shared_ptr<ChaCha20StringCrypter> default_chacha20_string_crypter()
    {
        return std::make_shared<ChaCha20StringCrypter>();
    }

    typedef typename ChaCha20StreamGenerator::KeyByteArray KeyByteArray;
    typedef typename ChaCha20StreamGenerator::IVByteArray IVByteArray;

    ChaCha20StreamGenerator::ChaCha20StreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalChaCha20StreamGenerator(iv, key, mode)
    {
        if (!EVP_EncryptInit_ex(m_ctx, EVP_chacha20(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
} // namespace ciftl
