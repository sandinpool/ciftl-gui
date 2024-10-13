#include <cstring>
#include <cstdint>

#include <ciftl_core/encoding/encoding.h>
#include <ciftl_core/encoding/hex.h>

namespace ciftl
{
    std::shared_ptr<HexEncoding> default_hex_encoding()
    {
        return std::make_shared<HexEncoding>();
    }

    std::string HexEncoding::encode(const ByteVector &vec)
    {
        return encode(vec.data(), vec.size());
    }

    std::string HexEncoding::encode(const byte *data, size_t len)
    {
        // 一个字节需要两个字符表示
        std::string res(len * 2, '\0');
        for (size_t i = 0; i < len; i++)
        {
            switch (m_case)
            {
            case Case::UPPER:
                sprintf(res.data() + 2 * i, "%02X", data[i]);
                break;
            case Case::LOWER:
                sprintf(res.data() + 2 * i, "%02x", data[i]);
                break;
            }
        }
        return res;
    }

    std::optional<Error> HexEncoding::validate(const std::string &str)
    {
        return validate(str.c_str(), str.size());
    }

    std::optional<Error> HexEncoding::validate(const char *str, size_t len)
    {
        if (len % 2 != 0)
        {
            return std::make_optional(Error(HexEncodingErrorCode::BAD_LENGTH, "输入文本长度有误"));
        }
        for (int i = 0; i < len; i++)
        {
            char ch = str[i];
            if (!((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')))
            {
                return std::make_optional(Error(HexEncodingErrorCode::BAD_CHARACTER, "输入文本中有非法字符"));
            }
        }
        return std::nullopt;
    }

    Result<ByteVector> HexEncoding::decode(const std::string &str, bool skip_validate)
    {
        return decode(str.c_str(), str.size());
    }

    Result<ByteVector> HexEncoding::decode(const char *str, size_t len, bool skip_validate)
    {
        if (!skip_validate)
        {
            if (auto res = validate(str, len); res)
            {
                return Result<ByteVector>::make_err(std::move(res.value()));
            }
        }
        ByteVector res(len / 2);
        for (size_t i = 0; i < len; i += 2)
        {
            uint32_t b = 0;
            for (size_t j = 0; j < 2; j++)
            {
                char ch = str[i + j];
                uint32_t val = 0;
                if (ch >= '0' && ch <= '9')
                {
                    val = ch - '0';
                }
                else if (ch >= 'A' && ch <= 'F')
                {
                    val = ch - 'A' + 10;
                }
                else if (ch >= 'a' && ch <= 'f')
                {
                    val = ch - 'a' + 10;
                }
                b = b * 0x10 + val;
            }
            res[i / 2] = (byte)b;
        }
        return Result<ByteVector>::make_ok<ByteVector>(std::move(res));
    }
}
