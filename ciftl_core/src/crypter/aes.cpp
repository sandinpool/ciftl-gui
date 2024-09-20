#include <crc32c/crc32c.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <ciftl_core/crypter/aes.h>
#include <ciftl_core/crypter/core_crypter.h>

namespace ciftl
{
    AES128StreamGenerator::AES128StreamGenerator(const AES128StreamGenerator::IVByteArray &iv, const AES128StreamGenerator::KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalAES128StreamGenerator(iv, key, mode)
    {
        // 初始化加密操作
        if (!EVP_EncryptInit_ex(m_ctx, EVP_aes_128_ofb(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
}

namespace ciftl
{
    AES192StreamGenerator::AES192StreamGenerator(const AES192StreamGenerator::IVByteArray &iv, const AES192StreamGenerator::KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalAES192StreamGenerator(iv, key, mode)
    {
        // 初始化加密操作
        if (!EVP_EncryptInit_ex(m_ctx, EVP_aes_192_ofb(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
}

namespace ciftl
{
    AES256StreamGenerator::AES256StreamGenerator(const AES256StreamGenerator::IVByteArray &iv, const AES256StreamGenerator::KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalAES256StreamGenerator(iv, key, mode)
    {
        // 初始化加密操作
        if (!EVP_EncryptInit_ex(m_ctx, EVP_aes_256_ofb(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
}
