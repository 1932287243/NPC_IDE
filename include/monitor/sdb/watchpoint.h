#ifndef __WATCHPOINTMANAGER_H__
#define __WATCHPOINTMANAGER_H__

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#define NR_WP 32 // 定义观察点池大小

class WatchPoint {
public:
    int NO;             // 观察点编号
    bool enb = false;   // 是否启用
    std::string expr;   // 表达式
    WatchPoint* next = nullptr; // 指向下一个观察点的指针

    WatchPoint() : NO(0), enb(false), expr(""), next(nullptr) {}
    
};

class WatchPointManager {
public:
    // bool stop_flag;
    WatchPointManager();
    ~WatchPointManager();
    void initPool();

    WatchPoint* newWatchPoint();
    WatchPoint* head;          // 已使用观察点链表头指针
    
    void freeWatchPoint(WatchPoint* wp);

    bool freeWatchPointByNo(int w_no);

    void printWatchPoints() const;

// protected:
//     void run() override;

private:
    WatchPoint wp_pool[NR_WP]; // 观察点池
    WatchPoint* free_;         // 空闲观察点链表头指针
    
};
#endif