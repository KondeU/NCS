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
        nodes.push_back({ Node(0), {} });
    }

    bool ReparentNode(Node node, Node parent)
    {
        if (node.IsInvalid() || (node == parent)) {
            return false;
        }

        NodeRelation* nodeRelation = nullptr;
        NodeRelation* parentRelation = nullptr;
        for (auto& [current, relation] : nodes) {
            if (current == node) {
                nodeRelation = &relation;
            }
            if (current == parent) {
                parentRelation = &relation;
            }

            if (nodeRelation && parentRelation) {
                // Erase the node in the old parent's childs list.
                auto oldParent = std::find(nodes.begin(), nodes.end(), nodeRelation->parent);
                oldParent->second.childs.erase(std::find(
                    oldParent->second.childs.begin(), oldParent->second.childs.end(), node));
                // Set the node's new parent.
                nodeRelation->parent = parent;
                // Add the node to the new parent's childs list.
                parentRelation->childs.emplace_back(node);
                // Reparent finished.
                return true;
            }
        }
        return false; // Failed, node or parent not found in the nodes list.
    }

protected:
    bool OnNodeCreate(Node node, Registry<TreeRelation>& registry)
    {
        nodes.push_back({ node, {} });
        std::find(nodes.begin(), nodes.end(), Node(0))->second.childs.emplace_back(node);
        return true;
    }

    bool OnNodeDestroy(Node node, Registry<TreeRelation>& registry)
    {
        auto& nodeRelation = std::find(nodes.begin(), nodes.end(), node)->second;
        auto& parentRelation = std::find(nodes.begin(), nodes.end(), nodeRelation.parent)->second;
        for (auto it = parentRelation.childs.begin(); it != parentRelation.childs.end(); it++) {
            if (*it == node) {
                parentRelation.childs.erase(it);
                break;
            }
        }
        // Destroy...
        return true;
    }

private:
    std::vector<std::pair<Node, NodeRelation>> nodes;
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
