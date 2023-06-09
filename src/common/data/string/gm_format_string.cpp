#include "gm_format_string.hpp"

#include "gm_string.hpp"
#include "gm_string_buffer.hpp"
#include "../file/gm_logger.hpp"

#include <cstdarg>
#include <cstring>

namespace game {
    template <typename T>
    UTF8Str FormatString::_intToStr(T val, uint8_t base, int64_t minDigits, const int32_t flags) noexcept {
        char* str = static_cast<char*>(std::malloc(MAX_DIGITS + sizeof("-.E18") + minDigits));
        bool isNeg = val < 0;
        int64_t len = 0;

        // Input validation
        if (base <= 1 || base > 36) base = 10;

        // Parse
        uint8_t digits = 0;
        if (val == 0) {
            digits = 1;
            if (flags & FORMAT_SIGNED) str[len++] = '+';
            else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';
            str[len++] = '0';

            // Prepend with padding if necessary
            if ((digits < minDigits) && !(flags & FORMAT_LEFT_JUSTIFIED)) {
                if ((flags & FORMAT_ZERO_PADDED) && !(flags & FORMAT_SCIENTIFIC)) {
                    do {
                        str[len++] = '0';
                        digits++;
                    }
                    while (digits < minDigits);
                } else {
                    do {
                        str[len++] = ' ';
                        digits++;
                    }
                    while (digits < minDigits);
                }
            }
        } else {
            if (isNeg) val *= -1;

            // Write each digit
            const char hexFormatChar = (flags & FORMAT_UPPERCASE) ? 'A' : 'a';
            T rem;
            do {
                digits++;
                rem = val % base;
                str[len++] = (rem < 10) ? (rem + '0') : ((rem - 10) + hexFormatChar);
                val /= base;
            } while (val != 0);

            // Prepend with padding if necessary
            if ((digits < minDigits) && !(flags & FORMAT_LEFT_JUSTIFIED)) {
                if ((flags & FORMAT_ZERO_PADDED) && !(flags & FORMAT_SCIENTIFIC)) {
                    do {
                        str[len++] = '0';
                        digits++;
                    }
                    while (digits < minDigits);
                } else {
                    do {
                        str[len++] = ' ';
                        digits++;
                    }
                    while (digits < minDigits);
                }
            }
            
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
            if (isNeg) str[len++] = '-';
            else if (flags & FORMAT_SIGNED) str[len++] = '+';
            else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';

            String::reverse(str, len);
        }
        
        if (flags & FORMAT_SCIENTIFIC) {
            UTF8Str exponentStr = _intToStr(digits - 1, base, 0, 0);

            // Insert characters
            String::insert(str, '.', isNeg ? 2 : 1);
            len++;
            str[len++] = (flags & FORMAT_UPPERCASE) ? 'E' : 'e';
            
            // Copy to string
            std::memcpy(str + len, exponentStr.get(), exponentStr.length());
            len += exponentStr.length();
        }
        
        // Append with padding as necessary (left justified)
        if (flags & FORMAT_LEFT_JUSTIFIED) {
            while (len < minDigits) str[len++] = ' ';
        }

        str[len] = '\0';
        str = static_cast<char*>(std::realloc(str, len + 1));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }

