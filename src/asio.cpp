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

static void cleanup_curses() {
    echo();
    keypad(stdscr, FALSE);
    nocbreak();
    endwin();
}

static void curses_print(DriveTarget target, std::string_view extra)
{
    clear();
    auto msg = fmt::format(
        "Teleop: use 'WASD'/'Arrows' to control robot.\n"
        "'Shift' for slide mode. 'Space' to stop. 'Ctrl+C' to exit\n"
        "X:{:-10} Rot:{:-10}\n\n{}",
        target.x, target.th, extra
    );
    printw("%s", msg.c_str());
}

static void curses_loop(asio::io_context& io, ITeleop* tele)
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    timeout(0);
    curs_set(0);
    std::atexit(cleanup_curses);
    std::shared_ptr<std::atomic<bool>> print{new std::atomic<bool>{true}};
    while (!io.stopped()) {
        refresh();
        int ch = getch();
        if (print->exchange(false)) {
            curses_print(tele->get_target(), tele->extra_msg());
        }
        if (ch != ERR) {
            asio::post([=]{
                tele->handle_press(ch);
                print->store(true);
            });
        }
        std::this_thread::sleep_for(5ms);
    }
}

void asio_loop(ITeleop* tele, std::function<void()> _spin)
{
    asio::io_context io;
    asio::co_spawn(io, spin(io, std::move(_spin)), asio::detached);
    asio::co_spawn(io, ctrlc(io), asio::detached);
    std::jthread inputs(curses_loop, std::ref(io), tele);
    io.run();
}