import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    use_sim_time = LaunchConfiguration('use_sim_time', default='false')
    share_directory = get_package_share_directory('ros2_orb_slam3')
    print("Share directory: ", share_directory)

    # Path to your main JS file (now in server folder)
    # start_js_file = os.path.join(
    #     share_directory,
    #     'dist',
    #     'server',
    #     'index.js')
    
    # cert_directory = os.path.join(share_directory, 'dist')

    mono_node = Node(
        name='mono_node',
        package='ros2_orb_slam3',
        executable='mono_node_cpp',
        output='screen',
        parameters=[
            {'use_sim_time': use_sim_time},
            {'node_name_arg': 'mono_slam_cpp'},
            {'settings_file_path_arg': "/home/tomas/ros2_ws/my-cam-calib/Pixel8.yaml"},
            {'voc_file_arg': "/home/tomas/ros2_ws/src/ros2_orb_slam3/orb_slam3/Vocabulary/ORBvoc.txt.bin"}
        ])

    ld = LaunchDescription()
    ld.add_action(mono_node)

    return ld