    template <typename T>
    UTF8Str FormatString::_floatToStrScientific(const T absVal, const bool isNeg, const uint8_t base,
        const int64_t precision, const int64_t minDigits, const int32_t flags) noexcept
    {
        char* str = static_cast<char*>(std::malloc(MAX_DIGITS + sizeof("-.E-9999") + precision + minDigits));
        int64_t len = 0;
        
        // Find exponent
        int16_t exponent = ((absVal == 0.0) || (absVal == 1.0)) ?
            0 : static_cast<int16_t>(std::floor(std::log(absVal) / std::log(base)));

        // Test for very close values (ie. 99.999... giving 2 instead of 1 due to rounding)
        T powVal = absVal * std::pow(base, -exponent);
        if (powVal > base) {
            exponent--;
            powVal = absVal * std::pow(base, -exponent);
        } else if ((powVal < 1) && (absVal != 0.0)) {
            exponent++;
            powVal = absVal * std::pow(base, -exponent);
        }

        // Prepend if required
        if (!(flags & FORMAT_ZERO_PADDED) && !(flags & FORMAT_LEFT_JUSTIFIED)) {
            const int16_t prependCount = minDigits - (exponent < 0 ? -exponent : exponent) + 1;
            for (int16_t i = 0; i < prependCount; i++) {
                str[len++] = ' ';
            }
        }

        // Add sign if required
        if (isNeg) str[len++] = '-';
        else {
            if (flags & FORMAT_SIGNED) str[len++] = '+';
            else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';
        }

        // Add tag if required
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

        // Write digits
        uint16_t digits = 0;
        const char hexChar = (flags & FORMAT_UPPERCASE) ? 'A' : 'a';
        char digit;
        do {
            // Compute the next digit and insert it into the buffer
            digit = static_cast<char>(std::floor(powVal));
            digits++;
            str[len++] = (digit < 10) ? (digit + '0') : ((digit - 10) + hexChar);
            powVal -= digit;
            powVal *= base;
            
            // Place decimal as required
            if (digits == 1) str[len++] = '.';
        } while (digits < precision);

        // Add zero padding as required
        if ((flags & FORMAT_ZERO_PADDED) && (digits < minDigits)) {
            while (digits < minDigits) {
                str[len++] = '0';
                digits++;
            }
        } else if (flags & FORMAT_TRUNCATE_ZEROES) {
            // Truncate if required
            while (str[len - 1] == '0') {
                digits--;
                len--;
            }
        }
        
        // Append exponent
        str[len++] = (flags & FORMAT_UPPERCASE) ? 'E' : 'e';
        if (exponent >= 0) str[len++] = '+';
        UTF8Str exponentStr = _intToStr(exponent, base, 0, flags & FORMAT_UPPERCASE);
        std::memcpy(str + len, exponentStr.get(), exponentStr.length());
        len += exponentStr.length();

        // Add padding if required
        if (flags & FORMAT_LEFT_JUSTIFIED) {
            while (len < minDigits) str[len++] = ' ';
        }

        str[len] = '\0';
        str = static_cast<char*>(std::realloc(str, len + 1));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }

    template <typename T>
    UTF8Str FormatString::_floatToStrDecimal(const T absVal, const bool isNeg, const uint8_t base,
        const int64_t precision, const int64_t minDigits, const int32_t flags) noexcept
    {
        int64_t len = 0;

        // Find integer part
        const int64_t integer = static_cast<int64_t>(std::floor(absVal));

        // Find decimal part
        T decimal = absVal - static_cast<T>(integer);
        char* str = static_cast<char*>(std::malloc(MAX_DIGITS + sizeof("-.E-99") + precision + minDigits));

        // Make 'precision' digits of decimal an integer
        decimal *= std::pow(base, precision);

        // Get decimal part
        const UTF8Str decimalStr = _intToStr(static_cast<int64_t>(std::floor(decimal)), base, 0, flags & FORMAT_UPPERCASE);

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

            if (precision == 0) { // No character is written for the value 0
                str[len++] = '.';
            } else {
                str[len++] = '0';
                str[len++] = '.';
                if (!(flags & FORMAT_TRUNCATE_ZEROES)) {
                    for (int64_t i = 0; i < precision; i++) str[len++] = '0';
                }
            }

            str[len] = '\0';
            str = static_cast<char*>(std::realloc(str, len + 1));
            return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
        }
        
