#pragma once
#include <date/date.h>
#include <iomanip>
#include <ratio>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstdio>

namespace date {

template <typename Rep, typename Period>
inline std::string format(std::chrono::duration<Rep, Period> value) {
  std::ostringstream oss;
  if (value < std::chrono::duration<Rep, Period>(0)) {
    value = -value;
    oss << '-';
  }
  const auto h = std::chrono::duration_cast<std::chrono::hours>(value);
  const auto m = std::chrono::duration_cast<std::chrono::minutes>(value - h);
  const auto s = std::chrono::duration_cast<std::chrono::seconds>(value - h - m);
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(value - h - m - s);
  oss << std::setfill('0')
      << std::setw(2) << h.count() << ':'
      << std::setw(2) << m.count() << ':'
      << std::setw(2) << s.count() << '.'
      << std::setw(3) << ms.count();
  return oss.str();
}

template <typename Rep, typename Period>
inline std::chrono::duration<Rep, Period> parse_duration(const std::string& str) {
  auto ptr = str.data();
  bool negative = false;
  if (ptr[0] == '-') {
    negative = true;
    ++ptr;
  }
  long long h = 0;
  unsigned char m = 0;
  unsigned char s = 0;
  unsigned char ms = 0;
  if (std::sscanf(ptr, "%lld:%hhu:%hhu.%hhu", &h, &m, &s, &ms) < 3) {
    throw std::domain_error("invalid duration format: \"" + str + "\"");
  }
  auto value = std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
    std::chrono::hours(h < 0 ? -h : h) + std::chrono::minutes(m) +
    std::chrono::seconds(s) + std::chrono::milliseconds(ms));
  return negative ? -value : value;
}

template <typename Clock, typename Duration>
inline std::string format(const std::chrono::time_point<Clock, Duration>& value) {
  if (std::ratio_less_v<typename Duration::period, days::period>) {
    return format("%F %T", value);
  }
  return format("%F", value);
}

template <typename Clock, typename Duration>
inline std::chrono::time_point<Clock, Duration> parse_time_point(const std::string& str) {
  std::istringstream iss(str);
  std::chrono::time_point<Clock, Duration> value;
  if (std::ratio_less_v<typename Duration::period, days::period>) {
    from_stream(iss, "%F %T", value);
  } else {
    from_stream(iss, "%F", value);
  }
  if (!iss) {
    throw std::domain_error("invalid time point format: \"" + str + "\"");
  }
  return value;
}

inline weekday parse_weekday(const std::string& str) {
  if (str.size() < 3) {
    throw std::domain_error("invalid weekday format: \"" + str + "\"");
  }
  switch (str[0]) {
  case 'S': return str[1] == 'u' ? sun : sat;
  case 'M': return mon;
  case 'T': return str[1] == 'u' ? tue : thu;
  case 'W': return wed;
  case 'F': return fri;
  default: throw std::domain_error("invalid weekday value: \"" + str + "\"");
  }
  return {};
}

inline month parse_month(const std::string& str) {
  if (str.size() < 3) {
    throw std::domain_error("invalid month format: \"" + str + "\"");
  }
  switch (str[0]) {
  case 'J': return str[1] == 'a' ? jan : str[2] == 'n' ? jun : jul;
  case 'F': return feb;
  case 'M': return str[2] == 'r' ? mar : may;
  case 'A': return str[1] == 'p' ? apr : aug;
  case 'S': return sep;
  case 'O': return oct;
  case 'N': return nov;
  case 'D': return dec;
  default: throw std::domain_error("invalid month value: \"" + str + "\"");
  }
  return {};
}

inline weekday_indexed parse_weekday_indexed(const std::string& str) {
  if (str.size() < 6 || str[3] != '[') {
    throw std::domain_error("invalid weekday[index] format: \"" + str + "\"");
  }
  unsigned i = 0;
  if (std::sscanf(str.data() + 3, "[%u]", &i) != 1) {
    throw std::domain_error("invalid weekday[index] value: \"" + str + "\"");
  }
  return parse_weekday(str)[i];
}

inline weekday_last parse_weekday_last(const std::string& str) {
  if (str.size() < 9 || str.compare(3, 6, "[last]") != 0) {
    throw std::domain_error("invalid weekday[last] format: \"" + str + "\"");
  }
  return parse_weekday(str)[last];
}

inline month_day parse_month_day(const std::string& str) {
  if (str.size() < 5 || str[3] != '/') {
    throw std::domain_error("invalid month/day format: \"" + str + "\"");
  }
  unsigned d = 0;
  if (std::sscanf(str.data() + 3, "/%u", &d) != 1) {
    throw std::domain_error("invalid month/day value: \"" + str + "\"");
  }
  return parse_month(str) / d;
}

inline month_day_last parse_month_day_last(const std::string& str) {
  if (str.size() < 8 || str.compare(3, 5, "/last") != 0) {
    throw std::domain_error("invalid month/last format: \"" + str + "\"");
  }
  return parse_month(str) / last;
}

inline month_weekday parse_month_weekday(const std::string& str) {
  if (str.size() < 10 || str[3] != '/' || str[7] != '[') {
    throw std::domain_error("invalid month/weekday[index] format: \"" + str + "\"");
  }
  return parse_month(str) / parse_weekday_indexed(str.substr(4));
}

inline month_weekday_last parse_month_weekday_last(const std::string& str) {
  if (str.size() < 13 || str[3] != '/' || str.compare(7, 6, "[last]") != 0) {
    throw std::domain_error("invalid month/weekday[last] format: \"" + str + "\"");
  }
  return parse_month(str) / parse_weekday(str.substr(4, 3))[last];
}

inline year_month parse_year_month(const std::string& str) {
  std::string m;
  m.resize(3);
  int y = 0;
  if (std::sscanf(str.data(), "%d/%c%c%c", &y, &m[0], &m[1], &m[2]) != 4) {
    throw std::domain_error("invalid year/month format: \"" + str + "\"");
  }
  return year(y) / parse_month(m);
}

inline year_month_day parse_year_month_day(const std::string& str) {
  int y = 0;
  unsigned m = 0;
  unsigned d = 0;
  if (std::sscanf(str.data(), "%d-%u-%u", &y, &m, &d) != 3) {
    throw std::domain_error("invalid year-month-day format: \"" + str + "\"");
  }
  return year(y) / m / d;
}

inline year_month_day_last parse_year_month_day_last(const std::string& str) {
  int y = 0;
  auto pos = str.find('/');
  if (pos == std::string::npos || std::sscanf(str.data(), "%d", &y) != 1) {
    throw std::domain_error("invalid year/month/last format: \"" + str + "\"");
  }
  return year(y) / parse_month_day_last(str.substr(pos + 1));
}

inline year_month_weekday parse_year_month_weekday(const std::string& str) {
  int y = 0;
  auto pos = str.find('/');
  if (pos == std::string::npos || std::sscanf(str.data(), "%d", &y) != 1) {
    throw std::domain_error("invalid year/month/weekday[index] format: \"" + str + "\"");
  }
  return year(y) / parse_month_weekday(str.substr(pos + 1));
}

inline year_month_weekday_last parse_year_month_weekday_last(const std::string& str) {
  int y = 0;
  auto pos = str.find('/');
  if (pos == std::string::npos || std::sscanf(str.data(), "%d", &y) != 1) {
    throw std::domain_error("invalid year/month/weekday[last] format: \"" + str + "\"");
  }
  return year(y) / parse_month_weekday_last(str.substr(pos + 1));
}

}  // namespace date
