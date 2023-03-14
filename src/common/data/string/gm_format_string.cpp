#include "gm_format_string.hpp"

#include "gm_string.hpp"

#include <cmath>
#include <cstdarg>
#include <cstring>

namespace game {
    template <typename T>
    UTF8Str FormatString::intToStr(T n, uint8_t base, size_t minDigits, const int32_t flags) noexcept {
        char* str = static_cast<char*>(std::malloc(MAX_DIGITS + sizeof("-.E18")));
        bool neg = n < 0;
        int64_t len = 0;

        // Input validation
        if (base == 0 || base > 36) base = 10;
        if (minDigits > MAX_DIGITS) minDigits = MAX_DIGITS;

        // Parse
        uint8_t digits = 0;
        if (n == 0) {
            digits = 1;
            if (flags & FORMAT_SIGNED) str[len++] = '+';
            else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';
            str[len++] = '0';
        } else {
            if (neg) n *= -1;

            // Write each digit
            const char hexFormatChar = (flags & FORMAT_UPPERCASE) ? 'A' : 'a';
            T rem;
            do {
                digits++;
                rem = n % base;
                str[len++] = (rem < 9) ? ((rem - 10) + hexFormatChar) : (rem + '0');
                n /= base;
            } while (n != 0);
            
            // Add tag
            if (flags & FORMAT_TAGGED) {
                switch (base) {
                    case 16: {
                        str[len++] = 'x';
                        str[len++] = '0';
                    } break;
                    case 8: {
                        str[len++] = '0';
                    } break;
                }
            }

            // Add sign
            if (neg) str[len++] = '-';
            else if (flags & FORMAT_SIGNED) str[len++] = '+';
            else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';

            String::reverse(str, len);
        }
        
        if (flags & FORMAT_SCIENTIFIC) {
            UTF8Str digitsStr = toStr(digits - 1, base);

            // Insert characters
            String::insert(str, '.', neg ? 2 : 1);
            len++;
            str[len++] = (flags & FORMAT_UPPERCASE) ? 'E' : 'e';
            
            // Copy to string
            std::memcpy(str + len, digitsStr.get(), digitsStr.length());
            len += digitsStr.length();
        }
        
        if (digits < minDigits) {
            if (flags & FORMAT_LEFT_JUSTIFIED) {
                while (digits < minDigits) {
                    str[len++] = ' ';
                    digits++;
                }
            } else if ((flags & FORMAT_ZERO_PADDED) && !(flags & FORMAT_SCIENTIFIC)) {
                const char zeroes[] = "000000000000000000";
                String::prepend(str, len, zeroes, minDigits - digits);
            } else {
                const char zeroes[] = "                  ";
                String::prepend(str, len, zeroes, minDigits - digits);
            }
        }
        
        str = static_cast<char*>(std::realloc(str, len));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }
    
