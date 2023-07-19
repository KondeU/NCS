#pragma once

#include <unordered_set>
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

    TreeRelation()
    {
        nodes[Node(0)] = {};
    }

    bool ReparentNode(Node node, Node parent)
    {
        // Root node can not be reparented, and node can not be reparented to itself.
        if (node.IsInvalid() || (node == parent)) {
            return false;
        }

        auto nodeIter = nodes.find(node);
        if (nodeIter == nodes.end()) {
            return false;
        }
        NodeRelation& nodeRelation = nodeIter->second;

        // Parent no change.
        if (nodeRelation.parent == parent) {
            return true;
        }

        auto oldParentIter = nodes.find(nodeRelation.parent);
        if (oldParentIter == nodes.end()) {
            return false; // Internal logic error maybe occur if run here.
        }
        NodeRelation& oldParentRelation = oldParentIter->second;

        auto newParentIter = nodes.find(parent);
        if (newParentIter == nodes.end()) {
            return false;
        }
        NodeRelation& newParentRelation = newParentIter->second;

        // Erase the node in the old parent relation's childs list.
        oldParentRelation.childs.erase(std::find(
            oldParentRelation.childs.begin(), oldParentRelation.childs.end(), node));
        // Set the node's new parent.
        nodeRelation.parent = parent;
        // Add the node to the new parent relation's childs list.
        newParentRelation.childs.emplace_back(node);

        return true;
    }

protected:
    bool OnNodeCreate(Node node, Registry<TreeRelation>& registry)
    {
        nodes[node] = {};
        nodes[Node(0)].childs.emplace_back(node);
        return true;
    }

    bool OnNodeDestroy(Node node, Registry<TreeRelation>& registry)
    {
        auto nodeIter = nodes.find(node);
        if (nodeIter == nodes.end()) {
            return false;
        }
        NodeRelation& nodeRelation = nodeIter->second;

        auto parentIter = nodes.find(nodeRelation.parent);
        if (parentIter == nodes.end()) {
            return false; // Internal logic error maybe occur if run here.
        }
        NodeRelation& parentRelation = parentIter->second;

        // Erase the node in the parent relation's childs list.
        parentRelation.childs.erase(std::find(
            parentRelation.childs.begin(), parentRelation.childs.end(), node));

        // Destroy childs.
        bool success = true;
        for (const auto& child : nodeRelation.childs) {
            success &= registry.DestroyNode(child);
        }

        nodes.erase(nodeIter);
        return success;
    }

private:
    std::unordered_map<Node, NodeRelation> nodes;
};

class UnorderedTreeRelation {
public:
    struct NodeRelation {
        Node parent;
        std::unordered_set<Node> childs;
    };

    UnorderedTreeRelation()
    {
        nodes[Node(0)] = {};
    }

    bool ReparentNode(Node node, Node parent)
    {
        return true;
    }

protected:
    bool OnNodeCreate(Node node, Registry<UnorderedTreeRelation>& registry)
    {
        return true;
    }

    bool OnNodeDestroy(Node node, Registry<UnorderedTreeRelation>& registry)
    {
        return true;
    }

private:
    std::unordered_map<Node, NodeRelation> nodes;
};

}
