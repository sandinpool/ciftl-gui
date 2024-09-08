#include <memory>

#include <ciftl_core/crypter/crypter.h>
#include <ciftl_core/encoding/encoding.h>

namespace ciftl
{
    StreamCrypter::StreamCrypter(std::shared_ptr<StreamGeneratorInterface> generator)
        : m_stream_generator(generator),
          m_original_crc32(0),
          m_crypted_crc32(0)
    {
    }

    Result<size_t> StreamCrypter::crypt(byte *data, size_t len)
    {
        if (!m_stream_generator)
        {
            return make_error<size_t>(StreamCrypterErrorCode::INVALID_CIPHER_STREAM_GENERATOR, "创建密码流生成器失败");
        }
        m_original_crc32 = crc32c::Extend(m_original_crc32, data, len);
        auto buffer = std::make_unique<byte[]>(len);
        if (auto res = m_stream_generator->generate(buffer.get(), len); res.is_err())
        {
            return make_error<size_t>(std::move(res.get_err_val().value()));
        }
        for (size_t i = 0; i < len; i++)
        {
            data[i] ^= buffer[i];
        }
        m_crypted_crc32 = crc32c::Extend(m_crypted_crc32, data, len);
        return make_ok<size_t>(len);
    }

    Result<size_t> StreamCrypter::crypt(ByteVector &data)
    {
        return crypt(data.data(), data.size());
    }

    uint32_t StreamCrypter::original_crc32()
    {
        return m_original_crc32;
    }

    uint32_t StreamCrypter::crypted_crc32()
    {
        return m_crypted_crc32;
    }
}
