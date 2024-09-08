#include <vector>
#include <gtest/gtest.h>
#include <ciftl_core/encoding/encoding.h>
#include <ciftl_core/crypter/crypter.h>
#include "test_core.h"

using namespace ciftl;

TEST(TestCrypter, TestStreamCrypter)
{
    auto test_iv1 = StreamGenerator<128, 128>::generate_iv_from_factor((uint32_t)123456);
    GTEST_LOG_(INFO) << fmt::format("Test IV1: {}", auto_format(test_iv1));
    auto test_iv2 = StreamGenerator<128, 128>::rand_iv();
    GTEST_LOG_(INFO) << fmt::format("Test IV2: {}", auto_format(test_iv2));
    auto test_key = StreamGenerator<128, 128>::generate_key_from_password("abcdefg");
    GTEST_LOG_(INFO) << fmt::format("Test Key: {}", auto_format(test_key));
    // ChaCha20
    {
        typedef ChaCha20StreamGenerator::IVByteArray IVByteArray;
        typedef ChaCha20StreamGenerator::KeyByteArray KeyByteArray;

        auto iv1 = ChaCha20StreamGenerator::rand_iv();
        GTEST_LOG_(INFO) << fmt::format("IV1: {}", auto_format(iv1));
        IVByteArray iv2 = {0x89, 0x45, 0x9A, 0x13,
                           0xB4, 0x57, 0x10, 0x4F,
                           0x23, 0x43, 0xD4, 0x59};
        KeyByteArray key = ChaCha20StreamGenerator::generate_key_from_password("123456");
        auto ccsg1 = std::make_shared<ChaCha20StreamGenerator>(iv2, key, StreamGeneratorMode::Large);
        StreamCrypter sc1(ccsg1);
        ByteVector plain = {0x41, 0x42, 0x43, 0x44, 0x31, 0x32, 0x33, 0x34};
        sc1.crypt(plain);
        auto ccsg2 = std::make_shared<ChaCha20StreamGenerator>(iv2, key, StreamGeneratorMode::Short);
        StreamCrypter sc2(ccsg2);
        sc2.crypt(plain);
        ASSERT_TRUE(is_equal(plain, {0x41, 0x42, 0x43, 0x44, 0x31, 0x32, 0x33, 0x34}));
    }
}

TEST(TestCrypter, TestStringCrypter)
{
    // ChaCha20
    {
        StringCrypter<ChaCha20StreamGenerator> string_crypter;
        std::string plain_text = "abcdefg1234567";
        auto res1 = string_crypter.encrypt(plain_text, "123456");
        EXPECT_TRUE(res1.is_ok());
        std::string en_text = res1.get_ok_val().value();
        auto res2 = string_crypter.decrypt(en_text, "123456");
        EXPECT_TRUE(res2.is_ok());
        EXPECT_EQ(plain_text, res2.get_ok_val().value());
        auto res3 = string_crypter.decrypt(en_text, "1234567");
        EXPECT_TRUE(res3.is_err());
    }
}
