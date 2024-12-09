#include "timer.h"

extern "C"
{
  #include <sys/time.h> // 引入时间相关的头文件
}

uint64_t NPCTimer::boot_time = 0;
uint64_t NPCTimer::now_time = 0;

NPCTimer::NPCTimer()
{
  
}
NPCTimer::~NPCTimer()
{

}

uint64_t NPCTimer::get_time_internal() {
    struct timeval now;
    gettimeofday(&now, nullptr);
    uint64_t us = now.tv_sec * 1000000 + now.tv_usec;
    return us;
}

uint64_t NPCTimer::get_time() {
    if (boot_time == 0) boot_time = get_time_internal();
    uint64_t now = get_time_internal();
    return now - boot_time;
}
