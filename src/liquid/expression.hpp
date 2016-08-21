#ifndef LIQUID_EXPRESSION_HPP
#define LIQUID_EXPRESSION_HPP

#include <QVariant>
#include <vector>
#include "parser.hpp"

namespace Liquid {
    
    class Expression {
    public:
        enum class Type {
            Nil,
            String,
            NumberInt,
            NumberFloat,
            BooleanTrue,
            BooleanFalse,
            Lookup,
            LookupKey,
            LookupBracketKey,
        };
        
        Expression(Type type)
            : type_(type)
        {
        }
        
        Expression(const Expression& other)
            : type_(other.type_)
            , var_(other.var_)
            , lookups_(other.lookups_)
        {
        }
        
        Expression& operator=(const Expression& other) {
            if (this != &other) {
                type_ = other.type_;
                var_ = other.var_;
                lookups_ = other.lookups_;
            }
            return *this;
        }
        
        bool operator==(const Expression& other) const {
            return type_ == other.type_ && var_ == other.var_ && lookups_ == other.lookups_;
        }
        
        Expression()
            : type_(Type::Nil)
        {
        }
        
        Expression(const QString& string)
            : type_(Type::String)
            , var_(string)
        {
        }
        
        Expression(const char *string) : Expression(QString(string)) {}

        Expression(int value)
            : type_(Type::NumberInt)
            , var_(value)
        {
        }

        Expression(double value)
            : type_(Type::NumberFloat)
            , var_(value)
        {
        }

        Expression(bool value)
            : type_(value ? Type::BooleanTrue : Type::BooleanFalse)
        {
        }

        Type type() const {
            return type_;
        }
        
        QString typeString() const {
            switch (type_) {
                case Type::Nil: return "Nil";
                case Type::String: return "String";
                case Type::NumberInt: return "NumberInt";
                case Type::NumberFloat: return "NumberFloat";
                case Type::BooleanTrue: return "BooleanTrue";
                case Type::BooleanFalse: return "BooleanFalse";
                case Type::Lookup: return "Lookup";
                case Type::LookupKey: return "LookupKey";
                case Type::LookupBracketKey: return "LookupBracketKey";
            }
        }
        
        bool isString() const {
            return type_ == Type::String;
        }

        bool isNumber() const {
            return type_ == Type::NumberInt || type_ == Type::NumberFloat;
        }
        
        bool isInt() const {
            return type_ == Type::NumberInt;
        }
        
        bool isFloat() const {
            return type_ == Type::NumberFloat;
        }

        bool isBoolean() const {
            return type_ == Type::BooleanTrue || type_ == Type::BooleanFalse;
        }
        
        bool isNil() const {
            return type_ == Type::Nil;
        }
        
        bool isLookup() const {
            return type_ == Type::Lookup;
        }
        
        bool isLookupKey() const {
            return type_ == Type::LookupKey;
        }
        
        bool isLookupBracketKey() const {
            return type_ == Type::LookupBracketKey;
        }
        
        QString toString() const {
            return var_.toString();
        }
        
        bool toBool() const {
            return type_ == Type::BooleanTrue;
        }
        
        int toInt() const {
            return var_.toInt();
        }
        
        double toFloat() const {
            return var_.toDouble();
        }
        
        void setKey(const QString& key) {
            var_ = key;
        }
        
        QString key() const {
            return var_.toString();
        }
        
        const std::vector<Expression>& lookups() const {
            return lookups_;
        }
        
        void addLookup(const Expression& exp) {
            lookups_.push_back(exp);
        }
        
        static Expression parse(const QStringRef& input) {
            Parser parser(input);
            return parse(parser);
        }
        
        static Expression parse(Parser& parser);
        
    private:
        Type type_;
        QVariant var_;
        std::vector<Expression> lookups_;
    };

}

#endif
