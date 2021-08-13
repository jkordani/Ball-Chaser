#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    unsigned int row_full = img.step * img.height;
    unsigned int col_full = img.step * img.width;

    unsigned int col = 0;

    for (unsigned int row = 0; row < row_full; ++row) {
      for ( ; col < col_full;  ++col) {
	int idx =  row * row_full + col;
	if (img.data[idx] == white_pixel) {
	  break;
	}
      }
    }

    ball_chaser::DriveToTarget srv;

    float while_straight_speed = 0.5;
    float while_turning_speed = 0.3;
    float steer_radps = .3;

    float commanded_speed, commanded_steer = 0;

    if ( col / col_full < col_full / 3 ) {
      //left
      commanded_speed = while_turning_speed;
      commanded_steer = -steer_radps;

    } else if ( (col / col_full) < ( 2 * col_full / 3) ) {
      //straight
      commanded_speed = while_straight_speed;
      commanded_steer = 0;

    } else {
      //right or search
      commanded_speed = while_turning_speed;
      commanded_steer = steer_radps;
    }

    srv.linear_x = commanded_speed;
    srv.angular_x = commanded_steer;

    client.call(srv);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
