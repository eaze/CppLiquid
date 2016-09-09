#ifndef LIQUID_NODE_HPP
#define LIQUID_NODE_HPP

#include "variable.hpp"
#include <QStringRef>
#include <memory>

namespace Liquid {
    
    class Context;
    class Tokenizer;
    
    class Node {
    public:
        virtual QString render(Context& context) = 0;
    };

    class TextNode : public Node {
    public:
        TextNode(const QStringRef& text);
        
        virtual QString render(Context&) override;
        
    private:
        const QStringRef text_;
    };
    
    class ObjectNode : public Node {
    public:
        ObjectNode(const Variable& var);

        virtual QString render(Context& context) override;

    private:
        const Variable var_;
    };
    
    class TagNode : public Node {
    public:
        TagNode(const QStringRef&, const QStringRef&) {}
        virtual QString render(Context& context) override;
    };
    
    using NodePtr = std::shared_ptr<Node>;
}

#endif
