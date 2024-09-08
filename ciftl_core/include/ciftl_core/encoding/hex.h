#pragma once

#include <cstdint>

#include <ciftl_core/encoding/encoding.h>

namespace ciftl
{
    class HexEncoding : public Encoding
    {
    public:
        enum class Case : uint8_t
        {
            LOWER,
            UPPER
        };

    private:
        Case m_case = Case::UPPER;

    public:
        inline void set_case(Case c)
        {
            m_case = c;
        }

        [[nodiscard]] inline Case get_case() const
        {
            return m_case;
        }

    public:
        std::string encode(const ByteVector &vec) override;

        std::string encode(const byte *data, size_t len) override;

        std::optional<Error> validate(const std::string &str) override;

        std::optional<Error> validate(const char *str, size_t len) override;

        Result<ByteVector> decode(const std::string &str, bool skip_validate = false) override;

        Result<ByteVector> decode(const char *str, size_t len, bool skip_validate = false) override;
    };

    std::shared_ptr<HexEncoding> default_hex_encoding();
}
