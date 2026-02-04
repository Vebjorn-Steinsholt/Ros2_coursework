from setuptools import find_packages, setup

package_name = 'move_robot'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='vebjorn-steinsholt',
    maintainer_email='vebjorn.steinsholt@ntnu.no',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            "move_robot_server = move_robot.move_robot_server:main",
            "move_robot_client = move_robot.move_robot_client:main",
        ],
    },
)
