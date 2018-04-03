#include "noisy.h"
#include <stream.h>
#include <filters_lib.h>
#include <vector>
#include <sstream>
#include <type_traits>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace stream;
using namespace stream::filters;


TEST(FILTERS_NOISY_FINITE_STREAM, PRINT_TO_EMPTY)
{
    std::ostringstream stream;

    stream::Stream<Noisy>() | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, PRINT_TO_1)
{
    std::ostringstream stream;

    stream::Stream(Noisy(20)) | print_to(stream);
    ASSERT_EQ(stream.str(), "20");
}

TEST(FILTERS_NOISY_FINITE_STREAM, PRINT_TO_2)
{
    std::ostringstream stream;

    stream::Stream(Noisy(1), Noisy(2)) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2");
}

TEST(FILTERS_NOISY_FINITE_STREAM, PRINT_TO_GENERIC)
{
    std::ostringstream stream;

    stream::Stream(Noisy(1), Noisy(2), Noisy(3), Noisy(4), Noisy(5), Noisy(6), Noisy(7), Noisy(8),
                   Noisy(9)) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5 6 7 8 9");
}


TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_0)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(0) | to_vector();
    ASSERT_EQ(result.size(), 3U);
    ASSERT_EQ(result[0].copyCount, 0U);
    ASSERT_EQ(result[1].copyCount, 0U);
    ASSERT_EQ(result[2].copyCount, 0U);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_1)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(1) | to_vector();
    ASSERT_EQ(result.size(), 2U);
    ASSERT_EQ(result[0].copyCount, 0U);
    ASSERT_EQ(result[1].copyCount, 0U);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_ALL)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(3) | to_vector();
    ASSERT_TRUE(result.empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_ALL_BUT_ONE)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(2) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0U);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_DOUBLE)
{
    Stream a = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(1);
    Stream b = a | skip(1);

    auto result = b | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0U);
}


TEST(FILTERS_NOISY_FINITE_STREAM, MAP_EMPTY)
{
    ASSERT_TRUE((stream::Stream<Noisy>() | map([](auto &&v){ return std::move(v); }) | to_vector()).empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, MAP_1)
{
    auto result = Stream(Noisy(15))
                  | map([](auto &&v){ v.value = v.value * v.value; return std::move(v); })
                  | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0U);
    ASSERT_EQ(result[0].value, 225);
}

TEST(FILTERS_NOISY_FINITE_STREAM, MAP_GENERIC)
{
    auto result = Stream(Noisy(-1),
                         Noisy(2),
                         Noisy(-3),
                         Noisy(4),
                         Noisy(-5),
                         Noisy(6),
                         Noisy(-7),
                         Noisy(8),
                         Noisy(-9))
                  | map([](auto &&v){ v.value = v.value * v.value; return std::move(v); })
                  | to_vector();

    ASSERT_EQ(result.size(), 9U);

    for (unsigned i = 0; i < 9; ++i)
    {
        ASSERT_EQ(result[i].copyCount, 0U);
        ASSERT_EQ(result[i].value, (i + 1) * (i + 1));
    }
}


TEST(FILTERS_NOISY_FINITE_STREAM, GET_0_EMPTY)
{
    ASSERT_TRUE((stream::Stream<Noisy>() | get(0) | to_vector()).empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, GET_0_NOTEMPTY)
{
    ASSERT_TRUE((stream::Stream(Noisy(1), Noisy(2), Noisy(3)) | get(0) | to_vector()).empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, GET_1_EMPTY)
{
    ASSERT_TRUE((stream::Stream<Noisy>() | get(1) | to_vector()).empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, GET_1_NOTEMPTY)
{
    auto result = Stream(Noisy(5), Noisy(2), Noisy(3)) | get(1) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0);
    ASSERT_EQ(result[0].value, 5);
}

TEST(FILTERS_NOISY_FINITE_STREAM, GET_1_FROM_SINGLE)
{
    auto result = Stream(Noisy(5)) | get(1) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0);
    ASSERT_EQ(result[0].value, 5);
}

TEST(FILTERS_NOISY_FINITE_STREAM, GET_ALL)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3), Noisy(4), Noisy(5)) | get(5) | to_vector();
    ASSERT_EQ(result.size(), 5U);
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_EQ(result[i].copyCount, 0);
        ASSERT_EQ(result[i].value, i + 1);
    }
}

TEST(FILTERS_NOISY_FINITE_STREAM, GET_OUT_OF_RANGE)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3), Noisy(4), Noisy(5)) | get(6) | to_vector();
    ASSERT_EQ(result.size(), 5U);
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_EQ(result[i].copyCount, 0);
        ASSERT_EQ(result[i].value, i + 1);
    }
}


auto sumReducer = [](auto &&v1, auto &&v2) { v1.value = v1.value + v2.value; return std::move(v1); };

