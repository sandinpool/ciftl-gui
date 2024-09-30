#pragma once
#include <memory>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <botan/stream_cipher.h>
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

    // 标准ChaCha20算法
    typedef StreamGenerator<CHACHA20_IV_LENGTH, CHACHA20_KEY_LENGTH> OriginalChaCha20StdStreamGenerator;
    class ChaCha20StdStreamGenerator final : public OriginalChaCha20StdStreamGenerator
    {
    public:
        typedef typename OriginalChaCha20StdStreamGenerator::IVByteArray IVByteArray;
        typedef typename OriginalChaCha20StdStreamGenerator::KeyByteArray KeyByteArray;

        static constexpr size_t IV_LENGTH = OriginalChaCha20StdStreamGenerator::IV_LENGTH;
        static constexpr size_t KEY_LENGTH = OriginalChaCha20StdStreamGenerator::KEY_LENGTH;
        static constexpr size_t BLOCK_LENGTH = OriginalChaCha20StdStreamGenerator::BLOCK_LENGTH;
    public:
        ChaCha20StdStreamGenerator(const IVByteArray &iv,
                                  const KeyByteArray &key,
                                  StreamGeneratorMode mode = StreamGeneratorMode::Medium)
            : StreamGenerator<IV_LENGTH, KEY_LENGTH, BLOCK_LENGTH>(iv, key),
            m_botan_chacha20(Botan::StreamCipher::create_or_throw("ChaCha20"))
        {
            m_botan_chacha20->set_key(key.data(), key.size());
            m_botan_chacha20->set_iv(iv.data(), iv.size());
        }

        ~ChaCha20StdStreamGenerator()=default;

    public:
        //刷新缓冲区内容
        Result<size_t> flush() override
        {
            if (this->m_current_index != this->m_max_buffer_size && this->m_is_flush_init)
            {
                return make_error<size_t>(StreamGeneratorErrorCode::FAILED_WHEN_FLUSHING_BUFFER, "刷新缓冲区时失败");
            }
            this->m_current_index = 0;
            auto temp_buffer = ByteVector(this->m_max_buffer_size);
            // 执行加密，这里是对m_plaintext_buffer进行加密，并将结果拷贝到temp_buffer中
            m_botan_chacha20->encipher(temp_buffer);
            memcpy(this->m_current_buffer.get(), temp_buffer.data(), this->m_max_buffer_size);
            return make_ok<size_t>(this->m_max_buffer_size);
        }
    private:
        std::unique_ptr<Botan::StreamCipher> m_botan_chacha20;
    };

    typedef StringCrypter<ChaCha20StdStreamGenerator> ChaCha20StdStringCrypter;
}
