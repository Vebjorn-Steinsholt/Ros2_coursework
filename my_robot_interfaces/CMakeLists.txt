cmake_minimum_required(VERSION 3.8)
project(my_robot_interfaces)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
 "msg/HardwareStatus.msg"
 "msg/LedStateArray.msg"
  "msg/Turtle.msg"
  "msg/TurtleArray.msg"
 "srv/ComputeRectangleArea.srv"
 "srv/SetLed.srv"
 "srv/CatchTurtle.srv"
 "action/CountUntil.action"
 "action/MoveRobot.action"
 "action/MoveTurtle.action"
 "srv/CreateFolder.srv"

)
# Export the package include directory so downstream targets using
# ament_target_dependencies(...) will pick up the include path.
ament_export_include_directories(include)
ament_export_dependencies(rosidl_default_runtime)


ament_package()