    template <typename T>
    UTF8Str FormatString::floatToStr(T n, uint8_t base, size_t precision, size_t minDigits, const int32_t flags) noexcept {
        const bool neg = n < 0;
        int64_t len = 0;
        int32_t integer;
        
        // Find integer part
        if (!neg && n > static_cast<T>(INT64_MAX)) {
            // TODO Allow for very large floats ex. E100
        } else if (neg && n < static_cast<T>(INT64_MAX)) {
            // TODO Allow for very small floats ex. E-100
        } else {
            integer = static_cast<int32_t>(std::floor(n));
        }

        // Input validation
        if (precision > MAX_DIGITS) precision = 6;

        // Find decimal part
        if (neg) n *= -1;
        T decimal = n - static_cast<T>(integer);
        char* str = static_cast<char*>(std::malloc(MAX_DIGITS + sizeof("-.E-999")));

        // Make 'precision' digits of decimal an integer
        decimal *= std::pow(base, precision);

        // Get decimal part
        const UTF8Str decimalStr = intToStr(static_cast<int32_t>(std::floor(decimal)), base, 0, flags & FORMAT_UPPERCASE);

        // Find zeroes between decimal and end
        // NOTE: When the decimal is raised to precision, it will first get rid of the leading zeroes,
        // then the decimal, then add trailing zeroes. This means that the only numbers missing from
        // the result are the number of zeroes we cut out.
        int8_t zeroes = precision - decimalStr.length();

        // 0 value test
        if (integer == 0 && decimalStr.length() == 1 && decimalStr.get()[0] == '0') {
            // Add sign
            if (flags & FORMAT_SIGNED) str[len++] = '+';
            else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';

            if (flags & FORMAT_SCIENTIFIC) {
                if (base == 16) {
                    const char* zeroStr = (flags & FORMAT_UPPERCASE) ? "0.0P0" : "0.0p0";
                    len += sizeof(zeroStr) - 1;
                    std::memcpy(str + len, zeroStr, len);
                } else {
                    const char* zeroStr = (flags & FORMAT_UPPERCASE) ? "0.0E0" : "0.0e0";
                    len += sizeof(zeroStr) - 1;
                    std::memcpy(str + len, zeroStr, len);
                }
            } else if (precision == 0) { // No character is written for the value 0
                str[len++] = '.';
            } else {
                str[len++] = '0';
                str[len++] = '.';
                if (!(flags & FORMAT_TRUNCATE_ZEROES)) {
                    for (size_t i = 0; i < precision; i++) str[len++] = '0';
                }
            }
            str = static_cast<char*>(std::realloc(str, len));
            return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
        }
        
        // Parse
        if ((flags & FORMAT_SCIENTIFIC) && (integer == 0)) { // E-x
            // Add sign
            if (neg) str[len++] = '-';
            else if (flags & FORMAT_SIGNED) str[len++] = '+';
            else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';

            // Copy to buffer
            std::memcpy(str + len, decimalStr.get(), decimalStr.length());
            len += decimalStr.length();
            
            // Truncate trailing 0's (precision overshot)
            while (str[len - 1] == '0') len--;

            // Find what power to raise E to
            UTF8Str powerStr = intToStr(zeroes ? zeroes + 1 : 1, base, 0, flags & FORMAT_UPPERCASE);

            // Insert characters
            String::insert(str, '.', neg ? 2 : 1);
            len++;
            if (base == 16) {
                str[len++] = (flags & FORMAT_UPPERCASE) ? 'P' : 'p';
            } else {
                str[len++] = (flags & FORMAT_UPPERCASE) ? 'E' : 'e';
            }
            str[len++] = '-';

            std::memcpy(str + len, powerStr.get(), powerStr.length());
            len += powerStr.length();
        } else {
            // Get integer part
            const UTF8Str intStr = intToStr(integer, base, minDigits, flags & (FORMAT_UPPERCASE | FORMAT_TAGGED));
            if (flags & FORMAT_SIGNED) str[len++] = '+';
            else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';
            std::memcpy(str, intStr.get(), intStr.length());
            len += intStr.length();

            if (flags & FORMAT_SCIENTIFIC) { // E+x
                // Only add decimal if needed
                if (intStr.length() > 1 || !((decimalStr.length() == 1) && (decimalStr.get()[0] == '0'))) {
                    String::insert(str, '.', neg ? 2 : 1);
                    len++;
                    
                    // Copy decimal
                    std::memcpy(str + len, decimalStr.get(), decimalStr.length());
                    len += decimalStr.length();
                    
                    if (flags & FORMAT_TRUNCATE_ZEROES) {
                        // Truncate trailing 0's (precision overshot)
                        while (str[len - 1] == '0') len--;
                    }
                } else if (flags & FORMAT_TAGGED) str[len++] = '.';
                
                // Find what power to raise E to
                UTF8Str powerStr = intToStr(zeroes ? zeroes + 1 : 1, base, 0, flags & FORMAT_UPPERCASE);
                if (base == 16) {
                    str[len++] = (flags & FORMAT_UPPERCASE) ? 'P' : 'p';
                } else {
                    str[len++] = (flags & FORMAT_UPPERCASE) ? 'E' : 'e';
                }

                std::memcpy(str + len, powerStr.get(), powerStr.length());
                len += powerStr.length();
            } else {
                // Only add decimal if needed
                if (precision > 0 || !((decimalStr.length() == 1) && (decimalStr.get()[0] == '0'))) {
                    str[len++] = '.';

                    // Add missing zeroes
                    while (zeroes--) str[len++] = '0';
                    
                    // Copy decimal
                    std::memcpy(str + len, decimalStr.get(), decimalStr.length());
                    len += decimalStr.length();

                    if (flags & FORMAT_TRUNCATE_ZEROES) {
                        // Truncate trailing 0's (precision overshot)
                        while (str[len - 1] == '0') len--;
                    }
                } else if (flags & FORMAT_TAGGED) str[len++] = '.';
            }
        }
        
        str = static_cast<char*>(std::realloc(str, len));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }

