/**
 * Copyright (C) Huawei Technologies Co., Ltd. 2020.  ALL RIGHTS RESERVED.
 * See file LICENSE for terms.
 */

#include <common/test.h>
extern "C" {
#include <ucs/arch/bitops.h>
}

class test_bitops : public ucs::test {
protected:
    static void
    check_bitwise_equality(const uint8_t *buffer1, const uint8_t *buffer2,
                           const std::vector<int> &indices, int max_equal_index)
    {
        for (int i : indices) {
            if (i <= max_equal_index) {
                ASSERT_TRUE(ucs_bitwise_is_equal(buffer1, buffer2, i));
            } else {
                ASSERT_FALSE(ucs_bitwise_is_equal(buffer1, buffer2, i));
            }
        }
    }
};

UCS_TEST_F(test_bitops, ffs32) {
    EXPECT_EQ(0u, ucs_ffs32(0xfffff));
    EXPECT_EQ(16u, ucs_ffs32(0xf0000));
    EXPECT_EQ(1u, ucs_ffs32(0x4002));
    EXPECT_EQ(21u, ucs_ffs32(1ull << 21));
}

UCS_TEST_F(test_bitops, ffs64) {
    EXPECT_EQ(0u, ucs_ffs64(0xfffff));
    EXPECT_EQ(16u, ucs_ffs64(0xf0000));
    EXPECT_EQ(1u, ucs_ffs64(0x4002));
    EXPECT_EQ(41u, ucs_ffs64(1ull << 41));
}

UCS_TEST_F(test_bitops, ilog2) {
    EXPECT_EQ(0u, ucs_ilog2(1));
    EXPECT_EQ(2u, ucs_ilog2(4));
    EXPECT_EQ(2u, ucs_ilog2(5));
    EXPECT_EQ(2u, ucs_ilog2(7));
    EXPECT_EQ(14u, ucs_ilog2(17000));
    EXPECT_EQ(40u, ucs_ilog2(1ull << 40));
}

UCS_TEST_F(test_bitops, popcount) {
    EXPECT_EQ(0, ucs_popcount(0));
    EXPECT_EQ(2, ucs_popcount(5));
    EXPECT_EQ(16, ucs_popcount(0xffff));
    EXPECT_EQ(48, ucs_popcount(0xffffffffffffUL));
}

UCS_TEST_F(test_bitops, ctz) {
    EXPECT_EQ(0, ucs_count_trailing_zero_bits(1));
    EXPECT_EQ(28, ucs_count_trailing_zero_bits(0x10000000));
    EXPECT_EQ(32, ucs_count_trailing_zero_bits(0x100000000UL));
}

UCS_TEST_F(test_bitops, ptr_ctz) {
    uint8_t buffer[20] = {0};

    ASSERT_EQ(0, ucs_count_ptr_trailing_zero_bits(buffer, 0));
    ASSERT_EQ(1, ucs_count_ptr_trailing_zero_bits(buffer, 1));
    ASSERT_EQ(8, ucs_count_ptr_trailing_zero_bits(buffer, 8));
    ASSERT_EQ(10, ucs_count_ptr_trailing_zero_bits(buffer, 10));
    ASSERT_EQ(64, ucs_count_ptr_trailing_zero_bits(buffer, 64));
    ASSERT_EQ(70, ucs_count_ptr_trailing_zero_bits(buffer, 70));

    buffer[0] = 0x10; /* 00010000 */

    ASSERT_EQ(0, ucs_count_ptr_trailing_zero_bits(buffer, 0));
    ASSERT_EQ(1, ucs_count_ptr_trailing_zero_bits(buffer, 1));
    ASSERT_EQ(4, ucs_count_ptr_trailing_zero_bits(buffer, 8));
    ASSERT_EQ(6, ucs_count_ptr_trailing_zero_bits(buffer, 10));
    ASSERT_EQ(60, ucs_count_ptr_trailing_zero_bits(buffer, 64));
    ASSERT_EQ(66, ucs_count_ptr_trailing_zero_bits(buffer, 70));

    buffer[0] = 0x01; /* 00000001 */

    ASSERT_EQ(0, ucs_count_ptr_trailing_zero_bits(buffer, 0));
    ASSERT_EQ(1, ucs_count_ptr_trailing_zero_bits(buffer, 1));
    ASSERT_EQ(0, ucs_count_ptr_trailing_zero_bits(buffer, 8));
    ASSERT_EQ(2, ucs_count_ptr_trailing_zero_bits(buffer, 10));
    ASSERT_EQ(56, ucs_count_ptr_trailing_zero_bits(buffer, 64));
    ASSERT_EQ(62, ucs_count_ptr_trailing_zero_bits(buffer, 70));

    buffer[8] = 0x01; /* 00000001 */

    ASSERT_EQ(0, ucs_count_ptr_trailing_zero_bits(buffer, 0));
    ASSERT_EQ(1, ucs_count_ptr_trailing_zero_bits(buffer, 1));
    ASSERT_EQ(0, ucs_count_ptr_trailing_zero_bits(buffer, 8));
    ASSERT_EQ(2, ucs_count_ptr_trailing_zero_bits(buffer, 10));
    ASSERT_EQ(56, ucs_count_ptr_trailing_zero_bits(buffer, 64));
    ASSERT_EQ(62, ucs_count_ptr_trailing_zero_bits(buffer, 70));

    ASSERT_EQ(0, ucs_count_ptr_trailing_zero_bits(buffer, 72));
    ASSERT_EQ(8, ucs_count_ptr_trailing_zero_bits(buffer, 80));
    ASSERT_EQ(56, ucs_count_ptr_trailing_zero_bits(buffer, 128));
    ASSERT_EQ(88, ucs_count_ptr_trailing_zero_bits(buffer, 160));
}

