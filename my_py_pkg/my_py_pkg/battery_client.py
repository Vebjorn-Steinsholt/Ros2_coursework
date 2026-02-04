#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from my_robot_interfaces.srv import SetLed


class BatteryClientNode(Node):
    def __init__(self):
        super().__init__("battery_client")
        self.battery_empty_ = False
        self.last_time_battery_state_changed_ = self.get_current_time_in_seconds()    
        self.battery_timer_ = self.create_timer(0.1, self.check_battery_state)
        self.get_logger().info("Battery Client Node has been started")
        self.set_led_client_ = self.create_client(SetLed, "set_led")

    def get_current_time_in_seconds(self):
        seconds, nanoseconds = self.get_clock().now().seconds_nanoseconds()
        return seconds + nanoseconds * 1e-9 

    def check_battery_state(self):
        time_now = self.get_current_time_in_seconds()
        if not self.battery_empty_:
            if time_now - self.last_time_battery_state_changed_ > 4:
                self.battery_empty_ = True
                self.get_logger().info("Battery is empty. Charging")
                self.call_set_led(2, 1)
                self.last_time_battery_state_changed_ = time_now
        elif self.battery_empty_:
            if time_now - self.last_time_battery_state_changed_ > 6:
                self.battery_empty_ = False
                self.get_logger().info("Battery is full.")
                self.call_set_led(2, 0)
                self.last_time_battery_state_changed_ = time_now
    def call_set_led(self, led_number,state):
        while not self.set_led_client_.wait_for_service(1.0):
            self.get_logger().warn("Waiting for Set Led server...")
        
        request = SetLed.Request()
        request.led_number = led_number
        request.state = state

        future = self.set_led_client_.call_async(request)
        future.add_done_callback(self.callback_call_set_led)

    def callback_call_set_led(self, future):
        response: SetLed.Response = future.result()
        if response.success:
            self.get_logger().info("Led state changed successfully")
        else:
            self.get_logger().error("Failed to turn on led")

def main(args=None):
    rclpy.init(args=args)
    node = BatteryClientNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()
