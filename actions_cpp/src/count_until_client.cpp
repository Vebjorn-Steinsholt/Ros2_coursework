#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_robot_interfaces/action/count_until.hpp"

using namespace std::placeholders;

using CountUntil = my_robot_interfaces::action::CountUntil;
using CountUntilGoalHandle = rclcpp_action::ClientGoalHandle<CountUntil>;

class CountUntilClientNode : public rclcpp::Node
{
public:
    CountUntilClientNode() : rclcpp::Node("count_until_client")
    {
        count_until_client_ = 
            rclcpp_action::create_client<CountUntil>(this, "count_until");
    }

    void send_goal(int target_number, double period)
    {
        // Wait for the action server to be available
        count_until_client_->wait_for_action_server();
        // Create a goal message
        auto goal = CountUntil::Goal();
        goal.target_number = target_number;
        goal.period = period;

        //add callbacks here if needed
        auto options = rclcpp_action::Client<CountUntil>::SendGoalOptions();
        options.result_callback = 
            std::bind(&CountUntilClientNode::goal_result_callback, this, _1);
        options.goal_response_callback = 
            std::bind(&CountUntilClientNode::goal_response_callback, this, _1);
        options.feedback_callback = 
            std::bind(&CountUntilClientNode::goal_feedback_callback, this, _1, _2);

        // Send the goal asynchronously
        RCLCPP_INFO(this->get_logger(), "Sending goal");
        count_until_client_->async_send_goal(
            goal, options);
        //create a timer to cancel the goal after some time
    //     timer_ = this->create_wall_timer( 
    //         std::chrono::seconds(2), 
    //         std::bind(&CountUntilClientNode::timer_callback, this));
    }

private:

    void timer_callback()
    {
        RCLCPP_INFO(this->get_logger(), "Cancelling the goal");
        count_until_client_->async_cancel_goal(goal_handle_);
        timer_ ->cancel();
                
    }
    //callback to know whether the goal was accepted or rejected
    void goal_response_callback(const CountUntilGoalHandle::SharedPtr &goal_handle)
    {
     if (!goal_handle)
     {
        RCLCPP_INFO(this->get_logger(), "Goal was rejected by server");
     }
     else
     {
        RCLCPP_INFO(this->get_logger(), "Goal accepted");
        this->goal_handle_ = goal_handle;
     }
     
    }
    //callback to receive the result once the goal is complete   
    void goal_result_callback(
        const CountUntilGoalHandle::WrappedResult & result)
        {
            auto status = result.code;
            if (status == rclcpp_action::ResultCode::SUCCEEDED)
            {
                RCLCPP_INFO(this->get_logger(), "Goal succeeded");
            }
            else if (status == rclcpp_action::ResultCode::ABORTED)
            {
                RCLCPP_ERROR(this->get_logger(), "Goal was aborted");
            }
            else if (status == rclcpp_action::ResultCode::CANCELED)
            {
                RCLCPP_WARN(this->get_logger(), "Goal was canceled");
            }
            else
            {
                RCLCPP_INFO(this->get_logger(), "Unknown result code");
            }
            int reached_number = result.result->reached_number;
            RCLCPP_INFO(this->get_logger(), "Goal completed with reached number: %d", reached_number);
        }

    //callback to receive feedback during execution
    void goal_feedback_callback(
        const CountUntilGoalHandle::SharedPtr &goal_handle,
        const std::shared_ptr<const CountUntil::Feedback> feedback)
        {
            (void)goal_handle;
            int number = feedback->current_number;
            RCLCPP_INFO(this->get_logger(), "Current number: %d", number);
        }
    rclcpp_action::Client<CountUntil>::SharedPtr count_until_client_;
    rclcpp::TimerBase::SharedPtr timer_;
    CountUntilGoalHandle::SharedPtr goal_handle_;

};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CountUntilClientNode>();
    node ->send_goal(6, 0.8);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}