#include <queue>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_robot_interfaces/action/count_until.hpp"

using namespace std::placeholders;

using CountUntil = my_robot_interfaces::action::CountUntil;
using CountUntilGoalHandle = rclcpp_action::ServerGoalHandle<CountUntil>;

class CountUntilServerNode : public rclcpp::Node
{
public:
    CountUntilServerNode() : rclcpp::Node("count_until_server")
    {   
        goal_queue_thread_ = std::thread(
            std::bind(&CountUntilServerNode::run_goal_queue, this)
        );
        cb_group_ = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);
        count_until_server_ = rclcpp_action::create_server<CountUntil>(
            this,
            "count_until",
            std::bind(&CountUntilServerNode::goal_callback, this, _1, _2),
            std::bind(&CountUntilServerNode::cancel_callback, this, _1),
            std::bind(&CountUntilServerNode::handle_accepted_callback, this, _1),
            rcl_action_server_get_default_options(),
            cb_group_
        );
        RCLCPP_INFO(this->get_logger(), "CountUntil action server started.");
    }
    ~CountUntilServerNode()
    {
        goal_queue_thread_.join();
    }

private:

    rclcpp_action::GoalResponse goal_callback(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const CountUntil::Goal> goal)
    {
        (void)uuid;
        RCLCPP_INFO(this->get_logger(), "Received goal");

        if (goal->target_number <= 0)
        {
            RCLCPP_INFO(this->get_logger(), "Rejected goal because target_number is not positive.");
            return rclcpp_action::GoalResponse::REJECT;
        }

        RCLCPP_INFO(this->get_logger(), "Accepted goal.");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse cancel_callback(
        const std::shared_ptr<CountUntilGoalHandle> goal_handle)
    {   
        RCLCPP_INFO(this->get_logger(), "Received request to cancel goal");
        (void)goal_handle;
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handle_accepted_callback(
        const std::shared_ptr<CountUntilGoalHandle> goal_handle)
    {
        {
        std::lock_guard<std::mutex> lock(goal_handle_mutex_);
        goal_queue_.push(goal_handle);
        RCLCPP_INFO(this->get_logger(), "Goal added to queue");
        RCLCPP_INFO(this->get_logger(), "Queue size: %d", (int)goal_queue_.size());
        }
    }

    void run_goal_queue()
    {
        rclcpp::Rate loop_rate(1000);
        while (rclcpp::ok())
        {
            std::shared_ptr<CountUntilGoalHandle> next_goal;
            {
                std::lock_guard<std::mutex> lock(goal_handle_mutex_);
                if(goal_queue_.size())
                {
                    next_goal = goal_queue_.front();
                    goal_queue_.pop();
                }
            }
            if (next_goal){
                RCLCPP_INFO(this->get_logger(), "Starting execution of next goal in queue.");
                execute_goal(next_goal);
            }
            loop_rate.sleep();
        }
    }
        

    void execute_goal(
        const std::shared_ptr<CountUntilGoalHandle> goal_handle)
    {
    
        // Get request from goal
        int target_number = goal_handle->get_goal()->target_number;
        double period = goal_handle->get_goal()->period;

        //Execute the action
        int counter = 0;
        auto result = std::make_shared<CountUntil::Result>();
        auto feedback = std::make_shared<CountUntil::Feedback>();
        rclcpp::Rate loop_rate(1.0 / period);
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Give time to set up feedback subscription
        for(int i = 0; (i < target_number);i++)
        {
            if (goal_handle->is_canceling())
            {
                result->reached_number = counter;
                goal_handle->canceled(result);
                return;
            }
            
            counter++;
            RCLCPP_INFO(this->get_logger(), "%d", counter);
            feedback->current_number = counter;
            goal_handle->publish_feedback(feedback);
            loop_rate.sleep();
        }
        // Set the final state and return result
        
        result->reached_number = counter;
        goal_handle->succeed(result);
    }

    rclcpp_action::Server<CountUntil>::SharedPtr count_until_server_;
    rclcpp::CallbackGroup::SharedPtr cb_group_;
    std::mutex goal_handle_mutex_;
    std::queue<std::shared_ptr<CountUntilGoalHandle>> goal_queue_;
    std::thread goal_queue_thread_;

};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CountUntilServerNode>(); 
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
    //    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}