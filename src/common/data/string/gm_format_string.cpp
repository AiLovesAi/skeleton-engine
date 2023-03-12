#include "gm_format_string.hpp"

#include "gm_string.hpp"

#include <cmath>
#include <cstdarg>
#include <cstring>

namespace game {
    template <typename T>
    UTF8Str FormatString::toStr(T n, uint8_t base, const uint8_t minDigits, const NumberFormat numberFormat) {
        char* str = static_cast<char*>(std::malloc(sizeof("-9.223372036854775807E18")));
        bool neg = false;
        int64_t len = 0;

        if (base == 0 || base > 36) base = 10;

        uint8_t digits = 0;
        if (n == 0) {
            digits = 1;
            str[len++] = '0';
            while (len < minDigits) str[len++] = '0';
            str[len] = '\0';
        } else {
            if (n < 0) {
                neg = true;
                n = -n;
            }

            // Write each digit
            const char hexFormatChar = (numberFormat & FORMAT_UPPERCASE) ? 'A' : 'a';
            T rem;
            do {
                digits++;
                rem = n % base;
                str[len++] = (rem < 9) ? ((rem - 10) + hexFormatChar) : (rem + '0');
                n /= base;
            } while (n != 0);
            
            // Add leading 0's (minDigits specified)
            while (len < minDigits) str[len++] = '0';
            
            if (neg) str[len++] = '-';
            str[len] = '\0';

            String::reverse(str, len);
        }
        
        if (numberFormat & FORMAT_SCIENTIFIC) {
            UTF8Str digitsStr = toStr(digits - 1, base);

            // Insert characters
            String::insert(str, '.', neg ? 2 : 1);
            len++;
            str[len++] = (numberFormat & FORMAT_UPPERCASE) ? 'E' : 'e';
            
            // Copy to string
            std::memcpy(str + len, digitsStr.get(), digitsStr.length());
            len += digitsStr.length();
            str[len] = '\0';
        }
        
        str = static_cast<char*>(std::realloc(str, len));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }
    
    UTF8Str FormatString::toStr(double n, uint8_t base, uint8_t precision, const uint8_t minDigits, const NumberFormat numberFormat) {
        const bool neg = n < 0;
        int32_t integer = static_cast<int32_t>(std::floor(n));
        if (neg) n *= -1;
        double decimal = n - static_cast<double>(integer);
        int64_t len = 0;

        if (precision > 15) precision = 15;

        // Make 'precision' digits of decimal an integer
        if (integer != 0) {
            int32_t temp = integer;
            int8_t digits = 0;
            while (temp != 0) {
                digits++;
                temp /= base;
            }
            precision -= digits;
        }
        decimal *= std::pow(base, precision);

        // Get decimal part
        const UTF8Str decimalStr = toStr(static_cast<int32_t>(std::floor(decimal)), base);

        // Find zeroes between decimal and end
        // NOTE: When the decimal is raised to precision, it will first get rid of the leading zeroes,
        // then the decimal, then add trailing zeroes. This means that the only numbers missing from
        // the result are the number of zeroes we cut out.
        int8_t zeroes = precision - decimalStr.length();

        // Specific parsing
        char* str = static_cast<char*>(std::malloc(sizeof("-9.223372036854775807E-15")));
        if ((numberFormat & FORMAT_SCIENTIFIC) && (integer == 0)) { // E-x
            // Get decimal part
            if (decimalStr.get()[0] == '0') { // 0.0E0
                std::free(str);
                const char* zeroStr = (numberFormat & FORMAT_UPPERCASE) ? "0.0E0" : "0.0e0";
                return UTF8Str{sizeof(zeroStr) - 1, std::shared_ptr<const char>(zeroStr, [](const char*){})};
            }

            // Copy to buffer
            if (neg) str[len++] = '-';
            std::memcpy(str + len, decimalStr.get(), decimalStr.length());
            len += decimalStr.length();
            
            // Truncate trailing 0's (precision overshot)
            while (str[len - 1] == '0') len--;

            // Add trailing 0's (minDigits specified)
            while (len < minDigits) str[len++] = '0';

            // Find what power to raise E to
            UTF8Str powerStr = toStr(zeroes ? zeroes + 1 : 1, base);

            // Insert characters
            String::insert(str, '.', neg ? 2 : 1);
            len++;
            str[len++] = (numberFormat & FORMAT_UPPERCASE) ? 'E' : 'e';
            str[len++] = '-';

            std::memcpy(str + len, powerStr.get(), powerStr.length());
            len += powerStr.length();
        } else {
            // Get integer part
            const UTF8Str intStr = toStr(integer, base, minDigits);
            std::memcpy(str, intStr.get(), intStr.length());
            len += intStr.length();

            if (numberFormat & FORMAT_SCIENTIFIC) { // E+x
                String::insert(str, '.', neg ? 2 : 1);
                len++;
                
                // Copy decimal
                std::memcpy(str + len, decimalStr.get(), decimalStr.length());
                len += decimalStr.length();
                
                // Truncate trailing 0's (precision overshot)
                while (str[len - 1] == '0') len--;

                // Add trailing 0's (minDigits specified)
                while (len < minDigits) str[len++] = '0';
                
                // Find what power to raise E to
                UTF8Str powerStr = toStr(zeroes ? zeroes + 1 : 1, base);
                str[len++] = (numberFormat & FORMAT_UPPERCASE) ? 'E' : 'e';

                std::memcpy(str + len, powerStr.get(), powerStr.length());
                len += powerStr.length();
            } else {
                str[len++] = '.';

                // Add missing zeroes
                while (zeroes--) str[len++] = '0';
                
                // Copy decimal
                std::memcpy(str + len, decimalStr.get(), decimalStr.length());
                len += decimalStr.length();

                // Truncate trailing 0's (precision overshot)
                while (str[len - 1] == '0') len--;

                // Add trailing 0's (minDigits specified)
                while (len < minDigits) str[len++] = '0';
            }
        }
        
        str[len] = '\0';
        str = static_cast<char*>(std::realloc(str, len));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }

