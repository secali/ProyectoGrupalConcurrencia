#include <chrono>
#include <thread>
#include <future>
#include <vector>

#include "pel_print.hpp"

using namespace std::chrono_literals;

auto op(int &n) -> void
{
    n *= 2;
    std::this_thread::sleep_for(10ms);
}

auto main() -> int
{
    auto nums = std::vector<int>{};
    for (auto i = 0; i < 1000; ++i)
    {
        nums.push_back(i);
    }

   auto const hc = std::thread::hardware_concurrency();
   pel::println("hardware concurrency: {}", hc);

   auto const max_cunk_sz = nums.size()/hc;

   auto for_each = [&] (auto chunk) -> void
   {
      for (int& i : chunk){
         op(i);
      }
   };

   auto futures = std::vector<std::future <void>> {};
   auto first = nums.begin();
   auto last = first + max_cunk_sz;

   for(auto i=0u; i< hc -1; i++){
      futures.push_back(std::async(std::launch::async, for_each, std::ranges::subrange{first, last}));

      first = last;
      last+ max_cunk_sz;
   }

   for_each(std::ranges::subrange{first, nums.end()});

   for(std::future <void>& f: futures){
      f.wait();
   }
}