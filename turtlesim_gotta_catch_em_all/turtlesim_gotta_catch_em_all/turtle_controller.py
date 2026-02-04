#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from turtlesim.msg import Pose
from turtlesim.srv import Kill
from geometry_msgs.msg import Twist
from my_robot_interfaces.msg import Turtle
from my_robot_interfaces.msg import TurtleArray
from my_robot_interfaces.srv import CatchTurtle
import math
from functools import partial


class TurtleController(Node):
    def __init__(self):
        super().__init__("turtle_controller")
        self.pose = None
        self.turtle_target_: Turtle = None
        self.catch_closest_turtle = self.declare_parameter('catch_closest_turtle', True).get_parameter_value().bool_value
        self.pose_sub = self.create_subscription(
            Pose,
            '/turtle1/pose',
            self.pose_callback,
            10
        )
        self.cmd_pub_ = self.create_publisher(
            Twist,
            '/turtle1/cmd_vel',
            10
        )
        self.alive_turtles_sub = self.create_subscription(
            TurtleArray, "alive_turtles", self.callback_alive_turtles, 10
        )
        self.catch_turtle_client_ = self.create_client(CatchTurtle, 'catch_turtle')
        self.timer = self.create_timer(0.01, self.control_loop)  # 100 Hz

    def callback_alive_turtles(self, msg: TurtleArray):
        if len(msg.turtles) > 0:
            if self.catch_closest_turtle:
                smallest_distance = None
                self.turtle_target_ = None
                for turtle in msg.turtles:
                    dx = turtle.x - self.pose.x
                    dy = turtle.y - self.pose.y
                    distance = math.sqrt(dx ** 2 + dy ** 2)
                    if smallest_distance is None or distance < smallest_distance:
                        smallest_distance = distance
                        self.turtle_target_ = turtle
            else:
                self.turtle_target_ = msg.turtles[0]

    def pose_callback(self, msg):
        self.pose = msg

    def control_loop(self):
        if self.pose is None or self.turtle_target_ is None:
            return
        dx = self.turtle_target_.x - self.pose.x
        dy = self.turtle_target_.y - self.pose.y
        distance = math.sqrt(dx ** 2 + dy ** 2)
        angle_to_target = math.atan2(dy, dx)
        angle_diff = angle_to_target - self.pose.theta
        # Normalize angle_diff to [-pi, pi]
        while angle_diff > math.pi:
            angle_diff -= 2 * math.pi
        while angle_diff < -math.pi:
            angle_diff += 2 * math.pi
        cmd = Twist()
        # Simple P controller
        if distance > 0.1:
            cmd.linear.x = 2.0 * distance
            cmd.angular.z = 6.0 * angle_diff
        else:
            cmd.linear.x = 0.0
            cmd.angular.z = 0.0
            self.call_catch_turtle_service(self.turtle_target_.name)
            self.turtle_target_ = None
        self.cmd_pub_.publish(cmd)

    def call_catch_turtle_service(self,turtle_name):
        while not self.catch_turtle_client_.wait_for_service(1.0):
            self.get_logger().info('Waiting for catch service...')

        request = CatchTurtle.Request()
        request.name = turtle_name

        future = self.catch_turtle_client_.call_async(request)
        future.add_done_callback(
            partial(self.callback_call_catch_turtle_service, turtle_name = turtle_name))

    def callback_call_catch_turtle_service(self, future, turtle_name):
        response: CatchTurtle.Response = future.result()
        if not response.success:
            self.get_logger().info('Failed to catch turtle: ' + turtle_name)

def main(args=None):
    rclpy.init(args=args)
    node = TurtleController()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()
