/**
 * @file ValueWrap.h
 * @author  (linuxzq93@163.com)
 * @brief 一些常见数据类型的封装类，便于特例化处理
 * @version 1.0
 * @date 2025-04-21
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __FRAMEWORK_INCLUDE_DATA_VALUEWRAP_H__
#define __FRAMEWORK_INCLUDE_DATA_VALUEWRAP_H__

#include "Any.h"

namespace base {

class ValueWrap {
public:
    enum ValueType {
        INT8,
        INT32,
        INT64,
        UINT8,
        UINT32,
        UINT64,
        FLOAT,
        DOUBLE,
        STRING,
        BOOL,
        UNKNOWN,
    };
public:
    ValueWrap();
    ValueWrap(const base::any &value);
    ValueWrap(int8_t value);
    ValueWrap(int value);
    ValueWrap(int64_t value);
    ValueWrap(uint8_t value);
    ValueWrap(uint32_t value);
    ValueWrap(uint64_t value);
    ValueWrap(float value);
    ValueWrap(double value);
    ValueWrap(const char *value);
    ValueWrap(const std::string &value);
    ValueWrap(bool value);
    ~ValueWrap();
    ValueWrap(const ValueWrap &other);
    ValueWrap(ValueWrap &&other);
    ValueWrap &operator=(const ValueWrap &other);
    ValueWrap &operator=(ValueWrap &&other);
    ValueType getType() const;
    bool isInt8() const;
    bool isInt() const;
    bool isInt64() const;
    bool isUInt8() const;
    bool isUInt32() const;
    bool isUInt64() const;
    bool isFloat() const;
    bool isDouble() const;
    bool isString() const;
    bool isBool() const;
    int8_t asInt8() const;
    int asInt() const;
    int64_t asInt64() const;
    uint8_t asUInt8() const;
    uint32_t asUInt32() const;
    uint64_t asUInt64() const;
    float asFloat() const;
    double asDouble() const;
    std::string asString() const;
    bool asBool() const;
    bool empty() const;
private:
    base::any m_value;
    ValueType m_type;
};
}

#endif /* __FRAMEWORK_INCLUDE_DATA_VALUEWARP_H__ */
