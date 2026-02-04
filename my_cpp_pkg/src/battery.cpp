#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/srv/set_led.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders;

class BatteryNode : public rclcpp::Node
{
public:
    BatteryNode() : rclcpp::Node("battery_node"),battery_state_(true)
    {
        last_state_change_ = this->get_clock()->now().seconds();
        battery_timer_ = this->create_wall_timer(
            0.1s,
            std::bind(&BatteryNode::checkBatteryState, this)
        );
        set_led_client_ = this->create_client<my_robot_interfaces::srv::SetLed>("set_led");
        while (!set_led_client_->wait_for_service(1s))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for the set_led service to be available...");
        }
        
    }

private:
    bool battery_state_; // true = full, false = empty
    rclcpp::Client<my_robot_interfaces::srv::SetLed>::SharedPtr set_led_client_;
    rclcpp::TimerBase::SharedPtr battery_timer_;
    double last_state_change_;
    
    void checkBatteryState(){
        double current_time = this->get_clock()->now().seconds();
        if(battery_state_ )
        {
            if(current_time - last_state_change_ >= 4.0)
            {
                RCLCPP_INFO(this->get_logger(), "Battery is empty. Charging...");
                battery_state_ = false;
                last_state_change_ = current_time;
                callSetLed(2,1);
            }
        }
        else
        {
            if(current_time - last_state_change_ >= 6.0){
                RCLCPP_INFO(this->get_logger(), "Battery is full. Discharging...");
                battery_state_ = true;
                last_state_change_ = current_time;
                callSetLed(2,0);
            }
        }
    }
    void callSetLed(int led_number, int state)
    {
        auto request = std::make_shared<my_robot_interfaces::srv::SetLed::Request>();
        request->led_number = led_number;
        request->state = state;
        auto result_future = set_led_client_->async_send_request(request,std::bind(&BatteryNode::callbackCallSetLed,this,_1)); 
    }
    void callbackCallSetLed(rclcpp::Client<my_robot_interfaces::srv::SetLed>::SharedFuture future)
    {
        auto response = future.get();
        if (response->success)
        {
            RCLCPP_INFO(this->get_logger(), "Led state set successfully.");
        }
        else {
            RCLCPP_ERROR(this->get_logger(), "Led state set failed.");
        }
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<BatteryNode>(); 
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}