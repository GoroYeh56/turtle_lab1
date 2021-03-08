#include "ros/ros.h"
#include "turtlesim/Pose.h"
#include "geometry_msgs/Twist.h"

#define PI 3.14159

ros::Publisher pub;
ros::Subscriber sub;


/****
void your_callback_function(const turtlesim::PoseConstPtr& msg)
{
	Do your callback function here.
	In this callback function, you need to write something
	to make the turtle move with square trajectory.

	...
}
****/



/*
Pose msg:
float32 x
float32 y
float32 theta

float32 linear_velocity
float32 angular_velocity
*/

enum State{RIGHT, UP, LEFT, DOWN};

enum Command{FORWARD, TURN_LEFT};

State state = RIGHT; //Initial turtle state/pose.

Command cmd = FORWARD; // Initial command 

	// Finite state machine.
	// If cmd == RIGHT
		// if x < 10, stay
		// if x >=10, switch cmd.
	// Else if cmd == UP:

const float left_bottom_x = 5.5;
const float left_bottom_y = 5.5;
const float right_top_x = 8.5;
const float right_top_y = 8.5;

/* Turning Threshold */
const float UP_THETA = 1.55; // PI/2 = 1.57
const float LEFT_THETA = 3.12;
const float DOWN_THETA = -1.6;
const float RIGHT_THETA = 0; // 0.


void poseCallback(const turtlesim::PoseConstPtr& msg){
	float x = msg->x;
	float y = msg->y;
	float theta = msg->theta;

	switch (state)
	{
	case RIGHT:
		if(x>=right_top_x){
			cmd = TURN_LEFT;
			ROS_INFO("Now switch to UP state.");
            if(theta >= UP_THETA){
                // Done turning UP.
                state = UP;
            } 
            else{
                ROS_INFO("Now turning...");
            }
		}
		else{
			cmd = FORWARD;
		}
	break;
	case UP:
		if(y>=right_top_y){
			cmd = TURN_LEFT;
			ROS_INFO("Now switch to LEFT state.");
            if(theta >= LEFT_THETA){
                // Done turning LEFT.
                state = LEFT;
            } 
            else{
                ROS_INFO("Now turning...");
            }            
		}
		else{
			cmd = FORWARD;
		}
	break;
	case LEFT:
		if(x<=left_bottom_x){
			cmd = TURN_LEFT;
			ROS_INFO("Now switch to DOWN state.");

            if( theta<=0 && theta >= DOWN_THETA ){
                // Done turning DOWN
                state = DOWN;
            } 
            else{
                ROS_INFO("Now turning...");
            }

		}
		else{
			cmd = FORWARD;
		}
	break;
	case DOWN:
		if(y<=left_bottom_y){
			cmd = TURN_LEFT;
			ROS_INFO("Now switch to RIGHT state.");
            if(theta >= RIGHT_THETA){
                // Done turning RIGHT.
                state = RIGHT;
            } 
            else{
                ROS_INFO("Now turning...");
            }
		}
		else{
			cmd = FORWARD;
		}
	break;	

	default:
		break;
	}


}


/*
	Note: the node /turtlesim_node would
	publish /pose constantly.
	We subscribe the pose and decide to change "cmd_vel" msg.
	And publish /cmd_vel continually.
*/


const float l_scale_ = 1.0;
const float a_scale_ = 1.0;


int main(int argc, char **argv)
{
    // Initialize the node here
	ros::init(argc, argv, "turtle");
    ros::NodeHandle node;

	ros::Publisher cmd_pub = node.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);

	ros::Subscriber sub = node.subscribe("turtle1/pose", 1000, poseCallback);

    // Set the publish rate here
	ros::Rate rate(100);
    
	float angular_, linear_;
	geometry_msgs::Twist twist;
    
	while (ros::ok()) 
	{
			


        /*
            Only 2 kinds of command:
            1. Go forward.
            2. Turn left.

            Forward: twist.linear =1, angular=0.
            Turn Left: twist.linear=0, angular=1.
        */

        switch (cmd)
        {
        case FORWARD:
            linear_ = 1.0;
            angular_ = 0.0;
        break;
        case TURN_LEFT:
            linear_ = 0.0;
            angular_ = 1.0;
        break;
 
        default:
            break;
        }

		// switch(cmd)
		// 	{
		// 	case LEFT:
		// 		ROS_DEBUG("LEFT");
		// 		angular_ = 1.0;
		// 		//  dirty = true;
		// 		break;
		// 	case RIGHT:
		// 		ROS_DEBUG("RIGHT");
		// 		angular_ = -1.0;
		// 		//  dirty = true;
		// 		break;
		// 	case UP:
		// 		ROS_DEBUG("UP");
		// 		linear_ = 1.0;
		// 		//  dirty = true;
		// 		break;
		// 	case DOWN:
		// 		ROS_DEBUG("DOWN");
		// 		linear_ = -1.0;
		// 		//  dirty = true;
		// 		break;
		// 	}
			
		twist.angular.z = a_scale_*angular_;
		twist.linear.x = l_scale_*linear_;
		cmd_pub.publish(twist); 
		// if(dirty ==true)
		// {
		//   twist_pub_.publish(twist);    
		//   dirty=false;
		// }


        ros::spinOnce();    // Allow processing of incoming messages

        rate.sleep();
    }
}

