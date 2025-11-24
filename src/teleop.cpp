#include "common.hpp"
#include "curses.h"
#include <cstdlib>
#include <fmt/core.h>

struct Teleop final : ITeleop
{
    IMotor** motors;

    Teleop(IMotor** mot) : motors(mot) {}

    void handle_press(int ch) override {
        
    }
};

ITeleop* make_teleop(IMotor** motors)
{
    return new Teleop{motors};
}