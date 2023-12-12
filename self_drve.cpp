#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <nav_msgs/msg/odometry.hpp>

using namespace std::chrono_literals;

class SelfDrive : public rclcpp::Node
{
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pose_pub_;
  int step_;

public:
  SelfDrive() : rclcpp::Node("self_drive"), step_(0)
  {
    auto lidar_qos_profile = rclcpp::QoS(rclcpp::KeepLast(1));
    lidar_qos_profile.reliability(rclcpp::ReliabilityPolicy::BestEffort);
    auto callback = std::bind(&SelfDrive::subscribe_scan, this, std::placeholders::_1);
    scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan", lidar_qos_profile, callback);
    auto vel_qos_profile = rclcpp::QoS(rclcpp::KeepLast(1));
    pose_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", vel_qos_profile);
  }

  void subscribe_scan(const sensor_msgs::msg::LaserScan::SharedPtr scan)
  {
    geometry_msgs::msg::Twist vel;

    if (scan->ranges[0] < 0.25 || scan->ranges[340] < 0.25)
    {
      vel.linear.x = 0.;
      vel.angular.z = 0.75;
    }
    else if (((scan->ranges[270] < 0.25) || (scan->ranges[250] < 0.25) || (scan->ranges[230] < 0.25) ||
              (scan->ranges[200] < 0.25 || (scan->ranges[190] < 0.25))) &&
             !(scan->ranges[320] < 0.25))
    {
      vel.linear.x = 0.1;
      vel.angular.z = -0.3;
    }
    else
    {
      vel.linear.x = 0.15;
      vel.angular.z = 0.;
    }

    RCLCPP_INFO(rclcpp::get_logger("self_drive"), "step=%d, range=%1.2f, linear=%1.2f, angular=%1.2f", step_,
                scan->ranges[0], vel.linear.x, vel.angular.z);
    pose_pub_->publish(vel);
    step_++;
  }
};

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SelfDrive>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
