#pragma once
#include <memory>
#include <string>
#include <vector>

#include <ciftl_core/etc/etc.h>

namespace ciftl
{
    class Encoding;

    enum class EncodingAlgorithm
    {
        BIN,
        HEX,
        BASE64
    };

    std::shared_ptr<Encoding> standard_encoding(EncodingAlgorithm ea);

    class Encoding
    {
    public:
        virtual std::string encode(const ByteVector &vec) = 0;

        virtual std::string encode(const byte *data, size_t len) = 0;

        virtual std::optional<Error> validate(const std::string &str) = 0;

        virtual std::optional<Error> validate(const char *str, size_t len) = 0;

        virtual Result<ByteVector> decode(const std::string &str, bool skip_validate = false) = 0;

        virtual Result<ByteVector> decode(const char *str, size_t len, bool skip_validate = false) = 0;
    };
}