    UTF8Str FormatString::boolToStr(const bool b, const StringCases caseType) {
        char* str = static_cast<char*>(std::malloc(sizeof("False")));
        int64_t len;

        switch (caseType) {
            case LOWERCASE:
                if (b) {
                    const char trueStr[] = "true";
                    len = sizeof(trueStr) - 1;
                    std::memcpy(str, trueStr, len);
                } else {
                    const char falseStr[] = "false";
                    len = sizeof(falseStr) - 1;
                    std::memcpy(str, falseStr, len);
                }
            break;
            case UPPERCASE_ALL:
                if (b) {
                    const char trueStr[] = "TRUE";
                    len = sizeof(trueStr) - 1;
                    std::memcpy(str, trueStr, len);
                } else {
                    const char falseStr[] = "FALSE";
                    len = sizeof(falseStr) - 1;
                    std::memcpy(str, falseStr, len);
                }
            break;
            case UPPERCASE_FIRST:
            default:
                if (b) {
                    const char trueStr[] = "True";
                    len = sizeof(trueStr) - 1;
                    std::memcpy(str, trueStr, len);
                } else {
                    const char falseStr[] = "False";
                    len = sizeof(falseStr) - 1;
                    std::memcpy(str, falseStr, len);
                }
            break;
        }

        str = static_cast<char*>(std::realloc(str, len));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }

    UTF8Str FormatString::ptrToStr(const void* ptr, const int32_t flags) {
        if (!ptr) {
            const char ptrStr[] = "(NULL)";
            return UTF8Str{sizeof(ptrStr) - 1, std::shared_ptr<const char>(ptrStr, [](const char*){})};
        }

        const char* ptrStr = reinterpret_cast<const char*>(&ptr);
        char *str = static_cast<char*>(std::malloc(sizeof("0x") + sizeof(ptr)));
        int64_t len = 0;
        if (flags & FORMAT_TAGGED) {
            str[len++] = '0';
            str[len++] = 'x';
        }
        std::memcpy(str + len, ptrStr, sizeof(ptr));
        len += sizeof(ptr);
        str = static_cast<char*>(std::realloc(str, len));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }
    
