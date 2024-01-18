#include <chrono>
#include <thread>
#include <future>

#include "pel_print.hpp"

using namespace std::chrono_literals;

auto do_something_1(int i) -> int
{
    pel::println("starting ds1");
    std::this_thread::sleep_for(1s);
    pel::println("ending ds1");

    return i+1;
}


auto do_something_2() -> void
{
    pel::println("starting ds2");
    std::this_thread::sleep_for(3s);
    pel::println("ending ds2");
}


auto main () -> int
{
    using clock = std::chrono::steady_clock;

   auto const start = clock::now();
    std::future <int> ftr = std::async(std::launch::async, do_something_1, 9);
    do_something_2();
   
   auto j = ftr.get();
   pel::println("j = {}", j);

    auto const end = clock::now();

    pel::println("Duration: {}", std::chrono::duration_cast<std::chrono::seconds>( end- start).count());

}