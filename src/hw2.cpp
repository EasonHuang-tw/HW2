// include ros library
#include "ros/ros.h"

// include msg library
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
// include cpp library 
#include <cstdio>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cmath> 
#define PI 3.14159265
#define Kp 1.1
#define Kp_x 0.5
using namespace std;
geometry_msgs::Twist vel_msg;
turtlesim::Pose turtle_position;
int flag = 0;
void Callback(const turtlesim::Pose::ConstPtr& msg){
  flag =1;
  turtle_position = *msg;
}
int main(int argc, char **argv)
{
  ros::init(argc, argv, "tutorial_1");
  ros::NodeHandle n;

  // declare publisher
  ros::Publisher turtlesim_pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 100);
  // setting frequency as 100 Hz
  ros::Rate loop_rate(100);
  //set param
  double target[2];    //[0] is x , [1] is y
  double error[2];
  double position_angle = 0;
  double error_angle = 0;
  vel_msg.linear.x = 0;
  vel_msg.angular.z = 0;

  cout<<"x:";       //desired x , y input
  cin>>target[0];
  cout<<"y:";
  cin>>target[1];
  ROS_INFO("target is : [%f,%f]",target[0],target[1]);
  ros::Subscriber sub = n.subscribe("turtle1/pose", 1000, Callback);
  int count = 0;
  while (ros::ok()){
    if(flag ==1 ){
      //start moving
      error[0] = target[0] - turtle_position.x ; //find error
      error[1] = target[1] - turtle_position.y ;
      cout<<"error_x:"<<error[0]<<endl;
      cout<<"error_y"<<error[1]<<endl;
      if(sqrt(pow(error[0],2.0)+pow(error[1],2.0))<0.5)
        vel_msg.linear.x = sqrt(pow(error[0],2.0)+pow(error[1],2.0))*Kp_x;
      else
        vel_msg.linear.x = 1;
      if (abs(error[0]) <= 0.002 && abs(error[1]) <= 0.002){
        cout<<"finish";
        break;
      }
      else{
        if(error[0] == 0.0)   //escape inffinite
          error[0]=0.000000000000001;

        //calculate theta 
        position_angle = atan (error[1]/error[0]);
        if(error[0]<0)
          position_angle>0 ? position_angle = -PI+position_angle:position_angle = PI+position_angle ;
        error_angle = position_angle-turtle_position.theta;
        if(error_angle>PI)
          error_angle-=2*PI;
        else if(error_angle<-PI)
          error_angle+=2*PI;
        
        cout<<"out put:" << Kp*error_angle<<endl;
        vel_msg.angular.z = Kp*error_angle;
        turtlesim_pub.publish(vel_msg);
      }
      flag=0;
      count ++;
    }




    
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}