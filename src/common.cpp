/*

A bare-bones example node demonstrating the use of the Monocular mode in ORB-SLAM3

Author: Azmyin Md. Kamal
Date: 01/01/24

REQUIREMENTS
* Make sure to set path to your workspace in common.hpp file

*/

//* Includes
#include "ros2_orb_slam3/common.hpp"

//* Constructor
MonocularMode::MonocularMode() :Node("mono_node_cpp")
{
    RCLCPP_INFO(this->get_logger(), "\nORB-SLAM3-V1 NODE STARTED");

    this->declare_parameter("voc_file_arg", "");
    this->declare_parameter("settings_file_path_arg", "");

    const std::string vocFileArg =
        this->get_parameter("voc_file_arg").as_string();
    const std::string settingsFilePathArg =
        this->get_parameter("settings_file_path_arg").as_string();

    RCLCPP_INFO(this->get_logger(), "vocFileArg %s", vocFileArg.c_str());
    RCLCPP_INFO(this->get_logger(), "settingsFilePathArg %s", settingsFilePathArg.c_str());

    subCImgMsg_subscription_ =
        this->create_subscription<sensor_msgs::msg::CompressedImage>(
            SUB_IMG_MSG_NAME, 1,
            std::bind(&MonocularMode::CImg_callback, this, _1));

    //* Wait to complete VSLAM initialization
    initializeVSLAM(vocFileArg, settingsFilePathArg);
}

//* Destructor
MonocularMode::~MonocularMode()
{      
    // Stop all threads
    // Call method to write the trajectory file
    // Release resources and cleanly shutdown
    pAgent->Shutdown();

}

//* Method to bind an initialized VSLAM framework to this node
void MonocularMode::initializeVSLAM(const std::string &strVocFile,
                                    const string &strSettingsFile) {
  // NOTE if you plan on passing other configuration parameters to ORB SLAM3
  // Systems class, do it here NOTE you may also use a .yaml file here to set
  // these values
  sensorType = ORB_SLAM3::System::MONOCULAR;
  enablePangolinWindow = true; // Shows Pangolin window output
  enableOpenCVWindow = true;   // Shows OpenCV window output

  pAgent = std::make_shared<ORB_SLAM3::System>(
      strVocFile, strSettingsFile, sensorType, enablePangolinWindow);
  std::cout << "MonocularMode node initialized"
            << std::endl; // TODO needs a better message
}

//* Callback to process image message and run SLAM node
void MonocularMode::CImg_callback(const sensor_msgs::msg::CompressedImage& msg)
{
    if (pAgent == nullptr)
    {
        RCLCPP_WARN(this->get_logger(), "ORB SLAM3 system not initialized yet");
        return;
    }

    // Initialize
    cv_bridge::CvImagePtr cv_ptr; //* Does not create a copy, memory efficient
    
    //* Convert ROS image to openCV image
    try
    {
        //cv::Mat im =  cv_bridge::toCvShare(msg.img, msg)->image;
        cv_ptr = cv_bridge::toCvCopy(msg); // Local scope
        // cv::resize(cv_ptr->image, cv_ptr->image, cv::Size2i(752, 480));

    }
    catch (cv_bridge::Exception& e)
    {
        RCLCPP_ERROR(this->get_logger(),"Error reading image");
        return;
    }

    double timeStamp = msg.header.stamp.sec + msg.header.stamp.nanosec * 1e-9;

    //* Perform all ORB-SLAM3 operations in Monocular mode
    //! Pose with respect to the camera coordinate frame not the world coordinate frame
    Sophus::SE3f Tcw = pAgent->TrackMonocular(cv_ptr->image, timeStamp); 
}
