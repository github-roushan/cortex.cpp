#include <yaml-cpp/yaml.h>
#include <limits>
#include "gtest/gtest.h"
#include "utils/format_utils.h"

class FormatUtilsTest : public ::testing::Test {};

TEST_F(FormatUtilsTest, WriteKeyValue) {
  {
    YAML::Node node;
    std::string result = format_utils::writeKeyValue("key", node["does_not_exist"]);
    EXPECT_EQ(result, "");
  }

  {
    YAML::Node node = YAML::Load("value");
    std::string result = format_utils::writeKeyValue("key", node);
    EXPECT_EQ(result, "key: value\n");
  }

  {
    YAML::Node node = YAML::Load("3.14159");
    std::string result = format_utils::writeKeyValue("key", node);
    EXPECT_EQ(result, "key: 3.14159\n");
  }

  {
    YAML::Node node = YAML::Load("3.000000");
    std::string result = format_utils::writeKeyValue("key", node);
    EXPECT_EQ(result, "key: 3\n");
  }

  {
    YAML::Node node = YAML::Load("3.140000");
    std::string result = format_utils::writeKeyValue("key", node);
    EXPECT_EQ(result, "key: 3.14\n");
  }

  {
    YAML::Node node = YAML::Load("value");
    std::string result = format_utils::writeKeyValue("key", node, "comment");
    EXPECT_EQ(result, "key: value # comment\n");
  }
}

TEST_F(FormatUtilsTest, BytesToHumanReadable) {
  {
    uint64_t bytes = 500;
    std::string result = format_utils::BytesToHumanReadable(bytes);
    EXPECT_EQ(result, "500.00 B");
  }

  {
    uint64_t bytes = 1500;
    std::string result = format_utils::BytesToHumanReadable(bytes);
    EXPECT_EQ(result, "1.46 KB");
  }

  {
    uint64_t bytes = 1500000;
    std::string result = format_utils::BytesToHumanReadable(bytes);
    EXPECT_EQ(result, "1.43 MB");
  }

  {
    uint64_t bytes = 1500000000;
    std::string result = format_utils::BytesToHumanReadable(bytes);
    EXPECT_EQ(result, "1.40 GB");
  }

  {
    uint64_t bytes = 1500000000000;
    std::string result = format_utils::BytesToHumanReadable(bytes);
    EXPECT_EQ(result, "1.36 TB");
  }
}

TEST_F(FormatUtilsTest, PrintComment) {
  std::string result = format_utils::print_comment("Test comment");
  EXPECT_EQ(result, "\033[1;90m# Test comment\033[0m\n");
}

TEST_F(FormatUtilsTest, PrintKV) {
  std::string result = format_utils::print_kv("key", "value");
  EXPECT_EQ(result, "\033[1;32mkey:\033[0m \033[0mvalue\033[0m\n");

  std::string result2 = format_utils::print_kv("key", "value", "\033[0;34m");
  EXPECT_EQ(result2, "\033[1;32mkey:\033[0m \033[0;34mvalue\033[0m\n");
}

TEST_F(FormatUtilsTest, PrintBool) {
  std::string result = format_utils::print_bool("key", true);
  EXPECT_EQ(result, "\033[1;32mkey:\033[0m \033[0;35mtrue\033[0m\n");

  result = format_utils::print_bool("key", false);
  EXPECT_EQ(result, "\033[1;32mkey:\033[0m \033[0;35mfalse\033[0m\n");
}

TEST_F(FormatUtilsTest, PrintFloat) {
  std::string result = format_utils::print_float("key", 3.14159f);
  EXPECT_EQ(result, "\033[1;32mkey:\033[0m \033[0;34m3.14159\033[0m\n");

  result = format_utils::print_float("key", 3.000f);
  EXPECT_EQ(result, "\033[1;32mkey:\033[0m \033[0;34m3\033[0m\n");

  result = format_utils::print_float("key", 3.14000f);
  EXPECT_EQ(result, "\033[1;32mkey:\033[0m \033[0;34m3.14\033[0m\n");

  result =
      format_utils::print_float("key", std::numeric_limits<float>::quiet_NaN());
  EXPECT_EQ(result, "");
}