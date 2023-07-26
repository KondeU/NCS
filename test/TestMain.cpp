#include "TestCommon.hpp"
#include "NCS.hpp"

struct Component1 {
    DEFINE_COMPONENT(Component1);
    double vd = 1.0;
    float vf = 2.0f;
    int vi = 3;
};

struct Component2 {
    DEFINE_COMPONENT(Component2);
    std::vector<float> vec{ 1.0f, 1.0f, 1.0f, 0.0f };
};

struct Component3 {
    DEFINE_COMPONENT(Component3);
    int arr[4] = { 255, 255, 255, 0 };
};

class System1 : public au::ncs::UnorderedTreeSystem {
public:
    // The following functions can be considered as mook and should
    // only be used for testing purposes, not used in business logic.
    au::ncs::Viewer<Component1, Component2>& GetViewer()
    {
        return *viewer;
    }

    ~System1() override
    {
    }

protected:
    void OnPrepare(au::ncs::UnorderedTreeRegistry& registry) override
    {
        viewer = std::make_unique<au::ncs::Viewer<Component1, Component2>>(registry);
        TEST(viewer->HasComponentType<Component1>());
        TEST(viewer->HasComponentType<Component2>());
        TEST(viewer->HasComponentType<Component3>() == false);
        TEST(viewer->HasType(Component1::ComponentUuid));
        TEST(viewer->HasType(Component2::ComponentUuid));
        TEST(viewer->HasType(Component3::ComponentUuid) == false);
    }

    void OnShutdown(au::ncs::UnorderedTreeRegistry& registry) override
    {
    }

    void OnPrepTick(au::ncs::UnorderedTreeRegistry& registry, float delta) override
    {
    }

    void OnProcTick(au::ncs::UnorderedTreeRegistry& registry, float delta) override
    {
    }

    void OnPostTick(au::ncs::UnorderedTreeRegistry& registry, float delta) override
    {
    }

private:
    std::unique_ptr<au::ncs::Viewer<Component1, Component2>> viewer;
};

void TEST_GROUP_1()
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
    au::ncs::ComponentBuffer<Component2> comp2buffers;
    au::ncs::ComponentBuffer<Component3> comp3buffers;
    TEST(registry.RegisterComponentStorage<Component2>(&comp2buffers));
    TEST(registry.RegisterComponentStorage<Component3>(&comp3buffers));

    node1comp1 = registry.AddComponent<Component1>(node1);
    TEST(node1comp1 != nullptr);
    TEST(registry.GetComponent<Component1>(node1) == node1comp1);
    TEST(registry.AddComponent<Component1>(node1) == node1comp1);

    System1 system1;
    TEST(scene.RegisterSystem(system1));
    TEST(scene.RegisterSystem(system1) == false);
    auto& system1viewer = system1.GetViewer();

    Component2* node1comp2 = registry.AddComponent<Component2>(node1);
    Component1* node2comp1 = registry.AddComponent<Component1>(node2);
    Component2* node2comp2 = registry.AddComponent<Component2>(node2);
    TEST((node1comp2 != nullptr) && (node2comp1 != nullptr) && (node2comp2 != nullptr));
    TEST(registry.RemoveComponent<Component2>(node2));

    system1viewer.ForEach([&](au::ncs::Node node) {
        TEST(node == node1);
    });

    node2comp2 = registry.AddComponent<Component2>(node2);
    TEST(node1comp2 != nullptr);
    {
        std::unordered_set<au::ncs::Node> check, benchmark = { node1, node2 };
        system1viewer.ForEach([&](au::ncs::Node node) {
            check.insert(node);
        });
        TEST(check == benchmark);
    }

    Component3* node1comp3 = registry.AddComponent<Component3>(node1);
    TEST(node1comp3 != nullptr);
    registry.ForEach<Component3>([&](au::ncs::Node node, Component3& component) {
        TEST((node == node1) && (&component == node1comp3));
    });

    static constexpr int TestLoopCount = 2;
    for (int i = 0; i < TestLoopCount; i++) {
        scene.Tick();
    }

    TEST(scene.UnregisterSystem(system1));
    TEST(registry.UnregisterComponentStorage<Component3>() == &comp3buffers);
    TEST(registry.UnregisterComponentStorage<Component2>() == &comp2buffers);
    TEST(registry.UnregisterComponentStorage<Component1>() == &comp1buffers);
    TEST(registry.UnregisterComponentStorage<Component1>() == nullptr);
}

int main(int argc, char* argv[]) // Args not used.
{
    TEST_GROUP_1();
    return 0;
}