        // Get integer part
        const UTF8Str intStr = _intToStr(integer, base, (flags & FORMAT_LEFT_JUSTIFIED) ? 0 : minDigits,
            flags & (FORMAT_UPPERCASE | FORMAT_TAGGED));
        if (isNeg) str[len++] = '-';
        else if (flags & FORMAT_SIGNED) str[len++] = '+';
        else if (flags & FORMAT_SIGN_PADDING) str[len++] = ' ';
        std::memcpy(str + len, intStr.get(), intStr.length());
        len += intStr.length();

        // Only add decimal if needed
        if ((precision > 0) || (flags & FORMAT_TAGGED) ||
            !((decimalStr.length() == 1) && (decimalStr.get()[0] == '0'))
        ) {
            str[len++] = '.';

            // Add missing zeroes between decimal and first non-zero digit
            while (zeroes--) str[len++] = '0';
            
            // Copy decimal
            std::memcpy(str + len, decimalStr.get(), decimalStr.length());
            len += decimalStr.length();

            if (flags & FORMAT_TRUNCATE_ZEROES) {
                // Truncate trailing 0's (precision overshot)
                while (str[len - 1] == '0') len--;
            }
        } else if (flags & FORMAT_TAGGED) str[len++] = '.';
        
        // Add padding if required
        if (flags & FORMAT_LEFT_JUSTIFIED) {
            while (len < minDigits) str[len++] = ' ';
        }

