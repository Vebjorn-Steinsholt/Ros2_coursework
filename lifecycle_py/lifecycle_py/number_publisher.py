#!/usr/bin/env python3
import rclpy
from rclpy.lifecycle import LifecycleNode
from rclpy.lifecycle.node import LifecycleState, TransitionCallbackReturn
from example_interfaces.msg import Int64


class NumberPublisherNode(LifecycleNode):
    def __init__(self):
        super().__init__("number_publisher")
        self.get_logger().info("In Constructor")
        self.number_ = 1
        self.publish_frequency_ = 1.0
        self.number_publisher_ = None
        self.number_timer_ = None
        
    # Create ROS2 communication, connect to HW
    def on_configure(self, previous_state: LifecycleState):
        self.get_logger().info("In on_configure")
        self.number_publisher_ = self.create_lifecycle_publisher(Int64, "number", 10)
        self.number_timer_ = self.create_timer(
            1.0/self.publish_frequency_, self.publish_number)
        return TransitionCallbackReturn.SUCCESS

    # Activate/ Enable HW
    def on_activate(self, previous_state: LifecycleState):
        self.get_logger().info("In on_activate")
        return super().on_activate(previous_state)

    # Deactivate/ Disable HW
    def on_deactivate(self, previous_state: LifecycleState):
        self.get_logger().info("In on_deactivate")
        return super().on_deactivate(previous_state)

    # Destroy ROS2 communication, disconnect from HW
    def on_cleanup(self, previous_state: LifecycleState):
        self.get_logger().info("In on_cleanup")
        self.clean_up()
        return TransitionCallbackReturn.SUCCESS
    
     # Destroy ROS2 communication, disconnect from HW
    def on_shutdown(self, previous_state: LifecycleState):
        self.get_logger().info("In on_shutdown")
        self.clean_up()
        return TransitionCallbackReturn.SUCCESS
    
    def on_error(self, previous_state: LifecycleState):
        self.get_logger().info("In on_error")
        self.clean_up()
        # Do some checks, if ok return SUCESS, if not FAILURE
        return TransitionCallbackReturn.SUCCESS

    def clean_up(self):
        self.destroy_publisher(self.number_publisher_)
        self.destroy_timer(self.number_timer_)

    def publish_number(self):
        msg = Int64()
        msg.data = self.number_
        self.number_publisher_.publish(msg)
        self.number_ += 1

def main(args=None):
    rclpy.init(args=args)
    node = NumberPublisherNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()