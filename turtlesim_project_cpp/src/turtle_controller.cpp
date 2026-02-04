#include "rclcpp/rclcpp.hpp"
#include <cmath>
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "my_robot_interfaces/msg/turtle.hpp"
#include "my_robot_interfaces/msg/turtle_array.hpp"
#include "my_robot_interfaces/srv/catch_turtle.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders;

class TurtleControllerNode : public rclcpp::Node
{
public:
    TurtleControllerNode() : Node("turtle_controller"), name_("turtle1"), turtlesim_up_(false)
    {
        this->declare_parameter<bool>("catch_closest_turtle", true);
        this->get_parameter("catch_closest_turtle", catch_closest_turtle_);
        cmd_vel_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
            name_ + "/cmd_vel", 10);
        pose_subscriber_ = this->create_subscription<turtlesim::msg::Pose>(
            name_ + "/pose", 10, std::bind(&TurtleControllerNode::callbackPose, this, _1));
        alive_turtles_subscriber_ = this->create_subscription<my_robot_interfaces::msg::TurtleArray>(
            "alive_turtles", 10, std::bind(&TurtleControllerNode::callbackAliveTurtles, this, _1));
        catch_turtle_client_ = this->create_client<my_robot_interfaces::srv::CatchTurtle>(
            "catch_turtle");
        control_loop_timer_ = this->create_wall_timer(
            0.01s, std::bind(&TurtleControllerNode::controlLoop, this));
    }

private:

    void callCatchTurtleService(std::string turtle_name)
    {
        while (!catch_turtle_client_->wait_for_service(1s))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Service Server to be up...");
        }

        auto request = std::make_shared<my_robot_interfaces::srv::CatchTurtle::Request>();
        request->name = turtle_name;

        catch_turtle_client_->async_send_request(
            request, std::bind(&TurtleControllerNode::callbackCallCatchTurtleService, this, _1));
    }

    void callbackCallCatchTurtleService(
        rclcpp::Client<my_robot_interfaces::srv::CatchTurtle>::SharedFuture future)
    {
        auto response = future.get();
        if (response->success)
        {
            RCLCPP_INFO(this->get_logger(), "Successfully caught turtle!");
        }
        else
        {
            RCLCPP_WARN(this->get_logger(), "Failed to catch turtle.");
        }
    }
    void callbackPose(const turtlesim::msg::Pose::SharedPtr pose)
    {
        pose_ = *pose.get();
        turtlesim_up_ = true;
    }

    void callbackAliveTurtles(const my_robot_interfaces::msg::TurtleArray::SharedPtr msg)
    {
        double min_distance = std::numeric_limits<double>::max();
        my_robot_interfaces::msg::Turtle closest_turtle;
        bool found_turtle = false;
        if (catch_closest_turtle_)
        {
            if (!msg->turtles.empty())
        {
            for( int i = 0; i < (int)msg->turtles.size(); i++)
            {
                double dist_x = msg->turtles.at(i).x - pose_.x;
                double dist_y = msg->turtles.at(i).y - pose_.y;
                double distance = std::sqrt(dist_x * dist_x + dist_y * dist_y);
                if (distance < min_distance)
                {
                    min_distance = distance;
                    closest_turtle = msg->turtles.at(i);
                    found_turtle = true;
                }

            }
        }
        if (found_turtle)
        {
            turtle_to_catch_ = closest_turtle;
        }
        }
        else
        {
            turtle_to_catch_ = msg->turtles.at(0);
        }
    }

    void publishCmdVel(double x, double theta)
    {
        auto msg = geometry_msgs::msg::Twist();
        msg.linear.x = x;
        msg.angular.z = theta;
        cmd_vel_publisher_->publish(msg);
    }

    void controlLoop()
    {
        if (!turtlesim_up_ || turtle_to_catch_.name == "")
        {
            return;
        }

        double dist_x = turtle_to_catch_.x - pose_.x;
        double dist_y = turtle_to_catch_.y - pose_.y;
        double distance = std::sqrt(dist_x * dist_x + dist_y * dist_y);

        auto msg = geometry_msgs::msg::Twist();

        if (distance > 0.5)
        {
            // position
            msg.linear.x = 2 * distance;

            // orientation
            double steering_angle = std::atan2(dist_y, dist_x);
            double angle_diff = steering_angle - pose_.theta;
            if (angle_diff > M_PI)
            {
                angle_diff -= 2 * M_PI;
            }
            else if (angle_diff < -M_PI)
            {
                angle_diff += 2 * M_PI;
            }
            msg.angular.z = 6 * angle_diff;
        }
        else
        {
            // target reached!
            msg.linear.x = 0.0;
            msg.angular.z = 0.0;
            callCatchTurtleService(turtle_to_catch_.name);
            turtle_to_catch_.name = "";
        }

        cmd_vel_publisher_->publish(msg);
    }

    std::string name_;
    turtlesim::msg::Pose pose_;
    bool turtlesim_up_;
    bool catch_closest_turtle_;
    my_robot_interfaces::msg::Turtle turtle_to_catch_;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscriber_;
    rclcpp::Subscription<my_robot_interfaces::msg::TurtleArray>::SharedPtr alive_turtles_subscriber_;
    rclcpp::TimerBase::SharedPtr control_loop_timer_;
    rclcpp::Client<my_robot_interfaces::srv::CatchTurtle>::SharedPtr catch_turtle_client_;
    
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleControllerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}