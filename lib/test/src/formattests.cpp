#include "thirdparty/doctest.h"
#include "lib/format.hpp"

#include <limits>

TEST_CASE("fmt::format")
{
	SUBCASE("std::string")
	{
		CHECK_EQ(lib::fmt::format("string: {}", std::string("value")), "string: value");
		CHECK_EQ(lib::fmt::format("string: {}", std::string()), "string: ");
	}

	SUBCASE("char*")
	{
		CHECK_EQ(lib::fmt::format("char*: {}", "value"), "char*: value");
		CHECK_EQ(lib::fmt::format("char*: {}", ""), "char*: ");
	}

	SUBCASE("bool")
	{
		CHECK_EQ(lib::fmt::format("bool: {}", true), "bool: true");
		CHECK_EQ(lib::fmt::format("bool: {}", false), "bool: false");
	}

	SUBCASE("to_string")
	{
		CHECK_EQ(lib::fmt::format("int: {}", (int)2147483647),
			"int: 2147483647");
		CHECK_EQ(lib::fmt::format("long long: {}", (long long)9223372036854775807),
			"long long: 9223372036854775807");

		CHECK_NOTHROW(lib::fmt::format("float: {}", std::numeric_limits<float>::max()));
		CHECK_NOTHROW(lib::fmt::format("double: {}", std::numeric_limits<double>::max()));

		CHECK_EQ(lib::fmt::format("json: {}", nlohmann::json{
			{"a", 1},
			{"b", 2}
		}), R"(json: {"a":1,"b":2})");
	}

	SUBCASE("time")
	{
		CHECK_EQ(lib::fmt::time(0), "0:00");
		CHECK_EQ(lib::fmt::time(999), "0:00");
		CHECK_EQ(lib::fmt::time(5000), "0:05");
		CHECK_EQ(lib::fmt::time(60 * 1000), "1:00");
		CHECK_EQ(lib::fmt::time(65 * 1000), "1:05");
	}

	SUBCASE("size")
	{
		CHECK_EQ(lib::fmt::size(1), "1 B");
		CHECK_EQ(lib::fmt::size(1000), "1 kB");
		CHECK_EQ(lib::fmt::size(1000000), "1 MB");
		CHECK_EQ(lib::fmt::size(1000000000), "1 GB");
	}
}
