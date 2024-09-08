#include <memory>
#include <ciftl_core/encoding/core_encoding.h>

#include "ciftl_core/encoding/hex.h"

namespace ciftl
{
    const static std::unordered_map<EncodingAlgorithm, std::shared_ptr<Encoding>> ENCODING_SELECTION = {
        // EncodingAlgorithm::BIN,
        {EncodingAlgorithm::HEX, std::make_shared<HexEncoding>()},
        {EncodingAlgorithm::BASE64, std::make_shared<Base64Encoding>()}};

    std::shared_ptr<Encoding> standard_encoding(EncodingAlgorithm ea)
    {
        if (auto iter = ENCODING_SELECTION.find(ea); iter != ENCODING_SELECTION.end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }
}
