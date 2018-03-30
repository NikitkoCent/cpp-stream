#include <stream.h>
#include <filters_lib.h>
#include <vector>
#include <sstream>
#include <type_traits>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace stream;
using namespace stream::filters;


TEST(FILTERS_INT_STREAM, PRINT_TO_RETURNVALUE)
{
    std::ostringstream stream;

    ASSERT_TRUE((std::is_same<decltype(stream::Stream{1, 2, 3} | print_to(stream)), decltype(stream)&>::value));
    ASSERT_EQ(&(stream::Stream{1, 2, 3} | print_to(stream)), &stream);
}

TEST(FILTERS_INT_STREAM, PRINT_TO_EMPTY)
{
    std::ostringstream stream;

    std::vector<int> v;
    stream::Stream(v) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, PRINT_TO_1)
{
    std::ostringstream stream;

    std::vector<int> v;
    stream::Stream(20) | print_to(stream);
    ASSERT_EQ(stream.str(), "20");
}

TEST(FILTERS_INT_STREAM, PRINT_TO_2)
{
    std::ostringstream stream;

    std::vector<int> v;
    stream::Stream(1, 2) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2");
}

TEST(FILTERS_INT_STREAM, PRINT_TO_GENERIC)
{
    std::ostringstream stream;

    std::vector<int> v;
    stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5 6 7 8 9");
}


TEST(FILTERS_INT_STREAM, SKIP_0)
{
    std::ostringstream stream;

    stream::Stream{1, 2, 3} | skip(0) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3");
}

TEST(FILTERS_INT_STREAM, SKIP_1)
{
    std::ostringstream stream;

    stream::Stream{1, 2, 3} | skip(1) | print_to(stream);
    ASSERT_EQ(stream.str(), "2 3");
}

TEST(FILTERS_INT_STREAM, SKIP_ALL)
{
    std::ostringstream stream;

    stream::Stream{1, 2, 3} | skip(3) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, SKIP_ALL_BUT_ONE)
{
    std::ostringstream stream;

    stream::Stream{1, 2, 3} | skip(2) | print_to(stream);
    ASSERT_EQ(stream.str(), "3");
}

TEST(FILTERS_INT_STREAM, SKIP_DOUBLE)
{
    std::ostringstream stream;

    Stream a = Stream(1, 2, 3) | skip(1);
    Stream b = a | skip(1);

    b | print_to(stream);
    ASSERT_EQ(stream.str(), "3");
}


TEST(FILTERS_INT_STREAM, MAP_EMPTY)
{
    std::ostringstream stream;

    stream::Stream(std::vector<int>{}) | map([](auto &&v){ return std::move(v); }) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, MAP_1)
{
    std::ostringstream stream;

    stream::Stream(15) | map([](auto &&v){ return v * v; }) | print_to(stream);
    ASSERT_EQ(stream.str(), "225");
}

TEST(FILTERS_INT_STREAM, MAP_GENERIC)
{
    std::ostringstream stream;

    stream::Stream(-1, 2, -3, 4, -5, 6, -7, 8, -9) | map([](auto &&v){ return v * v ;}) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 4 9 16 25 36 49 64 81");
}


TEST(FILTERS_INT_STREAM, GET_0_EMPTY)
{
    std::ostringstream stream;

    stream::Stream(std::vector<int>{}) | get(0) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, GET_0_NOTEMPTY)
{
    std::ostringstream stream;

    stream::Stream(1, 2, 3) | get(0) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, GET_1_EMPTY)
{
    std::ostringstream stream;

    stream::Stream(std::vector<int>{}) | get(1) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, GET_1_NOTEMPTY)
{
    std::ostringstream stream;

    stream::Stream(5, 2, 3) | get(1) | print_to(stream);
    ASSERT_EQ(stream.str(), "5");
}

TEST(FILTERS_INT_STREAM, GET_1_FROM_SINGLE)
{
    std::ostringstream stream;

    stream::Stream(5) | get(1) | print_to(stream);
    ASSERT_EQ(stream.str(), "5");
}

TEST(FILTERS_INT_STREAM, GET_ALL)
{
    std::ostringstream stream;

    stream::Stream(1, 2, 3, 4, 5) | get(5) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5");
}

TEST(FILTERS_INT_STREAM, GET_OUT_OF_RANGE)
{
    std::ostringstream stream;

    stream::Stream(1, 2, 3, 4, 5) | get(6) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5");
}