    UTF8Str FormatString::toStr(const bool b, const StringCases caseType) {
        char* str = static_cast<char*>(std::malloc(sizeof("False")));
        int64_t len;

        switch (caseType) {
            case LOWERCASE:
                if (b) {
                    len = sizeof("true") - 1;
                    std::memcpy(str, "true", len);
                } else {
                    len = sizeof("false") - 1;
                    std::memcpy(str, "false", len);
                }
            break;
            case UPPERCASE_ALL:
                if (b) {
                    len = sizeof("TRUE") - 1;
                    std::memcpy(str, "TRUE", len);
                } else {
                    len = sizeof("FALSE") - 1;
                    std::memcpy(str, "FALSE", len);
                }
            break;
            case UPPERCASE_FIRST:
            default:
                if (b) {
                    len = sizeof("True") - 1;
                    std::memcpy(str, "True", len);
                } else {
                    len = sizeof("False") - 1;
                    std::memcpy(str, "False", len);
                }
            break;
        }

        str[len] = '\0';
        str = static_cast<char*>(std::realloc(str, len + 1));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }
    
    void FormatString::formatStringFormat(const char c, va_list& args, char*& dst,
        int64_t& capacity, int64_t& len, bool& longChar)
    {
        switch (c) { // TODO %02lu
            case 'l':
            case 'L': {
                longChar = true;
            } break;
            case 'd':
            case 'i': { // Signed int
                if (longChar) {
                    int64_t val = va_arg(args, int64_t);
                    UTF8Str valStr = FormatString::toStr(val, 10);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    int32_t val = va_arg(args, int32_t);
                    UTF8Str valStr = FormatString::toStr(val, 10);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } break;
            case 'u': { // Unsigned int
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::toStr(val, 10);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::toStr(val, 10);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } break;
            case 'o': { // Unsigned octal
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::toStr(val, 8);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::toStr(val, 8);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } break;
            case 'x': { // Hex
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::toStr(val, 16);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::toStr(val, 16);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } break;
            case 'X': { // Hex uppercase
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::toStr(val, 16, 0, FORMAT_UPPERCASE);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::toStr(val, 16, 0, FORMAT_UPPERCASE);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } break;
            case 'f':
            case 'F': { // Float
                double val = va_arg(args, double);
                UTF8Str valStr = FormatString::toStr(val, 10);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } break;
            case 'e': { // Scientific notation
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::toStr(val, 10, 0, FORMAT_SCIENTIFIC_LOWERCASE);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::toStr(val, 10, 0, FORMAT_SCIENTIFIC_LOWERCASE);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } break;
            case 'E': { // Scientific notation uppercase
                if (longChar) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::toStr(val, 10, 0, FORMAT_SCIENTIFIC_UPPERCASE);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::toStr(val, 10, 0, FORMAT_SCIENTIFIC_UPPERCASE);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } break;
            case 'g': { // Shortest of %f or %e
                // TODO
            } break;
            case 'G': { // Shortest of %F or %E
                // TODO
            } break;
            case 'a': { // Signed hex float
                double val = va_arg(args, double);
                UTF8Str valStr = FormatString::toStr(val, 16);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } break;
            case 'A': { // Signed hex float uppercase
                double val = va_arg(args, double);
                UTF8Str valStr = FormatString::toStr(val, 16, 0, 0, FORMAT_UPPERCASE);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } break;
            case 'c': { // Character
                char c = static_cast<char>(va_arg(args, int));
                try {
                    String::checkResize_(dst, len + 1, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                dst[len++] = c;
            } break;
            case 's': { // String
                const char* str = va_arg(args, char*);
                int64_t strLen = std::strlen(str);
                
                try {
                    String::checkResize_(dst, len + strLen, len, capacity);
                } catch (std::runtime_error& e) { throw; }

                std::memcpy(dst, str, strLen);
                len += strLen;
            } break;
            case 'p': { // Pointer address
                // TODO
            } break;
            case 'n': { // Store currently written characters in variable (signed int); print nothing
                    size_t* storage = va_arg(args, size_t*);
                    if (storage) *storage = len;
            } break;
            case '%': { // '%'
                try {
                    String::checkResize_(dst, len + 1, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                dst[len++] = '%';
            } break;
            default: { // Throw
                throw std::runtime_error("Unexpected format character in formatString().");
            }
        }
    }

    UTF8Str FormatString::formatString(const char* str, ...) {
                va_list args;
                va_start(args, str);

                const int64_t strLen = static_cast<int64_t>(std::strlen(str));
                int64_t capacity = strLen + 1;
                int64_t len = 0;
                char* dst = static_cast<char*>(std::malloc(capacity));

                char c;
                bool formatChar = false;
                bool longChar = false;
                for (int64_t i = 0; i < strLen; i++) {
                    c = str[i];
                    if (formatChar) {
                        formatStringFormat(c, args, dst, capacity, len, longChar);
                    } else {
                        switch (c) {
                            case '%': { // Format
                                formatChar = true;
                            } break;
                            default: { // Copy to dst
                                try {
                                    String::checkResize_(dst, len + 1, len, capacity);
                                } catch (std::runtime_error& e) { throw; }
                                dst[len++] = c;
                            } break;
                        }
                    }
                }

                // TODO

                va_end(args);
                return UTF8Str{len, std::shared_ptr<const char>(dst, std::free)};
    }
}
