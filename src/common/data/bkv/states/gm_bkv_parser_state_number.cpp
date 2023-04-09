#include "gm_bkv_parser_state_number.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../gm_endianness.hpp"

#include <cmath>

namespace game {
    template <typename T>
    void BKV_Parser_State_Number::_appendValue(BKV_Parser& parser, const T value) {
        const T val = Endianness::hton(value);
        try {
            StringBuffer::checkResize(parser._buffer._bkv, parser._buffer._head + (int64_t) sizeof(T),
                parser._buffer._head, parser._buffer._capacity
            );
        } catch (std::runtime_error &e) {
            reset();
            throw;
        }
        std::memcpy(parser._buffer._bkv + parser._buffer._head, &val, sizeof(T));
        parser._buffer._head += sizeof(T);
    }
    
    void BKV_Parser_State_Number::_endNumber(BKV_Parser& parser, const char c) {
        reset();

        if ((c == '}') || (c == ',') || (c == ']')) {
            try { parser.endKV(c); } catch (std::runtime_error &e) {
                reset();
                throw;
            }
        } else {
            UTF8Str msg = FormatString::formatString("Invalid character in SBKV number at index %ld: %02x",
                parser._charactersRead, c
            );
            throw std::runtime_error(msg.get());
        }
    }

    void BKV_Parser_State_Number::parse(BKV_Parser& parser, const char c) {
        parser._charactersRead++;
        
        if (std::isspace(c)) {
            // Whitespace, ignore
            return;
        } else if (parser._tag & ~BKV::BKV_FLAGS_ALL) {
            // Check if number has been completed and a tag is assigned
            try { _endNumber(parser, c); } catch (std::runtime_error& e) {
                reset();
                throw;
            }
        } else if (std::isdigit(c) || (c == '.' && !_hasDecimal) || (c == '-' && _numBuffer.len() == 0)) {
            // Build number string
            if (c == '.') _hasDecimal = true;
            else if (c == '-') _hasNegative = true;

            if (_numBuffer.len() >= UINT8_MAX) {
                UTF8Str msg = FormatString::formatString("Too many digits in SBKV number at index %ld: %lu/%lu digits.",
                    parser._charactersRead, (_numBuffer.len() + 1), UINT8_MAX
                );
                reset();
                throw std::runtime_error(msg.get());
            }

            try { _numBuffer.append(c); } catch (std::runtime_error& e) {
                reset();
                throw;
            }
        } else {
            // Search for type
            switch (c) {
                case ',':
                case ']':
                case '}': {
                    // Use integer if possible, long otherwise
                    if (!_numBuffer.len()) {
                        UTF8Str msg = FormatString::formatString("BKV number has no value at index %ld.", parser._charactersRead);
                        reset();
                        throw std::runtime_error(msg.get());
                    }
                    if (_hasDecimal) {
                        try {
                            parser._tag |= BKV::BKV_DOUBLE;
                            float128_t val = Endianness::htonf(FormatString::strToFloat<float128_t>(_numBuffer.get(), 10, _numBuffer.len()));
                            std::memcpy(parser._buffer._bkv + parser._buffer._head, &val, sizeof(float128_t));
                            parser._buffer._head += sizeof(float128_t);
                        } catch (std::runtime_error &e) {
                            reset();
                            throw;
                        }
                    } else {
                        if (parser._tag & BKV::BKV_UNSIGNED) {
                            uint64_t val = FormatString::strToUInt<uint64_t>(_numBuffer.get(), 10, _numBuffer.len());
                            // Make sure value doesn't overflow
                            if (val > UINT32_MAX) {
                                parser._tag |= BKV::BKV_I64;
                                try { _appendValue(parser, val); } catch (std::runtime_error &e) {
                                    reset();
                                    throw;
                                }
                            } else {
                                parser._tag |= BKV::BKV_I32;
                                try { _appendValue(parser, static_cast<uint32_t>(val)); } catch (std::runtime_error &e) {
                                    reset();
                                    throw;
                                }
                            }
                        } else {
                            int64_t val = FormatString::strToInt<int64_t>(_numBuffer.get(), 10, _numBuffer.len());
                            // Make sure value doesn't overflow or underflow
                            if ((val < INT32_MIN) || (val > INT32_MAX)) {
                                parser._tag |= BKV::BKV_I64;
                                try { _appendValue(parser, val); } catch (std::runtime_error &e) {
                                    reset();
                                    throw;
                                }
                            } else {
                                parser._tag |= BKV::BKV_I32;
                                try { _appendValue(parser, static_cast<int32_t>(val)); } catch (std::runtime_error &e) {
                                    reset();
                                    throw;
                                }
                            }
                        }
                    }
                    // Start again now that we have a number
                    try { _endNumber(parser, c); } catch (std::runtime_error &e) {
                        reset();
                        throw;
                    }
                } break;
                case 'B':
                case 'b': {
                    parser._tag |= BKV::BKV_I8;
                    try {
                        if (parser._tag & BKV::BKV_UNSIGNED) {
                            uint8_t val = FormatString::strToUInt<uint8_t>(_numBuffer.get(), 10, _numBuffer.len());
                            _appendValue(parser, val);
                        } else {
                            int8_t val = FormatString::strToInt<int8_t>(_numBuffer.get(), 10, _numBuffer.len());
                            _appendValue(parser, val);
                        }
                    } catch (std::runtime_error &e) {
                        reset();
                        throw;
                    }
                } break;
                case 'D':
                case 'd': {
                    parser._tag |= BKV::BKV_DOUBLE;
                    try {
                        float128_t val = Endianness::htonf(FormatString::strToFloat<float128_t>(_numBuffer.get(), 10, _numBuffer.len()));
                        std::memcpy(parser._buffer._bkv + parser._buffer._head, &val, sizeof(float128_t));
                        parser._buffer._head += sizeof(float128_t);
                    } catch (std::runtime_error &e) {
                        reset();
                        throw;
                    }
                } break;
                case 'F':
                case 'f': {
                    parser._tag |= BKV::BKV_FLOAT;
                    try {
                        float32_t val = Endianness::htonf(FormatString::strToFloat<float32_t>(_numBuffer.get(), 10, _numBuffer.len()));
                        std::memcpy(parser._buffer._bkv + parser._buffer._head, &val, sizeof(float32_t));
                        parser._buffer._head += sizeof(float32_t);
                    } catch (std::runtime_error &e) {
                        reset();
                        throw;
                    }
                } break;
                case 'I':
                case 'i': {
                    parser._tag |= BKV::BKV_I32;
                    try {
                        if (parser._tag & BKV::BKV_UNSIGNED) {
                            uint32_t val = FormatString::strToUInt<uint32_t>(_numBuffer.get(), 10, _numBuffer.len());
                            _appendValue(parser, val);
                        } else {
                            int32_t val = FormatString::strToInt<int32_t>(_numBuffer.get(), 10, _numBuffer.len());
                            _appendValue(parser, val);
                        }
                    } catch (std::runtime_error &e) {
                        reset();
                        throw;
                    }
                } break;
                case 'L':
                case 'l': {
                    parser._tag |= BKV::BKV_I64;
                    try {
                        if (parser._tag & BKV::BKV_UNSIGNED) {
                            uint64_t val = FormatString::strToUInt<uint64_t>(_numBuffer.get(), 10, _numBuffer.len());
                            _appendValue(parser, val);
                        } else {
                            int64_t val = FormatString::strToInt<int64_t>(_numBuffer.get(), 10, _numBuffer.len());
                            _appendValue(parser, val);
                        }
                    } catch (std::runtime_error &e) {
                        reset();
                        throw;
                    }
                } break;
                case 'S':
                case 's': {
                    parser._tag |= BKV::BKV_I16;
                    try {
                        if (parser._tag & BKV::BKV_UNSIGNED) {
                            uint16_t val = FormatString::strToUInt<uint16_t>(_numBuffer.get(), 10, _numBuffer.len());
                            _appendValue(parser, val);
                        } else {
                            int16_t val = FormatString::strToInt<int16_t>(_numBuffer.get(), 10, _numBuffer.len());
                            _appendValue(parser, val);
                        }
                    } catch (std::runtime_error &e) {
                        reset();
                        throw;
                    }
                } break;
                case 'U':
                case 'u': {
                    if (_hasNegative) {
                        UTF8Str msg = FormatString::formatString("Negative BKV number is supposed to be unsigned at index %ld.",
                            parser._charactersRead
                        );
                        reset();
                        throw std::runtime_error(msg.get());
                    }
                    parser._tag |= BKV::BKV_UNSIGNED;
                } break;
                default: {
                    UTF8Str msg = FormatString::formatString("Invalid character in SBKV number at index %ld: %02x.",
                        parser._charactersRead, c
                    );
                    reset();
                    throw std::runtime_error(msg.get());
                }
            }
        }
    }
}
