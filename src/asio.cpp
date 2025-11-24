#include "common.hpp"
#include <boost/asio.hpp>
#include <coroutine>
#include <chrono>
#include <thread>
#include <fmt/core.h>
#include <ncurses.h>

using namespace std::chrono_literals;
using namespace boost;

static asio::awaitable<void> ctrlc(asio::io_context& io)
{
    asio::signal_set signals(io, SIGINT);
    co_await signals.async_wait(asio::use_awaitable);
    io.stop();
}

static asio::awaitable<void> spin(asio::io_context& io, std::function<void()> spin)
{
    asio::high_resolution_timer timer(io);
    while(true) {
        timer.expires_after(1ms);
        co_await timer.async_wait(asio::use_awaitable);
        spin();
    }
}

struct CursesInit {
    CursesInit() {
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        timeout(0);
        curs_set(0);
    }
    ~CursesInit() {
        echo();
        keypad(stdscr, FALSE);
        nocbreak();
        endwin();
    }
};

void asio_loop(ITeleop* tele, std::function<void()> _spin)
{
    asio::io_context io;
    asio::co_spawn(io, spin(io, std::move(_spin)), asio::detached);
    asio::co_spawn(io, ctrlc(io), asio::detached);

    CursesInit curses;
    std::jthread inputs([&]{
        clear();
        printw("Teleop: use WASD/Arrows to control robot. Ctrl+C or q to exit");
        while (!io.stopped()) {
            int ch = getch();
            if (ch != ERR) {
                asio::post([&, ch]{
                    tele->handle_press(ch);
                });
            }
            if (ch == 'q') {
                io.stop();
            }
            std::this_thread::sleep_for(5ms);
        }
    });

    io.run();
}