#include <crc32c/crc32c.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <ciftl_core/crypter/aes.h>
#include <ciftl_core/crypter/core_crypter.h>

namespace ciftl
{
    std::shared_ptr<AESStringCrypter> default_aes_string_crypter()
    {
        return std::make_shared<AESStringCrypter>();
    }

    typedef typename AESStreamGenerator::KeyByteArray KeyByteArray;
    typedef typename AESStreamGenerator::IVByteArray IVByteArray;

    AESStreamGenerator::AESStreamGenerator(const IVByteArray &iv, const KeyByteArray &key, StreamGeneratorMode mode)
        : OriginalAESStreamGenerator(iv, key, mode)
    {
        // 初始化加密操作
        if (!EVP_EncryptInit_ex(m_ctx, EVP_aes_128_ofb(), NULL, m_key.data(), m_iv.data()))
        {
            throw std::bad_alloc();
        }
    }
}
