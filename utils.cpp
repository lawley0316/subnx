#include "utils.h"
#include <random>
#include <cstring>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>

void str::split(const std::string& str, char sep, std::vector<std::string>& substrs) {
    std::string::size_type i = 0;
    std::string::size_type j = str.find(sep);
    while (j != std::string::npos) {
        substrs.push_back(str.substr(i, j-i));
        i = j + 1;
        j = str.find(sep, i);
    }
    substrs.push_back(str.substr(i));
}

void str::split(const std::string& str, const std::string& sep, std::vector<std::string>& substrs) {
    if (sep.empty()) {
        substrs.push_back(str);
        return;
    }
    std::string::size_type i = 0;
    std::string::size_type j = str.find(sep);
    std::string::size_type l = sep.length();
    while (j != std::string::npos) {
        substrs.push_back(str.substr(i, j-i));
        i = j + l;
        j = str.find(sep, i);
    }
    substrs.push_back(str.substr(i));
}

void str::join(const std::vector<std::string>& strs, const std::string& sep, std::string& str) {
    str.clear();
    for (auto it=strs.begin(); it!=strs.end(); ++it) {
        str += *it;
        if (it != strs.end() - 1) {
            str += sep;
        }
    }
}

bool str::startswith(const std::string& str, const char* prefix) {
    if (strlen(prefix) == 0) return true;
    return str.rfind(prefix, 0) == 0;
}

bool str::startswith(const std::string& str, const std::string& prefix) {
    if (prefix.empty()) return true;
    return str.rfind(prefix, 0) == 0;
}

void str::replace(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    std::string::size_type pos = str.find(from);
    while (pos != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos = str.find(from, pos+to.length());
    }
}

void str::upper(std::basic_string<char>& str) {
    for (auto it=str.begin(); it!=str.end(); ++it) {
        *it = toupper(*it);
    }
}

void str::upper(std::basic_string<wchar_t>& str) {
    for (auto it=str.begin(); it!=str.end(); ++it) {
        *it = towupper(*it);
    }
}

void str::lower(std::basic_string<char>& str) {
    for (auto it=str.begin(); it!=str.end(); ++it) {
        *it = tolower(*it);
    }
}

void str::lower(std::basic_string<wchar_t>& str) {
    for (auto it=str.begin(); it!=str.end(); ++it) {
        *it = towlower(*it);
    }
}

std::string str::random(std::size_t n) {
    if (n == 0) return "";
    std::string str(n, 0);
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, str::ALPHANUM.length()-1);
    std::generate_n(str.begin(), n, [&]() -> char {
        std::size_t i = dist(gen);
        return str::ALPHANUM[i];
    });
    return str;
}

bool os::path::exists(const std::string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string os::path::join(std::initializer_list<std::string> paths) {
    if (paths.size() == 0) return "";
    std::string result = *paths.begin();
    for (auto it=paths.begin()+1; it!=paths.end(); ++it) {
        if (it->empty()) continue;
        if (it->front() == SEP) {
            result = *it;
        } else if (result.empty() || (result.back() == SEP)) {
            result += *it;
        } else {
            result += SEP + *it;
        }
    }
    return result;
}
