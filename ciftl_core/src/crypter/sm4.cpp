#include <crc32c/crc32c.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <ciftl_core/crypter/sm4.h>
#include <ciftl_core/crypter/core_crypter.h>

namespace ciftl
{
    std::shared_ptr<SM4StringCrypter> default_sm4_string_crypter()
    {
        return std::make_shared<SM4StringCrypter>();
    }

    typedef typename SM4StreamGenerator::KeyByteArray KeyByteArray;
    typedef typename SM4StreamGenerator::IVByteArray IVByteArray;

    SM4StreamGenerator::SM4StreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalSM4StreamGenerator(iv, key, mode)
    {
        // 初始化加密操作
        if (!EVP_EncryptInit_ex(m_ctx, EVP_sm4_ofb(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
}
