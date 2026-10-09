#pragma once

#include "api.h"   
#include "main.h"
#include <vector>



//to whom may use this header
//plz give this to the next class

//you might want to change or rewrite, just do it

//I strongly advice not to put all the const here, put it in main

//usually dont use the forward_time modular which is not accurate,
//  also it might run slightly turned
//  yep you can add offset, we give you the function to do that

//in prepare to use this, you would need these constants:
//drivetrain ports(left and right), gyro port, wheel diameter, encoder unit, encoder wheel diameter
//you will need to develop your own function for the mechanical part
//this header is just for the drivetrain, 



class Robot;

class Two_way_Motor{
    //Two_way_Motor
    //creates a unified interface for the two way motor
    public:
    Two_way_Motor(Robot* robot, pros::Motor Motor, pros::controller_digital_e_t key1, pros::controller_digital_e_t key2, int8_t speed = 127);
    void  forward_distance(int angle);
    void  backward_distance(int angle);
    friend class Robot;

    private:
    Robot* robot;
    pros::Motor Motor;
    pros::controller_digital_e_t key1;
    pros::controller_digital_e_t key2;
    int8_t speed;
};

class Two_way_Motor_Group{
    //Two_way_Motor_Group
    //creates a unified interface for the two way motor group
    public:
    Two_way_Motor_Group(Robot* robot, pros::MotorGroup MotorGroup, pros::controller_digital_e_t key1, pros::controller_digital_e_t key2, int8_t speed = 127);
    void  forward_distance(int angle);
    void  backward_distance(int angle);
    friend class Robot;

    private:
    Robot* robot;
    pros::MotorGroup& MotorGroup;
    pros::controller_digital_e_t key1;
    pros::controller_digital_e_t key2;
    int8_t speed;
};


enum Drive_Type {
    Tank,
    Arcade
};


enum class Error {
    Normal,
    Overheat
};



class Robot {
    //Robot
    //creates a unified interface especially for the drivetrain
    //also have an error holder.

public:

    Robot(std::initializer_list<int8_t> left_motor_group,  std::initializer_list<int8_t> right_motor_group, pros::Controller &controller, Drive_Type drive_type, float left_offset = 1, float right_offset = 1);
    //Here is how this works:
    //there is a pointer to the left and right motor group, and a pointer to the controller, and a drive type
    //your claim would be like this:
    //{
    //pros::Controller master(pros::E_CONTROLLER_MASTER);
    //pros::MotorGroup Left_Drivetrain = LEFT_MOTOR_PORTS;
    //pros::MotorGroup Right_Drivetrain = RIGHT_MOTOR_PORTS;
    //Robot Your_Bot_Name(Left_Drivetrain, Right_Drivetrain, master, Tank);
    //}
    //for drive type explanation, see the update_Driver_ctrl function

    inline void forward_distance(int distance);
    //forward_distance: move forward a certain distance, the unit is (not sure yet)

    inline void backward_distance(int distance);
    //backward_distance: move backward a certain distance, the unit is (not sure yet)

    inline void forward_time(int time, int8_t speed);
    //forward_time: move forward for a certain time, the unit is (not sure yet), speed is from 0 to 127

    inline void backward_time(int time, int8_t speed);
    //backward_time: move backward for a certain time, the unit is (not sure yet), speed is from 0 to 127

    inline void turnLeft(int angle);

    inline void turnRight(int angle);

    inline void update_Driver_ctrl();
    //update_Driver_ctrl: update the driver control, this function should be called in the main loop
    //for drive type explanation:
    //Tank: left stick controls left side, right stick controls right side
    //Arcade: left stick controls forward/backward, right stick controls turning

    Error Bot_State = Error::Normal;
    int Error_Handler_Task();
    friend class Two_way_Motor;
    std::vector<Two_way_Motor> Two_way_Motor_list;
    std::vector<Two_way_Motor_Group*> Two_way_Motor_Group_list;

private:
    pros::MotorGroup Left_Drivetrain;
    pros::MotorGroup Right_Drivetrain;
    pros::Controller* main_controller;
    Drive_Type bot_drive_type;