TEST(FILTERS_NOISY_FINITE_STREAM, REDUCE_EMPTY)
{
    auto result = Stream<Noisy>() | reduce(sumReducer);
    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 0);
}

TEST(FILTERS_NOISY_FINITE_STREAM, REDUCE_SINGLE)
{
    auto result = Stream(Noisy(10)) | reduce(sumReducer);
    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 10);
}

TEST(FILTERS_NOISY_FINITE_STREAM, REDUCE_DOUBLE)
{
    auto result = Stream(Noisy(45), Noisy(-100)) | reduce(sumReducer);
    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, -55);
}

TEST(FILTERS_NOISY_FINITE_STREAM, REDUCE_GENERIC)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9),
                         Noisy(10)) | reduce(sumReducer);

    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 55);
}

auto stringReducer = [](auto &&str, auto &&num) { return std::move(str += " " + std::to_string(num.value)); };
auto stringInit = [](auto &&num) { return std::to_string(num.value); };

TEST(FILTERS_NOISY_FINITE_STREAM, REDUCE_TO_STRING_EMPTY)
{
    auto result = Stream<Noisy>() | reduce(stringInit, stringReducer);
    ASSERT_TRUE(result.empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, REDUCE_TO_STRING_SINGLE)
{
    auto result = Stream(Noisy(10)) | reduce(stringInit, stringReducer);
    ASSERT_EQ(result, "10");
}

TEST(FILTERS_NOISY_FINITE_STREAM, REDUCE_TO_STRING_DOUBLE)
{
    auto result = Stream(Noisy(45), Noisy(-100)) | reduce(stringInit, stringReducer);
    ASSERT_EQ(result, "45 -100");
}

TEST(FILTERS_NOISY_FINITE_STREAM, REDUCE_TO_STRING_GENERIC)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9),
                         Noisy(10)) | reduce(stringInit, stringReducer);
    ASSERT_EQ(result, "1 2 3 4 5 6 7 8 9 10");
}


TEST(FILTERS_NOISY_FINITE_STREAM, SUM_EMPTY)
{
    auto result = Stream<Noisy>() | sum();
    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 0);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SUM_SINGLE)
{
    auto result = Stream(Noisy(10)) | sum();
    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 10);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SUM_DOUBLE)
{
    auto result = Stream(Noisy(45), Noisy(-100)) | sum();
    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, -55);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SUM_GENERIC)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9),
                         Noisy(10)) | sum();
    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 55);
}


TEST(FILTERS_NOISY_FINITE_STREAM, NTH_EMPTY)
{
    ASSERT_THROW(stream::Stream<Noisy>() | nth(0), std::out_of_range);
}

TEST(FILTERS_NOISY_FINITE_STREAM, NTH_SINGLE)
{
    auto result = Stream(Noisy(125)) | nth(0);
    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 125);
}

TEST(FILTERS_NOISY_FINITE_STREAM, NTH_GENERIC_FIRST)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7)) | nth(0);

    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 1);
}

TEST(FILTERS_NOISY_FINITE_STREAM, NTH_GENERIC_MIDDLE)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7)) | nth(3);

    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 4);
}

TEST(FILTERS_NOISY_FINITE_STREAM, NTH_GENERIC_LAST)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7)) | nth(6);

    ASSERT_EQ(result.copyCount, 0);
    ASSERT_EQ(result.value, 7);
}

TEST(FILTERS_NOISY_FINITE_STREAM, NTH_GENERIC_OUT_OF_RANGE1)
{
    ASSERT_THROW(stream::Stream(Noisy(1),
                                Noisy(2),
                                Noisy(3),
                                Noisy(4),
                                Noisy(5),
                                Noisy(6),
                                Noisy(7)) | nth(7), std::out_of_range);
}

TEST(FILTERS_NOISY_FINITE_STREAM, NTH_GENERIC_OUT_OF_RANGE2)
{
    ASSERT_THROW(stream::Stream(Noisy(1),
                                Noisy(2),
                                Noisy(3),
                                Noisy(4),
                                Noisy(5),
                                Noisy(6),
                                Noisy(7)) | nth(100), std::out_of_range);
}


TEST(FILTERS_NOISY_FINITE_STREAM, TO_VECTOR_EMPTY)
{
    ASSERT_TRUE((stream::Stream<Noisy>() | to_vector()).empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, TO_VECTOR_SINGLE)
{
    auto result = Stream(Noisy(200)) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0);
    ASSERT_EQ(result[0].value, 200);
}

TEST(FILTERS_NOISY_FINITE_STREAM, TO_VECTOR_GENERIC)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5)) | to_vector();

    ASSERT_EQ(result.size(), 5U);
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_EQ(result[i].copyCount, 0);
        ASSERT_EQ(result[i].value, i + 1);
    }
}


