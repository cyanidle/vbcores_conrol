#include "common.hpp"
#include <boost/asio.hpp>
#include <boost/asio/dispatch.hpp>
#include <coroutine>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <fmt/core.h>
#include <ncurses.h>

using namespace std::chrono_literals;
using namespace boost;

[[maybe_unused]] static asio::awaitable<void> shutdown_later(asio::io_context& io, auto timeout)
{
    asio::high_resolution_timer timer(io, timeout);
    co_await timer.async_wait(asio::use_awaitable);
    io.stop();
}

static asio::awaitable<void> ctrlc(asio::io_context& io)
{
    asio::signal_set signals(io, SIGINT);
    co_await signals.async_wait(asio::use_awaitable);
    io.stop();
}

static asio::awaitable<void> call_each(asio::io_context& io, std::function<void()> func, auto timeout)
{
    asio::high_resolution_timer timer(io);
    while(true) {
        timer.expires_after(timeout);
        co_await timer.async_wait(asio::use_awaitable);
        func();
    }
}

static void cleanup_curses() {
    echo();
    keypad(stdscr, FALSE);
    nocbreak();
    endwin();
}

struct Asio final : Io
{
    asio::io_context io;
    std::jthread inputs;
    ITeleop* tele = nullptr;

    Asio() :
        inputs(&Asio::curses_loop, this)
    {
        asio::co_spawn(io, ctrlc(io), asio::detached);
    }

    void spawn(unsigned millis, std::function<void()> func) override {
        asio::co_spawn(io, call_each(io, func, 1s * millis), asio::detached);
    }

    void dispatch(std::function<void()> func) override {
        asio::dispatch(io, std::move(func));
    }

    void set_teleop(ITeleop* tele) override {
        this->tele = tele;
    }

    void loop() override {
        io.run();
    }

    void print(DriveTarget const& target, std::string_view extra) override {
        clear();
        auto msg = fmt::format(
            "Teleop: use 'WASD' + 'QEZC' to control robot.\n"
            "'Space' to stop. 'Ctrl+C' to exit\n"
            "X:{:-10} Rot:{:-10}\n\n{}",
            target.x, target.th, extra
        );
        printw("%s", msg.c_str());
    }

    void curses_loop()
    {
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        timeout(0);
        curs_set(0);
        std::atexit(cleanup_curses);
        print({}, {});
        while (!io.stopped()) {
            refresh();
            int ch = getch();
            if (ch != ERR) {
                if (tele) {
                    asio::post([tele = tele, ch]{
                        tele->handle_press(ch);
                    });
                }
            }
            std::this_thread::sleep_for(5ms);
        }
    }
};

Io* make_asio()
{
    return new Asio;
}