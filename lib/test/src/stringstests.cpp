#include "lib/strings.hpp"
#include "thirdparty/doctest.h"

#include <array>

TEST_CASE("strings::join")
{
	auto result = lib::strings::join({
		"a", "bb", "ccc",
	}, ", ");
	CHECK_EQ(result, "a, bb, ccc");
}

TEST_CASE("strings::contains")
{
	SUBCASE("contains")
	{
		CHECK(lib::strings::contains("aa,bb,cc", "aa"));
	}

	SUBCASE("does not contain")
	{
		CHECK_FALSE(lib::strings::contains("aa,bb,cc", "dd"));
	}
}

TEST_CASE("strings::trim")
{
	auto verify_trimmed = [](std::string &str, const char *expected = "aa")
	{
		CHECK_EQ(lib::strings::trim(str), "aa");
		CHECK_EQ(str, "aa");
	};

	SUBCASE("left")
	{
		std::string str("  aa");
		verify_trimmed(str);
	}

	SUBCASE("right")
	{
		std::string str("aa  ");
		verify_trimmed(str);
	}

	SUBCASE("both")
	{
		std::string str("  aa  ");
		verify_trimmed(str);
	}
}

TEST_CASE("strings::split")
{
	SUBCASE("with delimiter")
	{
		auto expected = std::array<std::string, 3>{
			"a", "bb", "ccc",
		};
		auto results = lib::strings::split("a,bb,ccc", ',');

		CHECK_EQ(results.size(), expected.size());
		for (auto i = 0; i < results.size(); i++)
		{
			CHECK_EQ(results[i], expected[i]);
		}
	}

	SUBCASE("without delimiter")
	{
		auto results = lib::strings::split("a,bb,ccc", '.');
		CHECK_EQ(results.size(), 1);
		CHECK_EQ(results[0], "a,bb,ccc");
	}

	SUBCASE("empty string")
	{
		auto results = lib::strings::split(std::string(), ',');
		CHECK_EQ(results.size(), 1);
		CHECK(results[0].empty());
	}

	SUBCASE("long delimiter")
	{
		// with delimiter
		auto expected = std::array<std::string, 3>{
			"a", "bb", "ccc",
		};
		auto results = lib::strings::split("a, bb, ccc", ", ");

		CHECK_EQ(results.size(), expected.size());
		for (auto i = 0; i < results.size(); i++)
		{
			CHECK_EQ(results[i], expected[i]);
		}

		// without delimiter
		CHECK_EQ(lib::strings::split("a, bb, ccc", ". ").at(0), "a, bb, ccc");
		// empty string
		CHECK(lib::strings::split(std::string(), ";").at(0).empty());
	}
}

TEST_CASE("strings::starts_with")
{
	SUBCASE("starts with")
	{
		CHECK(lib::strings::starts_with("aa,bb,cc", "aa"));
	}

	SUBCASE("does not start with")
	{
		CHECK_FALSE(lib::strings::starts_with("aa,bb,cc", "ab"));
	}
}

TEST_CASE("strings::remove")
{
	auto verify_removed = [](std::string &str, const std::string &substr,
		const std::string &expected)
	{
		CHECK_EQ(lib::strings::remove(str, substr), expected);
		CHECK_EQ(str, expected);
	};

	SUBCASE("start")
	{
		std::string str("aa,bb,cc");
		verify_removed(str, "aa", ",bb,cc");
	}

	SUBCASE("middle")
	{
		std::string str("aa,bb,cc");
		verify_removed(str, "bb", "aa,,cc");
	}

	SUBCASE("end")
	{
		std::string str("aa,bb,cc");
		verify_removed(str, "cc", "aa,bb,");
	}
}

TEST_CASE("strings::ends_with")
{
	SUBCASE("ends with")
	{
		CHECK(lib::strings::ends_with("aa,bb,cc", "cc"));
	}

	SUBCASE("does not end with")
	{
		CHECK_FALSE(lib::strings::ends_with("aa,bb,cc", "bc"));
	}
}

TEST_CASE("strings::try_to_int")
{
	SUBCASE("is int")
	{
		auto i = 0;
		CHECK(lib::strings::try_to_int("10", i));
		CHECK_EQ(i, 10);
	}

	SUBCASE("is not int")
	{
		auto i = 0;
		CHECK_FALSE(lib::strings::try_to_int("a", i));
		CHECK_EQ(i, 0);
	}

	SUBCASE("overflow")
	{
		int i = 0;
		CHECK_THROWS(lib::strings::try_to_int(std::to_string(INT64_MAX), i));
		CHECK_EQ(i, 0);
	}
}

TEST_CASE("strings::left")
{
	SUBCASE("contains")
	{
		CHECK_EQ(lib::strings::left("aa,bb,cc", 2), "aa");
	}

	SUBCASE("too long")
	{
		CHECK_EQ(lib::strings::left("aa", 5), "aa");
	}
}

TEST_CASE("strings::right")
{
	SUBCASE("contains")
	{
		CHECK_EQ(lib::strings::right("aa,bb,cc", 2), "cc");
	}

	SUBCASE("too long")
	{
		CHECK_EQ(lib::strings::right("aa", 5), "aa");
	}
}

TEST_CASE("strings::to_lower")
{
	std::string str("aa,BB,Cc");
	CHECK_EQ(lib::strings::to_lower(str), "aa,bb,cc");
}

TEST_CASE("strings::to_upper")
{
	std::string str("aa,BB,Cc");
	CHECK_EQ(lib::strings::to_upper(str), "AA,BB,CC");
}

TEST_CASE("strings::capitalize")
{
	CHECK_EQ(lib::strings::capitalize("Hello World"), "Hello world");
	CHECK_EQ(lib::strings::capitalize("HELLO WORLD"), "Hello world");
	CHECK_EQ(lib::strings::capitalize("hello world"), "Hello world");
}

TEST_CASE("strings::index_of")
{
	CHECK_EQ(lib::strings::index_of("aa,bb,aa", "aa"), 0);
	CHECK_EQ(lib::strings::index_of("aa,bb,aa", "bb"), 3);
	CHECK_EQ(lib::strings::index_of("aa,bb,aa", "cc"), -1);
}

TEST_CASE("strings::last_index_of")
{
	CHECK_EQ(lib::strings::last_index_of("aa,bb,aa", "aa"), 6);
	CHECK_EQ(lib::strings::last_index_of("aa,bb,aa", "bb"), 3);
	CHECK_EQ(lib::strings::last_index_of("aa,bb,aa", "cc"), -1);
}

TEST_CASE("strings::to_string")
{
	constexpr double pi = 3.14159265;

	CHECK_EQ(lib::strings::to_string(pi, 0), "3");
	CHECK_EQ(lib::strings::to_string(pi, 3), "3.142");
}