        str[len] = '\0';
        str = static_cast<char*>(std::realloc(str, len + 1));
        return UTF8Str{len, std::shared_ptr<const char>(str, std::free)};
    }
    
    template <typename T>
    UTF8Str FormatString::_floatToStr(T val, uint8_t base, int64_t precision, int64_t minDigits, const int32_t flags) noexcept {
        const bool isNeg = val < 0;
        T absN = isNeg ? -val : val;
        
        // Input validation
        if (std::isnan(val)) {
            return UTF8Str{sizeof("NaN") - 1, std::shared_ptr<const char>("NaN", [](const char*){})};
        } else if (std::isinf(val)) {
            return UTF8Str{sizeof("Inf") - 1, std::shared_ptr<const char>("Inf", [](const char*){})};
        }
        if (precision > MAX_DIGITS) precision = 6;
        if (base <= 1 || base > 36) base = 10;

        // Handle scientific notation
        // Use scientific notation for very large or small values that may overflow int64_t
        if ((flags & FORMAT_SCIENTIFIC) ||
            (absN > INT64_MAX) ||
            ((absN < 0.0000000000000001) && (absN > 0))
        ) return _floatToStrScientific(absN, isNeg, base, precision, minDigits, flags);
        else return _floatToStrDecimal(absN, isNeg, base, precision, minDigits, flags);
    }

    UTF8Str FormatString::boolToStr(const bool boolean, const StringCases caseType) noexcept {
        char* str = static_cast<char*>(std::malloc(sizeof("False")));
        int64_t len;

        switch (caseType) {
            case LOWERCASE:
                if (boolean) {
                    len = sizeof("true") - 1;
                    std::memcpy(str, "true", len);
                } else {
                    len = sizeof("false") - 1;
                    std::memcpy(str, "false", len);
                }
            break;
            case UPPERCASE_ALL:
                if (boolean) {
                    len = sizeof("TRUE") - 1;
                    std::memcpy(str, "TRUE", len);
                } else {
                    len = sizeof("FALSE") - 1;
                    std::memcpy(str, "FALSE", len);
                }
            break;
            case UPPERCASE_FIRST:
            default:
                if (boolean) {
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

    UTF8Str FormatString::ptrToStr(const void* ptr, const int32_t flags) noexcept {
        if (!ptr) {
            return UTF8Str{sizeof("(NULL)") - 1, std::shared_ptr<const char>("(NULL)", [](const char*){})};
        }

        const size_t ptrVal = reinterpret_cast<const size_t>(ptr);
        return _intToStr(ptrVal, 16, sizeof(ptr), flags & FORMAT_TAGGED);
    }
    
    bool FormatString::_formatStringFormat(const char c, va_list& args, char*& dst,
        int64_t& capacity, int64_t& len, int& flags, int64_t& minDigits, int64_t& precision)
    {
        switch (c) {
            // Minimum characters
            case '.': {
                flags |= FORMAT_PRECISION;
                precision = 0; // Reset from default of 6
            } break;
            case '*': {
                if (flags & FORMAT_PRECISION) precision = va_arg(args, int64_t);
                else minDigits = va_arg(args, int64_t);
            } break;
            case '0': {
                if (flags & FORMAT_PRECISION) precision *= 10;
                else {
                    if (minDigits != 0) minDigits *= 10;
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
                    precision += 3;
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
                    UTF8Str valStr = FormatString::_intToStr(val, 10, std::max(minDigits, precision), flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    int32_t val = va_arg(args, int32_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 10, std::max(minDigits, precision), flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'u': { // Unsigned int
                if (flags & FORMAT_LONG) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 10, std::max(minDigits, precision), flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 10, std::max(minDigits, precision), flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'q': { // Signed int scientific notation
                if (flags & FORMAT_LONG) {
                    int64_t val = va_arg(args, int64_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 10, std::max(minDigits, precision),
                        flags | FORMAT_SCIENTIFIC);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    int32_t val = va_arg(args, int32_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 10, std::max(minDigits, precision),
                        flags | FORMAT_SCIENTIFIC);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'Q': { // Signed int scientific notation uppercase
                if (flags & FORMAT_LONG) {
                    int64_t val = va_arg(args, int64_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 10, std::max(minDigits, precision),
                        flags | FORMAT_SCIENTIFIC_UPPERCASE);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    int32_t val = va_arg(args, int32_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 10, std::max(minDigits, precision),
                        flags | FORMAT_SCIENTIFIC_UPPERCASE);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'o': { // Unsigned octal
                if (flags & FORMAT_LONG) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 8, std::max(minDigits, precision), flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 8, std::max(minDigits, precision), flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'x': { // Hex
                if (flags & FORMAT_LONG) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 16, std::max(minDigits, precision), flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 16, std::max(minDigits, precision), flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'X': { // Hex uppercase
                if (flags & FORMAT_LONG) {
                    uint64_t val = va_arg(args, uint64_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 16, std::max(minDigits, precision), flags | FORMAT_UPPERCASE);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    uint32_t val = va_arg(args, uint32_t);
                    UTF8Str valStr = FormatString::_intToStr(val, 16, std::max(minDigits, precision), flags | FORMAT_UPPERCASE);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'f':
            case 'F': { // Float
                if (flags & FORMAT_LONG) {
                    float128_t val = va_arg(args, float128_t);
                    if (!(flags & FORMAT_PRECISION)) precision = 6;
                    UTF8Str valStr = FormatString::_floatToStr(val, 10, precision, minDigits, flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    float64_t val = va_arg(args, float64_t);
                    if (!(flags & FORMAT_PRECISION)) precision = 6;
                    UTF8Str valStr = FormatString::_floatToStr(val, 10, precision, minDigits, flags);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'e': { // Scientific notation
                float128_t val = va_arg(args, float128_t);
                if (!(flags & FORMAT_PRECISION)) precision = 6;
                UTF8Str valStr = FormatString::_floatToStr(val, 10, precision, minDigits, flags | FORMAT_SCIENTIFIC_LOWERCASE);
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'E': { // Scientific notation uppercase
                float128_t val = va_arg(args, float128_t);
                if (!(flags & FORMAT_PRECISION)) precision = 6;
                UTF8Str valStr = FormatString::_floatToStr(val, 10, precision, minDigits, flags | FORMAT_SCIENTIFIC_UPPERCASE);
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'g': { // Shortest of %f or %e
                float128_t val = va_arg(args, float128_t);
                if (!(flags & FORMAT_PRECISION)) precision = 6;
                int64_t exponent = static_cast<int64_t>(std::ceil(std::log10(std::fabs(val))));
                exponent = (exponent < 0) ? -exponent : exponent;
                UTF8Str valStr = FormatString::_floatToStr(val, 10, precision, minDigits,
                    exponent > static_cast<int64_t>(precision) ? (flags | FORMAT_SCIENTIFIC_LOWERCASE) : flags
                );
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'G': { // Shortest of %F or %E
                float128_t val = va_arg(args, float128_t);
                if (!(flags & FORMAT_PRECISION)) precision = 6;
                int64_t exponent = static_cast<int64_t>(std::ceil(std::log10(std::fabs(val))));
                exponent = (exponent < 0) ? -exponent : exponent;
                UTF8Str valStr = FormatString::_floatToStr(val, 10, precision, minDigits,
                    exponent > static_cast<int64_t>(precision) ? (flags | FORMAT_SCIENTIFIC_UPPERCASE) : flags
                );
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'a': { // Signed hex float
                float128_t val = va_arg(args, float128_t);
                if (!(flags & FORMAT_PRECISION)) precision = 6;
                UTF8Str valStr = FormatString::_floatToStr(val, 16, precision, minDigits, flags);
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'A': { // Signed hex float uppercase
                float128_t val = va_arg(args, float128_t);
                if (!(flags & FORMAT_PRECISION)) precision = 6;
                UTF8Str valStr = FormatString::_floatToStr(val, 16, precision, minDigits, flags | FORMAT_UPPERCASE);
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'c': { // Character
                char c = static_cast<char>(va_arg(args, int));
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + 1, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                dst[len++] = c;
            } return false;
            case 's': { // String
                const char* str = va_arg(args, char*);
                const int64_t strLen = precision ?
                    std::min(std::strlen(str), static_cast<size_t>(precision)) : std::strlen(str);
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + strLen, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, str, strLen);
                len += strLen;
            } return false;
            case 'p': { // Pointer address
                const void* ptr = va_arg(args, void*);
                UTF8Str valStr = FormatString::ptrToStr(ptr, flags);
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'b': { // Boolean
                const bool boolean = static_cast<bool>(va_arg(args, int));
                UTF8Str valStr = FormatString::boolToStr(boolean);
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                } catch (std::runtime_error& e) { throw; }
                std::memcpy(dst + len, valStr.get(), valStr.length());
                len += valStr.length();
            } return false;
            case 'B': { // Uppercase boolean
                const bool boolean = static_cast<bool>(va_arg(args, int));
                if (flags & FORMAT_LONG) {
                    UTF8Str valStr = FormatString::boolToStr(boolean, UPPERCASE_ALL);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                } else {
                    UTF8Str valStr = FormatString::boolToStr(boolean, UPPERCASE_FIRST);
                    try {
                        StringBuffer::checkResizeNoFormat(dst, len + valStr.length(), len, capacity);
                    } catch (std::runtime_error& e) { throw; }
                    std::memcpy(dst + len, valStr.get(), valStr.length());
                    len += valStr.length();
                }
            } return false;
            case 'n': { // Store currently written characters in variable (signed int); print nothing
                    int64_t* storage = va_arg(args, int64_t*);
                    if (storage) *storage = len;
            } return false;

            // Characters
            case '%': { // '%'
                try {
                    StringBuffer::checkResizeNoFormat(dst, len + 1, len, capacity);
                } catch (std::runtime_error& e) { throw; }
                dst[len++] = '%';
            } return false;
            default: { // Throw
                throw std::runtime_error("Unexpected format character in formatString().");
            }
        }
        return true;
    }

    UTF8Str FormatString::formatString(const char*__restrict__ str, ...) noexcept {
        va_list args;
        va_start(args, str);

        // Create buffer
        const int64_t strLen = static_cast<int64_t>(std::strlen(str));
        int64_t capacity = strLen + 1;
        int64_t len = 0;
        char* dst = static_cast<char*>(std::malloc(capacity));

        // Parse
        char c;
        bool formatChar = false;
        int64_t minDigits = 0, precision = 0;
        int32_t flags = 0;
        for (int64_t i = 0; i < strLen; i++) {
            c = str[i];
            if (formatChar) {
                try {
                    formatChar = _formatStringFormat(c, args, dst, capacity, len, flags, minDigits, precision);
                } catch (std::runtime_error& e) { Logger::crash(e.what()); }
                if (!formatChar) {
                    minDigits = 0;
                    precision = 0;
                    flags = 0;
                }
            } else {
                switch (c) {
                    case '%': { // Format
                        formatChar = true;
                    } break;
                    default: { // Copy to dst
                        try {
                            StringBuffer::checkResizeNoFormat(dst, len + 1, len, capacity);
                        } catch (std::runtime_error& e) { Logger::crash(e.what()); }
                        dst[len++] = c;
                    } break;
                }
            }
        }

        // Close
        va_end(args);
        dst[len] = '\0';
        dst = static_cast<char*>(std::realloc(dst, len + 1));
        return UTF8Str{len, std::shared_ptr<const char>(dst, std::free)};
    }

    bool FormatString::strToBool(const char*__restrict__ str, const int64_t len) {
        if ((len == 4) &&
            ((str[0] == 't') || (str[0] == 'T')) &&
            ((str[1] == 'r') || (str[1] == 'R')) &&
            ((str[2] == 'u') || (str[2] == 'U')) &&
            ((str[3] == 'e') || (str[3] == 'E'))
        ) return true;
        
        return false;
    }

    template <typename T>
    T FormatString::_strToInt(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        T result = 0, oldResult = 0;
        uint8_t digit;
        char c;
        for (int64_t i = 0; i < len; i++) {
            c = str[i];
            if (std::isalnum(c)) {
                if (std::isdigit(c)) {
                    digit = c - '0';
                } else if ((c >= 'a') || (c <= 'z')) {
                    digit = c - 'a' + 10;
                } else if ((c >= 'A') || (c <= 'Z')) {
                    digit = c - 'A' + 10;
                }

                // Not our base
                if (digit >= base) {
                    UTF8Str msg = formatString("Unexpected character (%02x) in strToInt(): %s", c, str);
                    throw std::runtime_error(msg.get());
                }

                oldResult = result;
                result = (result * base) + digit;

                // Test for overflow
                if (result < oldResult) {
                    UTF8Str msg = formatString("Integer overflows strToInt(): %s", str);
                    throw std::runtime_error(msg.get());
                }
            } else if (!((c == '-') || (c == '+')) && (i == 0)) {
                UTF8Str msg = formatString("Unexpected character (%02x) in strToInt(): %s", c, str);
                throw std::runtime_error(msg.get());
            }
        }

        return result;
    }
    
    template <typename T>
    T FormatString::_strToFloat(const char*__restrict__ str, const uint8_t base, const int64_t len) {
        T result = static_cast<T>(0), fraction = result;
        uint8_t digit;
        bool hasFraction = false;
        int16_t exponent = 0;
        char c;
        for (int64_t i = 0; i < len; i++) {
            c = str[i];
            if (std::isalnum(c)) {
                if (std::isdigit(c)) {
                    digit = c - '0';
                } else if ((c >= 'a') || (c <= 'z')) {
                    digit = c - 'a' + 10;
                } else if ((c >= 'A') || (c <= 'Z')) {
                    digit = c - 'A' + 10;
                }

                // Not our base
                if (digit >= base) {
                    UTF8Str msg = formatString("Unexpected character (%02x) in strToFloat(): %s", c, str);
                    throw std::runtime_error(msg.get());
                }

                if (hasFraction) {
                    fraction = (fraction * static_cast<float32_t>(base)) + static_cast<float32_t>(digit);
                    exponent--;
                } else {
                    result = (result * static_cast<float32_t>(base)) + static_cast<float32_t>(digit);
                }
            } else if (c == '.') {
                hasFraction = true;
            } else if (!((c == '-') || (c == '+')) && (i == 0)) {
                UTF8Str msg = formatString("Unexpected character (%02x) in strToFloat(): %s", c, str);
                throw std::runtime_error(msg.get());
            }
        }

        result += fraction / std::pow(base, -exponent);

        // Test for overflow
        if (std::isinf(result)) {
            UTF8Str msg = formatString("Float overflows strToFloat(): %s", str);
            throw std::runtime_error(msg.get());
        }
        
        // Check for negative values
        if (str[0] == '-') result = -result;

        return result;
    }
    
    int8_t FormatString::_strToInt8(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        int8_t result = _strToInt<int8_t>(str, base, len);
        
        // Check for negative values
        if (str[0] == '-') result = -result;

        return result;
    }
    uint8_t FormatString::_strToIntU8(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        return _strToInt<uint8_t>(str, base, len);
    }
    int16_t FormatString::_strToInt16(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        int16_t result = _strToInt<int16_t>(str, base, len);
        
        // Check for negative values
        if (str[0] == '-') result = -result;

        return result;
    }
    uint16_t FormatString::_strToIntU16(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        return _strToInt<uint16_t>(str, base, len);
    }
    int32_t FormatString::_strToInt32(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        int32_t result = _strToInt<int32_t>(str, base, len);

        // Check for negative values
        if (str[0] == '-') result = -result;

        return result;
    }
    uint32_t FormatString::_strToIntU32(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        return _strToInt<uint32_t>(str, base, len);
    }
    int64_t FormatString::_strToInt64(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        int64_t result = _strToInt<int64_t>(str, base, len);

        // Check for negative values
        if (str[0] == '-') result = -result;

        return result;
    }
    uint64_t FormatString::_strToIntU64(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        return _strToInt<uint64_t>(str, base, len);
    }

    float32_t FormatString::_strToFloat32(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        return _strToFloat<float32_t>(str, base, len);
    }
    float64_t FormatString::_strToFloat64(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        return _strToFloat<float64_t>(str, base, len);
    }
    float128_t FormatString::_strToFloat128(const char *__restrict__ str, const uint8_t base, const int64_t len) {
        return _strToFloat<float128_t>(str, base, len);
    }
    
    UTF8Str FormatString::_intToStr8(int8_t val, uint8_t base, int64_t minDigits) {
        return _intToStr(val, base, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_intToStrU8(uint8_t val, uint8_t base, int64_t minDigits) {
        return _intToStr(val, base, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_intToStr16(int16_t val, uint8_t base, int64_t minDigits) {
        return _intToStr(val, base, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_intToStrU16(uint16_t val, uint8_t base, int64_t minDigits) {
        return _intToStr<uint16_t>(val, base, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_intToStr32(int32_t val, uint8_t base, int64_t minDigits) {
        return _intToStr(val, base, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_intToStrU32(uint32_t val, uint8_t base, int64_t minDigits) {
        return _intToStr<uint32_t>(val, base, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_intToStr64(int64_t val, uint8_t base, int64_t minDigits) {
        return _intToStr(val, base, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_intToStrU64(uint64_t val, uint8_t base, int64_t minDigits) {
        return _intToStr<uint64_t>(val, base, minDigits, FORMAT_DIGITAL);
    }
    
    UTF8Str FormatString::_floatToStr32(float32_t val, uint8_t base, int64_t precision, int64_t minDigits) {
        return _floatToStr(val, base, precision, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_floatToStr64(float64_t val, uint8_t base, int64_t precision, int64_t minDigits) {
        return _floatToStr(val, base, precision, minDigits, FORMAT_DIGITAL);
    }
    UTF8Str FormatString::_floatToStr128(float128_t val, uint8_t base, int64_t precision, int64_t minDigits) {
        return _floatToStr(val, base, precision, minDigits, FORMAT_DIGITAL);
    }
}
