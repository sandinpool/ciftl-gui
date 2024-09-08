#include <gtest/gtest.h>
#include <ciftl_core/encoding/encoding.h>
#include <ciftl_core/crypter/crypter.h>
#include "test_core.h"

using namespace ciftl;

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
