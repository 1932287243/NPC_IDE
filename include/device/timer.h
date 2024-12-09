#ifndef __TIMER_H__
#define __TIMER_H__

#include <cstdint>

class NPCTimer
{
public:
    NPCTimer();
    ~NPCTimer();
    static uint64_t now_time;
    static uint64_t get_time();
private:
    static uint64_t boot_time;
    static uint64_t get_time_internal();
};
#endif