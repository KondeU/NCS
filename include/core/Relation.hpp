#pragma once

#include "Registry.hpp"

namespace au::ncs {

class EmptyRelation {
protected:
    bool OnNodeCreate(Node node, Registry<EmptyRelation>& registry)
    {
        return true;
    }

    bool OnNodeDestroy(Node node, Registry<EmptyRelation>& registry)
    {
        return true;
    }
};

class TreeRelation {
public:
    struct NodeRelation {
        Node parent;
        std::vector<Node> childs;
    };

    bool ReparentNode(Node node, Node parent)
    {
        return true;
    }

protected:
    bool OnNodeCreate(Node node, Registry<TreeRelation>& registry)
    {
        return true;
    }

    bool OnNodeDestroy(Node node, Registry<TreeRelation>& registry)
    {
        return true;
    }

private:
    std::unordered_map<Node, NodeRelation> nodes;
};

}
