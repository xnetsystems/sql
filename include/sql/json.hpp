#pragma once
#include <nlohmann/json.hpp>
#include <sql/statement.hpp>
#include <ice/base.hpp>

namespace nlohmann {

inline void to_json(json& j, sql::statement v)
{
  j = ice::json::array();
  for (const auto& row : v) {
    auto entry = ice::json::object();
    for (std::size_t i = 0, max = row.column_count(); i < max; i++) {
      const auto name = std::string(row.column_name(i));
      switch (row.type(i)) {
      case sql::type::null:
        entry[name] = nullptr;
        break;
      case sql::type::integer:
        entry[name] = row.get<long long>(i);
        break;
      case sql::type::real:
        entry[name] = row.get<double>(i);
        break;
      case sql::type::text:
        entry[name] = row.get<std::string>(i);
        break;
      case sql::type::blob:
        entry[name] = ice::base::encode(row.get<std::string>(i));
        break;
      }
    }
    j.push_back(std::move(entry));
  }
}

}  // namespace nlohmann