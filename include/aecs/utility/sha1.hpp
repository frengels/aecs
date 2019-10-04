#pragma once

#include <array>
#include <bit>
#include <string_view>

namespace aecs
{

// calculate the sha1 for a string_view. This is used to calculate the hash
// for component and system names. As these operations are done at compile time,
// there is no runtime overhead for calculating the hash and you get a strong
// collision-free hash.
//
// Unlike regular SHA1, the result from this function is truncated down to
// std::size_t. Making it usable for hashtables.
struct sha1_fn
{
private:
    static constexpr uint32_t k1 = 0x5a827999;
    static constexpr uint32_t k2 = 0x6ed9eba1;
    static constexpr uint32_t k3 = 0x8f1bbcdc;
    static constexpr uint32_t k4 = 0xca62c1d6;

    static constexpr auto block_length = 64;

    struct context
    {

        std::array<uint32_t, 5> state{0x67452301,
                                      0xefcdab89,
                                      0x98badcfe,
                                      0x10325476,
                                      0xc3d2e1f0};

        std::array<uint8_t, 64> buffer{};
        uint8_t                 buffer_offset{};
        uint32_t                bit_count_low{};
        uint32_t                bit_count_high{};

        context() = default;

        constexpr void push_byte(uint8_t data)
        {
            push_uncounted(data);

            if (bit_count_low < 0xfffffff8)
            {
                bit_count_low += 8;
            }
            else
            {
                bit_count_low = 0;
                ++bit_count_high;
            }
        }

        constexpr void push_uncounted(uint8_t data)
        {
            buffer[buffer_offset++] = data;

            if (buffer_offset == block_length)
            {
                buffer_offset = 0;
                process_block();
            }
        }

        constexpr void pad() noexcept
        {
            push_uncounted(0x80);

            while (buffer_offset != 56)
            {
                push_uncounted(0x00);
            }

            push_uncounted(bit_count_high >> 24);
            push_uncounted(bit_count_high >> 16);
            push_uncounted(bit_count_high >> 8);
            push_uncounted(bit_count_high);
            push_uncounted(bit_count_low >> 24);
            push_uncounted(bit_count_low >> 16);
            push_uncounted(bit_count_low >> 8);
            push_uncounted(bit_count_low);
        }

        constexpr void process_block() noexcept
        {
            uint32_t w[80]{};

            for (auto i = 0; i < 16; ++i)
            {
                w[i] = (buffer[i * 4 + 0] << 24);
                w[i] |= (buffer[i * 4 + 1] << 16);
                w[i] |= (buffer[i * 4 + 2] << 8);
                w[i] |= (buffer[i * 4 + 3]);
            }

            for (auto i = 16; i < 80; ++i)
            {
                w[i] = lrot(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
            }

            uint32_t a = state[0];
            uint32_t b = state[1];
            uint32_t c = state[2];
            uint32_t d = state[3];
            uint32_t e = state[4];
            uint32_t t = 0;

            for (auto i = 0; i < 80; ++i)
            {
                if (i < 20)
                {
                    t = ((b & c) | (~b & d)) + k1;
                }
                else if (i < 40)
                {
                    t = (b ^ c ^ d) + k2;
                }
                else if (i < 60)
                {
                    t = ((b & c) | (b & d) | (c & d)) + k3;
                }
                else
                {
                    t = (b ^ c ^ d) + k4;
                }

                t += lrot(a, 5) + e + w[i];
                e = d;
                d = c;
                c = lrot(b, 30);
                b = a;
                a = t;
            }

            state[0] += a;
            state[1] += b;
            state[2] += c;
            state[3] += d;
            state[4] += e;
        }

        constexpr void update(std::string_view str)
        {
            for (auto c : str)
            {
                push_byte(c);
            }
        }

        constexpr std::array<uint32_t, 5> final() noexcept
        {
            pad();

            return state;
        }
    };

    static constexpr uint32_t lrot(uint32_t num, int bits) noexcept
    {
        return ((num << bits) ^ (num >> (32 - bits)));
    }

public:
    constexpr auto operator()(std::string_view str) const noexcept
    {
        context ctx{};
        ctx.update(str);
        auto res = ctx.final();

        // round up for non negative, non 0 numbers
        constexpr auto ceil_div = [](auto n, auto d) {
            return 1 + ((n - 1) / d);
        };

        constexpr auto required =
            ceil_div(sizeof(std::size_t), sizeof(uint32_t));

        static_assert(required <= res.size(),
                      "Digest not long enough to fit in std::size_t");

        std::size_t hash{};

        for (auto i = 0; i < required; ++i)
        {
            auto shift_amount = (sizeof(uint32_t) * 8) * (required - 1 - i);

            hash |= static_cast<std::size_t>(res[i]) << shift_amount;
        }

        return hash;
    }
};

inline constexpr auto sha1 = sha1_fn{};
} // namespace aecs