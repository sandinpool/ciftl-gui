#include <cstring>
#include <cstdint>
#include <memory>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <ciftl_core/encoding/encoding.h>

namespace ciftl
{
    std::shared_ptr<Base64Encoding> default_base64_encoding()
    {
        return std::make_shared<Base64Encoding>();
    }

    std::string Base64Encoding::encode(const ByteVector &vec)
    {
        return encode(vec.data(), vec.size());
    }

    std::string Base64Encoding::encode(const byte *data, size_t len)
    {
        // chunk_size一定是3的倍数
        size_t chunk_size = BLOCK_SIZE / 4 * 3;
        size_t offset = 0;
        std::string base64_str;
        while (offset < len)
        {
            BIO *b64 = BIO_new(BIO_f_base64());
            BIO *bio = BIO_new(BIO_s_mem());
            bio = BIO_push(b64, bio);

            // 去掉最后的换行符
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

            int current_chunk_size = static_cast<int>(std::min(chunk_size, len - offset));
            BIO_write(bio, data + offset, current_chunk_size);
            BIO_flush(bio);

            BUF_MEM *buf;
            BIO_get_mem_ptr(bio, &buf);
            base64_str.append(buf->data, buf->length);

            offset += current_chunk_size;
            BIO_free_all(bio);
        }
        return base64_str;
    }

    std::optional<Error> Base64Encoding::validate(const std::string &str)
    {
        return validate(str.c_str(), str.size());
    }

    std::optional<Error> Base64Encoding::validate(const char *str, size_t len)
    {
        if (len % 4 != 0)
        {
            return std::make_optional(Error(Base64EncodingErrorCode::BAD_DECODING_SOURCE, "非法的Base64字符串"));
        }

        for (size_t i = 0; i < len; ++i)
        {
            if ((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') ||
                (str[i] >= '0' && str[i] <= '9') || str[i] == '+' || str[i] == '/')
            {
                continue;
            }
            else if (str[i] == '=')
            {
                // padding character
                if (i < len - 2 || (i == len - 2 && str[i + 1] != '='))
                {
                    return std::make_optional(Error(Base64EncodingErrorCode::BAD_DECODING_SOURCE, "非法的Base64字符串"));
                }
            }
            else
            {
                return std::make_optional(Error(Base64EncodingErrorCode::BAD_DECODING_SOURCE, "非法的Base64字符串"));
            }
        }
        return std::nullopt;
    }

    Result<ByteVector> Base64Encoding::decode(const std::string &str, bool skip_validate)
    {
        return decode(str.c_str(), str.size());
    }

    Result<ByteVector> Base64Encoding::decode(const char *str, size_t len, bool skip_validate)
    {
        if (!skip_validate)
        {
            if (auto res = validate(str, len); res)
            {
                return make_error<ByteVector>(std::move(res.value()));
            }
        }
        BIO *b64 = BIO_new(BIO_f_base64());
        BIO *bio = BIO_new_mem_buf(str, static_cast<int>(len));
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        // 动态分配内存
        std::string decoded_str;

        auto buffer = std::make_unique<char[]>(BLOCK_SIZE);
        int part_len;
        while ((part_len = BIO_read(bio, buffer.get(), BLOCK_SIZE)) > 0)
        {
            decoded_str.append(buffer.get(), part_len);
        }

        BIO_free_all(bio);
        ByteVector res(decoded_str.size());
        memcpy(res.data(), decoded_str.data(), decoded_str.size());
        return make_ok<ByteVector>(std::move(res));
    }
}