    float LEFT_MOTOR_OFFSET;
    float RIGHT_MOTOR_OFFSET;
    
    
	// Correct: Captures 'this' so the task knows which Robot instance to use
    
};

Robot::Robot(std::initializer_list<int8_t> left_motor_group,  std::initializer_list<int8_t> right_motor_group, pros::Controller &controller, Drive_Type drive_type, float left_offset, float right_offset): Left_Drivetrain(left_motor_group), Right_Drivetrain(right_motor_group), main_controller(&controller), bot_drive_type(drive_type), LEFT_MOTOR_OFFSET(left_offset), RIGHT_MOTOR_OFFSET(right_offset) {
        // Constructor implementation
        main_controller = &controller;
        bot_drive_type = drive_type;
        pros::Task error_task([this] { this->Error_Handler_Task(); });
    }

inline void Robot::update_Driver_ctrl(){
    if (main_controller == nullptr) return;
    switch (bot_drive_type) {
        case Tank: {
            int left_move = main_controller->get_analog(ANALOG_LEFT_Y);  
            int right_move = main_controller->get_analog(ANALOG_RIGHT_Y);  
            Left_Drivetrain.move(left_move*LEFT_MOTOR_OFFSET); 
            Right_Drivetrain.move(right_move*RIGHT_MOTOR_OFFSET);
            break;
        }
        case Arcade: {
            int dir = main_controller->get_analog(ANALOG_LEFT_Y);    
            int turn = main_controller->get_analog(ANALOG_RIGHT_X);  
            Left_Drivetrain.move((dir - turn)*LEFT_MOTOR_OFFSET);                      
            Right_Drivetrain.move((dir + turn)*RIGHT_MOTOR_OFFSET);                     
            break; 
        }
    }
    for (auto& motor : Two_way_Motor_list) {
        if (main_controller->get_digital(motor.key1)) {
            motor.Motor.move(motor.speed);  
        } else if (main_controller->get_digital(motor.key2)) {
            motor.Motor.move(-motor.speed); 
        } else {
            motor.Motor.move(0);    
        }
    }
    for (auto& motor_group : Two_way_Motor_Group_list) {
        if (main_controller->get_digital(motor_group->key1)) {
            motor_group->MotorGroup.move(motor_group->speed);  
        } else if (main_controller->get_digital(motor_group->key2)) {
            motor_group->MotorGroup.move(-motor_group->speed); 
        } else {
            motor_group->MotorGroup.move(0);    
        }
    }
}



inline int Robot::Error_Handler_Task(){
    while (true) {
        switch (Bot_State) {
            case Error::Normal:
                pros::lcd::print(0, 0, "Normal");
                break;
            case Error::Overheat:
                pros::lcd::print(0, 0, "Overheat");
                break;
            default:
                pros::lcd::print(0, 0, "Unknown Error");
                break;
        }
        
        pros::delay(20);
    }

    return 0; 
}


inline Two_way_Motor::Two_way_Motor(Robot* robot, pros::Motor Motor, pros::controller_digital_e_t key1, pros::controller_digital_e_t key2, int8_t speed) : robot(robot), Motor(Motor), key1(key1), key2(key2), speed(speed) {
        // Constructor implementation
        robot->Two_way_Motor_list.push_back(*this);
}

inline Two_way_Motor_Group::Two_way_Motor_Group(Robot* robot, pros::MotorGroup MotorGroup, pros::controller_digital_e_t key1, pros::controller_digital_e_t key2, int8_t speed) : robot(robot), MotorGroup(MotorGroup), key1(key1), key2(key2), speed(speed) {
        // Constructor implementation
        robot->Two_way_Motor_Group_list.push_back(this);
}

