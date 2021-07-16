#pragma once

/**
 * EnumClass provides a class wrapper around an enum so you can define member functions for the enum.
 * Example:
 *      EnumClass MyAwesomeEnum {
 *           enum Value : int {
 *               enum_val1,
 *               enum_val2,
 *               ...
 *               enum_val_N
 *           };
 *           ENUM_CLASS(MyAwesomeEnum)
 *
 *           void myAwesomeEnumFunction_1();
 *           void myAwesomeEnumFunction_2();
 *           void myAwesomeEnumFunction_3();
 *       };
 */

#define EnumClass   struct

#define ENUM_CLASS(ClassName) \
    public: \
        ClassName() = default; \
        inline ClassName(Value v) : value(v) {} \
        inline operator Value &() { return value;} \
        inline operator Value() const { return value;} \
        inline Value get() const {return value;} \
        inline Value& get() {return value;} \
        inline bool operator == (const ClassName& other) const {return other.value == value;} \
        inline bool operator != (const ClassName& other) const {return other.value != value;} \
        inline bool operator == (const Value& other) const {return other == value;} \
        inline bool operator != (const Value& other) const {return other != value;} \
    private: \
        Value value = (Value) 0; \
    public:
