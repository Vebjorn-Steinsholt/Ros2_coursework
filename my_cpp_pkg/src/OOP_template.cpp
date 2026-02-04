#include "rclcpp/rclcpp.hpp"

// TEMPLATE: class and node name placeholders below should be replaced
class MyCustomNode : public rclcpp::Node
{
public:
    // Replace "my_custom_node" with your node's name
    MyCustomNode() : rclcpp::Node("my_custom_node")
    {
        // add initialization or callbacks here
    }

private:
    // add private members here
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MyCustomNode>(); // replace class name as needed
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}