    bool FormatString::formatStringFormat(const char c, va_list& args, char*& dst,
        int64_t& capacity, int64_t& len, int& flags, size_t& minDigits, size_t& precision)
    {
        switch (c) {
            // Minimum characters
            case '.': {
                flags |= FORMAT_PRECISION;
                precision = 0; // Reset from default of 6
            } break;
            case '*': {
                if (flags & FORMAT_PRECISION) precision = va_arg(args, size_t);
                else minDigits = va_arg(args, size_t);
            } break;
            case '0': {
                if (flags & FORMAT_PRECISION) precision *= 10;
                else {
                    if (minDigits == 0) minDigits *= 10;
                    else flags |= FORMAT_ZERO_PADDED;
                }
            } break;
            case '1': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 1;
                } else {
                    minDigits *= 10;
                    minDigits += 1;
                }
            } break;
            case '2': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 2;
                } else {
                    minDigits *= 10;
                    minDigits += 2;
                }
            } break;
            case '3': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 2;
                } else {
                    minDigits *= 10;
                    minDigits += 3;
                }
            } break;
            case '4': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 4;
                } else {
                    minDigits *= 10;
                    minDigits += 4;
                }
            } break;
            case '5': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 5;
                } else {
                    minDigits *= 10;
                    minDigits += 5;
                }
            } break;
            case '6': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 6;
                } else {
                    minDigits *= 10;
                    minDigits += 6;
                }
            } break;
            case '7': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 7;
                } else {
                    minDigits *= 10;
                    minDigits += 7;
                }
            } break;
            case '8': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 8;
                } else {
                    minDigits *= 10;
                    minDigits += 8;
                }
            } break;
            case '9': {
                if (flags & FORMAT_PRECISION) {
                    precision *= 10;
                    precision += 9;
                } else {
                    minDigits *= 10;
                    minDigits += 9;
                }
            } break;

            // Flags
            case 'l':
            case 'L': {
                flags |= FORMAT_LONG;
            } break;
            case '-': {
                flags |= FORMAT_LEFT_JUSTIFIED;
            } break;
            case '+': {
                flags |= FORMAT_SIGNED;
            } break;
            case '#': {
                flags |= FORMAT_TAGGED;
            } break;
            case '!': {
                flags |= FORMAT_TRUNCATE_ZEROES;
            } break;

            // Types
            case 'd':
            case 'i': { // Signed int
                if (flags & FORMAT_LONG) {
                    int64_t val = va_arg(args, int64_t);
                    UTF8Str valStr = FormatString::intToStr(val, 10, std::max(minDigits, precision));
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    int32_t val = va_arg(args, int32_t);
                    UTF8Str valStr = FormatString::intToStr(val, 10, std::max(minDigits, precision));
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'u': { // Unsigned int
                if (flags & FORMAT_LONG) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::intToStr(val, 10, std::max(minDigits, precision));
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::intToStr(val, 10, std::max(minDigits, precision));
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'o': { // Unsigned octal
                if (flags & FORMAT_LONG) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::intToStr(val, 8, std::max(minDigits, precision));
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::intToStr(val, 8, std::max(minDigits, precision));
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'x': { // Hex
                if (flags & FORMAT_LONG) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::intToStr(val, 16, std::max(minDigits, precision));
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::intToStr(val, 16, std::max(minDigits, precision));
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'X': { // Hex uppercase
                if (flags & FORMAT_LONG) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::intToStr(val, 16, std::max(minDigits, precision), FORMAT_UPPERCASE);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::intToStr(val, 16, std::max(minDigits, precision), FORMAT_UPPERCASE);
                    try {
                        String::checkResize_(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'f':
            case 'F': { // Float
                double val = va_arg(args, double);
                UTF8Str valStr = FormatString::floatToStr(val, 10, precision, minDigits);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'e': { // Scientific notation
                double val = va_arg(args, double);
                UTF8Str valStr = FormatString::floatToStr(val, 10, precision, minDigits, FORMAT_SCIENTIFIC_LOWERCASE);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'E': { // Scientific notation uppercase
                double val = va_arg(args, double);
                UTF8Str valStr = FormatString::floatToStr(val, 10, precision, minDigits, FORMAT_SCIENTIFIC_UPPERCASE);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'g': { // Shortest of %f or %e
                double val = va_arg(args, double);
                const int64_t exponent = static_cast<int64_t>(std::ceil(std::log10(std::fabs(val))));
                UTF8Str valStr = FormatString::floatToStr(val, 10, precision, minDigits,
                    exponent > precision ? FORMAT_SCIENTIFIC_LOWERCASE : 0
                );
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'G': { // Shortest of %F or %E
                double val = va_arg(args, double);
                const int64_t exponent = static_cast<int64_t>(std::ceil(std::log10(std::fabs(val))));
                UTF8Str valStr = FormatString::floatToStr(val, 10, precision, minDigits,
                    exponent > precision ? FORMAT_SCIENTIFIC_UPPERCASE : 0
                );
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'a': { // Signed hex float
                double val = va_arg(args, double);
                UTF8Str valStr = FormatString::floatToStr(val, 16, precision, minDigits);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'A': { // Signed hex float uppercase
                double val = va_arg(args, double);
                UTF8Str valStr = FormatString::floatToStr(val, 16, precision, minDigits, FORMAT_UPPERCASE);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'c': { // Character
                char c = static_cast<char>(va_arg(args, int));
                try {
                    String::checkResize_(dst, len + 1, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                dst[len++] = c;
            } return false;
            case 's': { // String
                const char* str = va_arg(args, char*);
                const int64_t strLen = precision ? std::min(std::strlen(str), precision) : std::strlen(str);
                
                try {
                    String::checkResize_(dst, len + strLen, len, capacity);
                } catch (std::runtime_error& e) { throw; }

                std::memcpy(dst + len, str, strLen);
                len += strLen;
            } return false;
            case 'p': { // Pointer address
                const void* ptr = va_arg(args, void*);
                UTF8Str valStr = FormatString::ptrToStr(ptr, flags);
                try {
                    String::checkResize_(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), sizeof(ptr));
                len += valStr.length();
            } return false;
            case 'n': { // Store currently written characters in variable (signed int); print nothing
                    size_t* storage = va_arg(args, size_t*);
                    if (storage) *storage = len;
            } return false;
            case '%': { // '%'
                try {
                    String::checkResize_(dst, len + 1, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                dst[len++] = '%';
            } return false;
            default: { // Throw
                throw std::runtime_error("Unexpected format character in formatString().");
            }
        }
        return true;
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
                size_t minDigits = 0, precision = 6;
                int32_t flags = 0;
                for (int64_t i = 0; i < strLen; i++) {
                    c = str[i];
                    if (formatChar) {
                        formatChar = formatStringFormat(c, args, dst, capacity, len, flags, minDigits, precision);
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
