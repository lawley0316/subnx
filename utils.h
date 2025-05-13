// Utilities
#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H
#include <vector>
#include <string>

namespace str {
    // 常用字符串常量
    static const std::string DIGITS = "0123456789";
    static const std::string LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
    static const std::string UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static const std::string LETTERS = LOWERCASE + UPPERCASE;
    static const std::string ALPHANUM = DIGITS + LETTERS;

    // 字符串基本函数
    void split(const std::string& str, char sep, std::vector<std::string>& substrs);
    void split(const std::string& str, const std::string& sep, std::vector<std::string>& substrs);
    void join(const std::vector<std::string>& strs, const std::string& sep, std::string& str);
    bool startswith(const std::string& str, const char* prefix);
    bool startswith(const std::string& str, const std::string& prefix);
    void replace(std::string& str, const std::string& from, const std::string& to);
    void upper(std::basic_string<char>& str);
    void upper(std::basic_string<wchar_t>& str);
    void lower(std::basic_string<char>& str);
    void lower(std::basic_string<wchar_t>& str);
    std::string random(std::size_t n);
} // namespace str

namespace os {
    namespace path {
        static const char WIN_SEP = '\\';
        static const char UNIX_SEP = '/';
#ifdef _WIN32
        static const char SEP = WIN_SEP;
#else
        static const char SEP = UNIX_SEP;
#endif
        bool exists(const std::string& path);
        std::string join(std::initializer_list<std::string> paths);
    }
}

#endif //UTILS_UTILS_H
