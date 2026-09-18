#!/usr/bin/env python3
#
# This file is part of CyberShip Enterpries Suite.
#
# CyberShip Enterpries Suite software is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CyberShip Enterpries Suite is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# CyberShip Enterpries Suite. If not, see <https://www.gnu.org/licenses/>.
#
# Maintainer: Emir Cem Gezer
# Email: emir.cem.gezer@ntnu.no, emircem.gezer@gmail.com, me@emircem
# Year: 2024
# Copyright (C) 2024 NTNU Marine Cybernetics Laboratory

import rclpy
import rclpy.node
import sensor_msgs.msg
import geometry_msgs.msg

from .joystick_mapping import JoystickMapping

class Joystick_Teleop_Twist(rclpy.node.Node):

    def __init__(self):
        super().__init__('joystick_teleop_twist')

        self.pubs = {}
        self.subs = {}

        self.subs["joy"] = self.create_subscription(
            sensor_msgs.msg.Joy, '/joy', self.joy_callback, 10)


        self.pubs["cmd_vel"] = self.create_publisher( 
            geometry_msgs.msg.TwistStamped, '/cmd_vel', 10)


        self.joystick_mapping = JoystickMapping()

        joystick_params = [
            'LEFT_STICK_HORIZONTAL', 'LEFT_STICK_VERTICAL', 'RIGHT_STICK_HORIZONTAL', 
            'RIGHT_STICK_VERTICAL', 'LEFT_TRIGGER', 'RIGHT_TRIGGER', 
            'A_BUTTON', 'B_BUTTON', 'X_BUTTON', 'Y_BUTTON', 'L1_BUTTON'
        ]

        for param in joystick_params:
            self.declare_parameter(param, getattr(self.joystick_mapping, param))

        for param in joystick_params:
            setattr(self.joystick_mapping, param, self.get_parameter(param).value)

        self.declare_parameter('linear_x_scale', 1.0)
        self.declare_parameter('linear_y_scale', 1.0)
        self.declare_parameter('angular_z_scale', 1.0)

        self.linear_x_scale = self.get_parameter('linear_x_scale').value
        self.linear_y_scale = self.get_parameter('linear_y_scale').value
        self.angular_z_scale = self.get_parameter('angular_z_scale').value

        self.sent_stop = False

    @staticmethod
    def axis_value(msg, axis, scale=1.0):
        if 0 <= axis < len(msg.axes):
            return msg.axes[axis] * scale
        return 0.0

    def joy_callback(self, msg):
        deadman_pressed = (
            0 <= self.joystick_mapping.L1_BUTTON < len(msg.buttons)
            and msg.buttons[self.joystick_mapping.L1_BUTTON] == 1
        )

        if not deadman_pressed:
            if not self.sent_stop:
                self.pubs['cmd_vel'].publish(self.make_twist_stamped())
                self.sent_stop = True
            return

        cmd_vel = self.make_twist_stamped()
        cmd_vel.twist.linear.x = self.axis_value(
            msg,
            self.joystick_mapping.LEFT_STICK_VERTICAL,
            self.linear_x_scale,
        )
        cmd_vel.twist.linear.y = self.axis_value(
            msg,
            self.joystick_mapping.LEFT_STICK_HORIZONTAL,
            self.linear_y_scale,
        )
        cmd_vel.twist.angular.z = self.axis_value(
            msg,
            self.joystick_mapping.RIGHT_STICK_HORIZONTAL,
            self.angular_z_scale,
        )
        self.pubs['cmd_vel'].publish(cmd_vel)
        self.sent_stop = False

    def make_twist_stamped(self):
        cmd_vel = geometry_msgs.msg.TwistStamped()
        cmd_vel.header.stamp = self.get_clock().now().to_msg()
        return cmd_vel


def main(args=None):
    # Initialize the node
    rclpy.init(args=args)

    rclpy.spin(Joystick_Teleop_Twist())

    rclpy.shutdown()


if __name__ == '__main__':
    main()