TEST(FILTERS_NOISY_FINITE_STREAM, FILTER_EMPTY)
{
    ASSERT_TRUE((stream::Stream<Noisy>() | filter([](auto &&) { return true; }) | to_vector()).empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, FILTER_SINGLE_TO_EMPTY)
{
    ASSERT_TRUE((stream::Stream(Noisy(45)) | filter([](auto &&) { return false; }) | to_vector()).empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, FILTER_SINGLE_TO_SINGLE)
{
    auto result = Stream(Noisy(45)) | filter([](auto &&) { return true; }) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0);
    ASSERT_EQ(result[0].value, 45);
}

TEST(FILTERS_NOISY_FINITE_STREAM, FILTER_GENERIC_TO_EMPTY)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9)) | filter([](auto &&) { return false; }) | to_vector();

    ASSERT_TRUE(result.empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, FILTER_GENERIC_TO_ITSELF)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9)) | filter([](auto &&) { return true; }) | to_vector();

    ASSERT_EQ(result.size(), 9U);
    for (int i = 0; i < 9; ++i)
    {
        ASSERT_EQ(result[i].copyCount, 0);
        ASSERT_EQ(result[i].value, i + 1);
    }
}

TEST(FILTERS_NOISY_FINITE_STREAM, FILTER_GENERIC_ONLY_EVEN)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9)) | filter([](auto &&v) { return !(v.value % 2); }) | to_vector();

    ASSERT_EQ(result.size(), 4U);
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(result[i].copyCount, 0);
        ASSERT_EQ(result[i].value, 2 + 2 * i);
    }
}


TEST(FILTERS_NOISY_FINITE_STREAM, GROUP_EMPTY)
{
    ASSERT_TRUE((stream::Stream<Noisy>() | group(1) | to_vector()).empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, GROUP_SINGLE_BY_1)
{
    auto result = Stream(Noisy(45)) | group(1) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].size(), 1U);
    ASSERT_EQ(result[0][0].copyCount, 0);
    ASSERT_EQ(result[0][0].value, 45);
}

TEST(FILTERS_NOISY_FINITE_STREAM, GROUP_SINGLE_BY_2)
{
    auto result = Stream(Noisy(45)) | group(2) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].size(), 1U);
    ASSERT_EQ(result[0][0].copyCount, 0);
    ASSERT_EQ(result[0][0].value, 45);
}

TEST(FILTERS_NOISY_FINITE_STREAM, GROUP_GENERIC_BY_1)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9)) | group(1) | to_vector();

    ASSERT_EQ(result.size(), 9U);
    for (int i = 0; i < 9; ++i)
    {
        ASSERT_EQ(result[i].size(), 1U);
        ASSERT_EQ(result[i][0].copyCount, 0);
        ASSERT_EQ(result[i][0].value, i + 1);
    }
}

TEST(FILTERS_NOISY_FINITE_STREAM, GROUP_GENERIC_TO_FEW)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9)) | group(4) | to_vector();

    ASSERT_EQ(result.size(), 3U);

    ASSERT_EQ(result[0].size(), 4U);
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(result[0][i].copyCount, 0);
        ASSERT_EQ(result[0][i].value, i + 1);
    }

    ASSERT_EQ(result[1].size(), 4U);
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(result[1][i].copyCount, 0);
        ASSERT_EQ(result[1][i].value, i + 5);
    }

    ASSERT_EQ(result[2].size(), 1U);
    ASSERT_EQ(result[2][0].copyCount, 0);
    ASSERT_EQ(result[2][0].value, 9);
}

TEST(FILTERS_NOISY_FINITE_STREAM, GROUP_GENERIC_TO_ONE_SAME)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9)) | group(9) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].size(), 9U);

    for (int i = 0; i < 9; ++i)
    {
        ASSERT_EQ(result[0][i].copyCount, 0);
        ASSERT_EQ(result[0][i].value, i + 1);
    }
}

TEST(FILTERS_NOISY_FINITE_STREAM, GROUP_GENERIC_TO_ONE_MORE1)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9)) | group(10) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].size(), 9U);

    for (int i = 0; i < 9; ++i)
    {
        ASSERT_EQ(result[0][i].copyCount, 0);
        ASSERT_EQ(result[0][i].value, i + 1);
    }
}

TEST(FILTERS_NOISY_FINITE_STREAM, GROUP_GENERIC_TO_ONE_MORE2)
{
    auto result = Stream(Noisy(1),
                         Noisy(2),
                         Noisy(3),
                         Noisy(4),
                         Noisy(5),
                         Noisy(6),
                         Noisy(7),
                         Noisy(8),
                         Noisy(9)) | group(100) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].size(), 9U);

    for (int i = 0; i < 9; ++i)
    {
        ASSERT_EQ(result[0][i].copyCount, 0);
        ASSERT_EQ(result[0][i].value, i + 1);
    }
}
