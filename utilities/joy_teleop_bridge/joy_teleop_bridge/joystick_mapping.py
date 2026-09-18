from dataclasses import dataclass

@dataclass(slots=True)
class JoystickMapping:
    LEFT_STICK_HORIZONTAL: int = 0
    LEFT_STICK_VERTICAL: int = 1
    RIGHT_STICK_HORIZONTAL: int = 3
    RIGHT_STICK_VERTICAL: int = 4
    LEFT_TRIGGER: int = 2 
    RIGHT_TRIGGER: int = 5
    A_BUTTON: int = 0  # Cross
    B_BUTTON: int = 1  # Circle
    X_BUTTON: int = 3  # Square
    Y_BUTTON: int = 2  # Triangle
    L1_BUTTON: int = 4