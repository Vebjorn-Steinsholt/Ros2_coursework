#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/msg/led_state_array.hpp"
#include "my_robot_interfaces/srv/set_led.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders;


class LedPanelNode : public rclcpp::Node
{
public:
    LedPanelNode() : rclcpp::Node("led_panel_state")
    {
        this->declare_parameter("led_state", std::vector<int64_t>{0,0,0});
        led_state_ = this->get_parameter("led_state").as_integer_array();
        publisher_ = this->create_publisher<my_robot_interfaces::msg::LedStateArray>("led_panel_status", 10);
        timer_ = this->create_wall_timer(
            1s,
            std::bind(&LedPanelNode::publishLedPanelStatus, this)
        );
        service_ = this->create_service<my_robot_interfaces::srv::SetLed>(
            "set_led",
            std::bind(&LedPanelNode::callbackSetLed, this, _1, _2));
        RCLCPP_INFO(this->get_logger(), "Led Pane Node has been started.");

    }

private:
    rclcpp::Publisher<my_robot_interfaces::msg::LedStateArray>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::vector<int64_t> led_state_;
    rclcpp::Service<my_robot_interfaces::srv::SetLed>::SharedPtr service_;

    void publishLedPanelStatus()
    {
        auto msg = my_robot_interfaces::msg::LedStateArray();
        msg.led_states = led_state_;
        publisher_->publish(msg);
    }
    void callbackSetLed(
        const std::shared_ptr<my_robot_interfaces::srv::SetLed::Request> request,
        std::shared_ptr<my_robot_interfaces::srv::SetLed::Response> response)
    {
      int64_t led_number = request->led_number;
      int64_t state = request->state;
      if(led_number >= (int64_t)led_state_.size() || led_number < 0)
      {
        response->success = false;
        return;
      }
      if (state!=0 && state!=1)
      {
        response->success = false;
        return;
      }
      led_state_.at(led_number) = state;
      response->success = true;
      publishLedPanelStatus();
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LedPanelNode>(); 
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}