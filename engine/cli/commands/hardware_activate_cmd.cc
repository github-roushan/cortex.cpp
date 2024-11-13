#include "hardware_activate_cmd.h"
#include "server_start_cmd.h"
#include "utils/json_helper.h"
#include "utils/logging_utils.h"

namespace commands {
namespace {
std::vector<int> ParseStringToVector(const std::string& str) {
  // [0, 1, 2, 3]
  std::string cleaned_str =
      std::regex_replace(str, std::regex(R"([\[\]\s])"), "");

  // Prepare to parse the cleaned string
  std::vector<int> result;
  std::stringstream ss(cleaned_str);
  std::string number;

  // Use getline to split by comma
  while (std::getline(ss, number, ',')) {
    result.push_back(std::stoi(number));
  }

  return result;
}
}  // namespace

bool HardwareActivateCmd::Exec(
    const std::string& host, int port,
    const std::unordered_map<std::string, std::string>& options) {
  // Start server if server is not started yet
  if (!commands::IsServerAlive(host, port)) {
    CLI_LOG("Starting server ...");
    commands::ServerStartCmd ssc;
    if (!ssc.Exec(host, port)) {
      return false;
    }
  }

  // TODO(sang) should use curl but it does not work (?)
  Json::Value body;
  Json::Value gpus_json = Json::arrayValue;
  std::vector<int> gpus;
  for (auto const& [key, value] : options) {
    if (key == "gpus") {
      gpus = ParseStringToVector(value);
    }
  }
  for (auto g : gpus) {
    gpus_json.append(g);
  }
  body["gpus"] = gpus_json;
  auto data_str = body.toStyledString();

  httplib::Client cli(host + ":" + std::to_string(port));

  auto res = cli.Post("/v1/hardware/activate", httplib::Headers(),
                      data_str.data(), data_str.size(), "application/json");
  if (res) {
    if (res->status == httplib::StatusCode::OK_200) {
      auto root = json_helper::ParseJsonString(res->body);
      if (!root["warning"].isNull()) {
        CLI_LOG(root["warning"].asString());
      }
      if(body["gpus"].empty()) {
        CLI_LOG("Deactivated all GPUs!");        
      } else {
        std::string gpus_str;
        for(auto i: gpus) {
            gpus_str += " " + std::to_string(i);
        }
        CLI_LOG("Activated GPUs:" << gpus_str);
      }
      return true;
    } else {
      auto root = json_helper::ParseJsonString(res->body);
      CLI_LOG(root["message"].asString());
      return false;
    }
  } else {
    auto err = res.error();
    CTL_ERR("HTTP error: " << httplib::to_string(err));
    return false;
  }
  return true;
}
}  // namespace commands