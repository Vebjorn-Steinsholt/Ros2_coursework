#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/int64.hpp"
#include "example_interfaces/srv/set_bool.hpp"

using namespace std::placeholders;

class NumberCounterNode : public rclcpp::Node
{
public:

    NumberCounterNode() : rclcpp::Node("number_counter")
    {
        subscriber_ = this-> create_subscription<example_interfaces::msg::Int64>(
            "number",10,
            std::bind(&NumberCounterNode::callbackNumberCounter, this, _1));
        publisher_ = this->create_publisher<example_interfaces::msg::Int64>("number_count", 10);
        service_ = this->create_service<example_interfaces::srv::SetBool>(
            "reset_counter",
            std::bind(&NumberCounterNode::callbackResetCounter, this, _1, _2));
    }

private:
    void callbackNumberCounter(const example_interfaces::msg::Int64::SharedPtr msg)
    {
        number_counter_ += msg->data;
        auto count_msg = example_interfaces::msg::Int64();
        count_msg.data = number_counter_;
        publisher_->publish(count_msg);
    }
    void callbackResetCounter(
        const example_interfaces::srv::SetBool::Request::SharedPtr request,
        example_interfaces::srv::SetBool::Response::SharedPtr response)
    {
        if (request->data) {
            number_counter_ = 0;
            response->success = true;
            response->message = "Counter reset to zero.";
        } else {
            response->success = false;
            response->message = "Counter not reset.";
        }
    }

    rclcpp::Subscription<example_interfaces::msg::Int64>::SharedPtr subscriber_;
    rclcpp::Publisher<example_interfaces::msg::Int64>::SharedPtr publisher_;
    rclcpp::Service<example_interfaces::srv::SetBool>::SharedPtr service_;
    int number_counter_ = 0;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberCounterNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}