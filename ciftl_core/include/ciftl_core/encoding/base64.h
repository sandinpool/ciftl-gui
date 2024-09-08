#pragma once

#include <cstdint>

#include <ciftl_core/encoding/encoding.h>

namespace ciftl
{
    class Base64Encoding : public Encoding
    {
    private:
        static const size_t BLOCK_SIZE = 2048;

    public:
        std::string encode(const ByteVector &vec) override;

        std::string encode(const byte *data, size_t len) override;

        std::optional<Error> validate(const std::string &str) override;

        std::optional<Error> validate(const char *str, size_t len) override;

        Result<ByteVector> decode(const std::string &str, bool skip_validate = false) override;

        Result<ByteVector> decode(const char *str, size_t len, bool skip_validate = false) override;
    };

    std::shared_ptr<Base64Encoding> default_base64_encoding();
}
