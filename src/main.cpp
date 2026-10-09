#include "main.h"


#define BotA
#include "profile.hpp"

pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::MotorGroup Left_DriveTrain = LEFT_MOTOR_PORTS;
pros::MotorGroup Right_DriveTrain = RIGHT_MOTOR_PORTS;
void initialize() {
    pros::lcd::initialize();
}
void disabled() {}
void competition_initialize() {}
void autonomous() {}
void opcontrol() {
    while (true) {
        Left_DriveTrain.move(master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
        Right_DriveTrain.move(master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
        pros::delay(20);
    }
}