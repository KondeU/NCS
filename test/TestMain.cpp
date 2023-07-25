#include "TestCommon.hpp"
#include "TestSystem.hpp"
#include "TestComponent.hpp"
#include "TestRelation.hpp"

void TEST_GROUP_1()
{
}

void TEST_GROUP_2()
{
}

void TEST_GROUP_3()
{
    au::ncs::UnorderedTreeScene scene;
    au::ncs::UnorderedTreeRegistry& registry = scene.GetRegistry();
    TEST(scene.GetDefaultRootNode() == au::ncs::DEFAULT_ROOT_NODE);

    au::ncs::Node node1 = registry.CreateNode();
    TEST(registry.GetParentNode(node1) == au::ncs::DEFAULT_ROOT_NODE);

    au::ncs::Node node2 = registry.CreateNode();
    TEST(registry.GetParentNode(node2) == au::ncs::DEFAULT_ROOT_NODE);

    au::ncs::Node node3 = registry.CreateNode();
    TEST(registry.GetParentNode(node3) == au::ncs::DEFAULT_ROOT_NODE);

    registry.ReparentNode(node3, node1);
    TEST(registry.GetParentNode(node3) == node1);
    TEST(registry.GetChildrenNodes(node1).size() == 1);
    TEST(*registry.GetChildrenNodes(node1).begin() == node3);

    Component1* node1comp1 = registry.AddComponent<Component1>(node1);
    TEST(node1comp1 == nullptr);

    au::ncs::ComponentBuffer<Component1> comp1buffers, comp1buffers_exist_failed_test;
    TEST(registry.RegisterComponentStorage<Component1>(&comp1buffers));
    TEST(registry.RegisterComponentStorage<Component1>(&comp1buffers_exist_failed_test) == false);

    node1comp1 = registry.AddComponent<Component1>(node1);
    TEST(node1comp1 != nullptr);

    // TODO: more test cases...

    static constexpr int TestLoopCount = 2;
    for (int i = 0; i < TestLoopCount; i++) {
        scene.Tick();
    }

    TEST(registry.UnregisterComponentStorage<Component1>() == &comp1buffers);
}

int main(int argc, char* argv[]) // Args not used.
{
    TEST_GROUP_1();
    TEST_GROUP_2();
    TEST_GROUP_3();
    return 0;
}
