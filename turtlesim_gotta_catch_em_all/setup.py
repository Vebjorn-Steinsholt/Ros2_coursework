from setuptools import find_packages, setup

package_name = 'turtlesim_gotta_catch_em_all'

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
    maintainer='imt-hydrolab',
    maintainer_email='vebjorn.steinsholt@ntnu.no',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'controller = turtlesim_gotta_catch_em_all.turtle_controller:main',
            'spawner = turtlesim_gotta_catch_em_all.turtle_spawner:main',

        ],
    },
)
