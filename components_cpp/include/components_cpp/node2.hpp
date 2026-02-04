#include "rclcpp/rclcpp.hpp"

class Node2 : public rclcpp::Node
{
public:
    Node2();

private:

    void callbackTimer4();

    void callbackTimer5();

    rclcpp::TimerBase::SharedPtr timer4_;
    rclcpp::TimerBase::SharedPtr timer5_;
};