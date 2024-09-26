#include <crc32c/crc32c.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <ciftl_core/crypter/sm4.h>
#include <ciftl_core/crypter/core_crypter.h>

namespace ciftl
{
    typedef typename SM4OpenSSLStreamGenerator::KeyByteArray KeyByteArray;
    typedef typename SM4OpenSSLStreamGenerator::IVByteArray IVByteArray;

    SM4OpenSSLStreamGenerator::SM4OpenSSLStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalSM4OpenSSLStreamGenerator(iv, key, mode)
    {
        // 初始化加密操作
        if (!EVP_EncryptInit_ex(m_ctx, EVP_sm4_ofb(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
}
