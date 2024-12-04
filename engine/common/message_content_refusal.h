#pragma once

#include "common/message_content.h"

namespace ThreadMessage {
// The refusal content generated by the assistant.
struct Refusal : Content {

  // Always refusal.
  Refusal(const std::string& refusal) : Content("refusal"), refusal{refusal} {}

  Refusal(Refusal&&) noexcept = default;

  Refusal& operator=(Refusal&&) noexcept = default;

  Refusal(const Refusal&) = delete;

  Refusal& operator=(const Refusal&) = delete;

  std::string refusal;

  static cpp::result<Refusal, std::string> FromJson(Json::Value&& json) {
    if (json.empty()) {
      return cpp::fail("Json string is empty");
    }

    try {
      Refusal content{std::move(json["refusal"].asString())};
      return content;
    } catch (const std::exception& e) {
      return cpp::fail(std::string("FromJson failed: ") + e.what());
    }
  }

  cpp::result<Json::Value, std::string> ToJson() override {
    try {
      Json::Value json;
      json["type"] = type;
      json["refusal"] = refusal;
      return json;
    } catch (const std::exception& e) {
      return cpp::fail(std::string("ToJson failed: ") + e.what());
    }
  }
};
}  // namespace ThreadMessage
