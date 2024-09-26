#include <crc32c/crc32c.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <ciftl_core/crypter/aes.h>
#include <ciftl_core/crypter/core_crypter.h>

namespace ciftl
{
    AES128OpenSSLStreamGenerator::AES128OpenSSLStreamGenerator(const AES128OpenSSLStreamGenerator::IVByteArray &iv, const AES128OpenSSLStreamGenerator::KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalAES128OpenSSLStreamGenerator(iv, key, mode)
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
    AES192OpenSSLStreamGenerator::AES192OpenSSLStreamGenerator(const AES192OpenSSLStreamGenerator::IVByteArray &iv, const AES192OpenSSLStreamGenerator::KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalAES192OpenSSLStreamGenerator(iv, key, mode)
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
    AES256OpenSSLStreamGenerator::AES256OpenSSLStreamGenerator(const AES256OpenSSLStreamGenerator::IVByteArray &iv, const AES256OpenSSLStreamGenerator::KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalAES256OpenSSLStreamGenerator(iv, key, mode)
    {
        // 初始化加密操作
        if (!EVP_EncryptInit_ex(m_ctx, EVP_aes_256_ofb(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
}