UCS_TEST_F(test_bitops, is_equal) {
    uint8_t buffer1[20]            = {0};
    uint8_t buffer2[20]            = {0};
    const std::vector<int> indices = {0, 1, 8, 64, 65, 128, 130, 159, 160};

    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 160);

    buffer1[19] = 0x1; /* 00000001 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 159);

    buffer1[19] = 0x10; /* 00010000 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 130);

    buffer1[16] = 0xff; /* 11111111 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 128);

    buffer1[9] = 0xff; /* 11111111 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 65);

    buffer1[7] = 0xff; /* 11111111 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 8);

    buffer1[1] = 0xff; /* 11111111 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 8);

    buffer1[0] = 0x1; /* 00000001 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 1);

    buffer2[0] = 0x1; /* 00000001 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 8);

    buffer2[0] = 0xff; /* 11111111 */
    test_bitops::check_bitwise_equality(buffer1, buffer2, indices, 0);
}

template<typename T> void test_clz()
{
    constexpr int bits = sizeof(T) * 8;
    T v                = 1;

    for (int i = bits - 1; v != 0; v <<= 1, --i) {
        ASSERT_EQ(i, ucs_count_leading_zero_bits(v));
    }

    ASSERT_EQ(bits, ucs_count_leading_zero_bits(v));
}

UCS_TEST_F(test_bitops, clz) {
    test_clz<uint32_t>();
    test_clz<uint64_t>();
    test_clz<int32_t>();
    test_clz<int64_t>();
    test_clz<size_t>();
    test_clz<ssize_t>();
}

UCS_TEST_F(test_bitops, clz_type)
{
    EXPECT_GT(0, ucs_count_leading_zero_bits(~0LLU) - 1);

    EXPECT_EQ(UINT32_MAX, ucs_count_leading_zero_bits(~0LLU) - 1);
    EXPECT_EQ(UINT32_MAX, ucs_count_leading_zero_bits(0LLU) - 65);
    EXPECT_EQ(UINT32_MAX, ucs_count_leading_zero_bits(~0U) - 1);
    EXPECT_EQ(UINT32_MAX, ucs_count_leading_zero_bits(0U) - 33);

    EXPECT_EQ(UINT32_MAX, ucs_count_leading_zero_bits(~0LL) - 1);
    EXPECT_EQ(UINT32_MAX, ucs_count_leading_zero_bits(0LL) - 65);
    EXPECT_EQ(UINT32_MAX, ucs_count_leading_zero_bits(~0) - 1);
    EXPECT_EQ(UINT32_MAX, ucs_count_leading_zero_bits(0) - 33);
}

template<typename Type> void test_mask()
{
    Type expected = 0;
    /* Test extra bit (should return full mask) */
    for (size_t bit_num = 0; bit_num <= (sizeof(Type) * 8 + 1); ++bit_num) {
        Type mask = UCS_MASK(bit_num);
        if ((bit_num > 0) && (bit_num <= 64)) {
            expected |= UCS_BIT(bit_num - 1);
        }

        EXPECT_EQ(expected, mask) << "bit_num=" << bit_num;
    }
}

UCS_TEST_F(test_bitops, mask) {
    test_mask<int8_t>();
    test_mask<uint8_t>();
    test_mask<int16_t>();
    test_mask<uint16_t>();
    test_mask<int32_t>();
    test_mask<uint32_t>();
    test_mask<int64_t>();
    test_mask<uint64_t>();
}
