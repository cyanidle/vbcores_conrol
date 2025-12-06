#include "common.hpp"
#include "curses.h"
#include <cstdint>
#include <cstdlib>
#include <fmt/core.h>

static void norm(float& x) {
    if (std::abs(x) > 1.0f) {
        x = x > 0 ? 1.0f : -1.0f;
    }
}

struct TeleopState
{
    bool operator<=>(TeleopState const&) const = default;

    DriveTarget target = {};
    int32_t encoder_values[4] = {};
    int last_char = 0;
};

struct Teleop final : ITeleop
{
    Io* io;
    IMotor** motors;
    TeleopState last_state = {};
    TeleopState state = {};

    Teleop(Io* io, IMotor** mot) : motors(mot), io(io) {
        for (int i = 0; i < 4; ++i) {
            mot[i]->set_encoder_callback([this, i](int32_t enc){
                state.encoder_values[i] = enc;
                update_msg();
            });
        }
    }

    void apply_target() {
        for (int i: {0, 1, 2, 3}) {
            float sign = (i >= 2) ? -1.f : 1.f; 
            float move = state.target.x * 0.7 * sign;
            float rot = state.target.th * -0.3;
            float res = move + rot;
            if (std::abs(res) < std::numeric_limits<float>::epsilon()) {
                res = 0;
            }
            motors[i]->set_target_speed(res);
        }
    }

    void update_msg() {
        if (state == last_state)
            return;
        last_state = state;
        io->print(state.target, fmt::format("Last: '{}'.\nEncoders: [{:^5}|{:^5}|{:^5}|{:^5}]", char(state.last_char),
            state.encoder_values[0],
            state.encoder_values[1],
            state.encoder_values[2],
            state.encoder_values[3]
        ));
    }

    void handle_press(int ch) override {
        state.last_char = ch;
        update_msg();
        switch (ch) {
        case 'w': case 'W': {
            state.target.x += 0.1;
            break;
        }
        case 's': case 'S': {
            state.target.x -= 0.1;
            break;
        }
        case 'a': case 'A': {
            state.target.th += 0.1;
            break;
        }
        case 'd': case 'D': {
            state.target.th -= 0.1;
            break;
        }
        case 'q': case 'Q': {
            state.target.x += 0.1;
            state.target.th += 0.1;
            break;
        }
        case 'e': case 'E': {
            state.target.x += 0.1;
            state.target.th -= 0.1;
            break;
        }
        case 'z': case 'Z': {
            state.target.x -= 0.1;
            state.target.th -= 0.1;
            break;
        }
        case 'c': case 'C': {
            state.target.x -= 0.1;
            state.target.th += 0.1;
            break;
        }
        case ' ': {
            state.target = {};
            break;
        }
        }
        norm(state.target.x);
        norm(state.target.th);
        apply_target();
    }
};

ITeleop* make_teleop(Io* io, IMotor** motors)
{
    return new Teleop{io, motors};
}