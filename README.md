# ROS2 Coursework Projects

This directory contains various ROS2 learning projects and coursework assignments.

## Project Structure

- **actions_cpp/** - ROS2 Actions implementation in C++
- **actions_py/** - ROS2 Actions implementation in Python
- **components_cpp/** - ROS2 Components in C++
- **components_py/** - ROS2 Components in Python
- **executors_cpp/** - ROS2 Executors in C++
- **executors_py/** - ROS2 Executors in Python
- **lifecycle_cpp/** - ROS2 Lifecycle nodes in C++
- **lifecycle_py/** - ROS2 Lifecycle nodes in Python
- **my_robot_bringup/** - Robot bringup configuration
- **my_robot_description/** - Robot URDF and models
- **my_robot_interfaces/** - Custom message and service definitions
- **move_robot/** - Robot motion control
- **turtlesim_gotta_catch_em_all/** - Turtlesim game project
- **turtlesim_project_cpp/** - Turtlesim project in C++
- **final_project/** - Capstone/final project

## Building

```bash
cd ~/ros2_ws
colcon build --packages-select ros2_coursework
# Or to exclude from main build:
colcon build --packages-ignore ros2_coursework
```

## Notes

These are learning projects from ROS2 courses and tutorials. They serve as examples and practice for various ROS2 concepts.
