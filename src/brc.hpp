

//to whom may use this header
//plz give this to the next class

//you might want to change or rewrite, just do it

//I strongly advice not to put all the const here, put it in main

#include "api.h"

class Robot {
public:
    Robot();
    void Robot::initialize(const std::vector<std::int8_t>& left_ports, const std::vector<std::int8_t>& right_ports, std::uint8_t Some_gyro_port) ;
    void forward(int distance);
    void backward(int distance);
    void turnLeft(int angle);
    void turnRight(int angle);

private:
    pros::MotorGroup left_mg;
    pros::MotorGroup right_mg;
    pros::ADIAnalogIn left_encoder;
    pros::ADIAnalogIn right_encoder;
    pros::ADIAnalogIn gyro_sensor;

};

void Robot::initialize(const std::vector<std::int8_t>& left_ports, const std::vector<std::int8_t>& right_ports, std::uint8_t Some_gyro_port) {

}
