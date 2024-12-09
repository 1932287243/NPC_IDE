#include "sdb/watchpoint.h"
#include "main.h"

WatchPointManager::WatchPointManager(): free_(nullptr), head(nullptr) {
    initPool();
    // stop_flag = false;
}

WatchPointManager::~WatchPointManager(){
    std::cout << "delete wp" << std::endl;
}

void WatchPointManager::initPool() {
    for (int i = 0; i < NR_WP; i++) {
        wp_pool[i].NO = i;
        wp_pool[i].next = (i == NR_WP - 1 ? nullptr : &wp_pool[i + 1]);
    }
    head = nullptr;
    free_ = wp_pool;
}

WatchPoint* WatchPointManager::newWatchPoint() {
    if (free_ == nullptr) {
        assert(false && "No free watch points available!");
    }

    WatchPoint* new_wp = free_;
    free_ = free_->next; // 从空闲链表中取出

    if (head == nullptr) {
        head = new_wp;
    } else {
        WatchPoint* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = new_wp;
    }

    new_wp->next = nullptr; // 新观察点的 next 设为 nullptr
    return new_wp;
}

void WatchPointManager::freeWatchPoint(WatchPoint* wp) 
{
    if (head == nullptr || wp == nullptr) {
        return;
    }
    // 如果要释放的是头节点
    if (head == wp) {
        head = head->next;
    } 
    else {
        WatchPoint* temp = head;
        WatchPoint* prev = nullptr;
        while (temp != nullptr && temp != wp) {
            prev = temp;
            temp = temp->next;
        }
        if (temp == wp) {
            prev->next = temp->next; // 从链表中移除
        }
    }

    // 清空观察点数据并加入空闲链表
    wp->enb = false;
    wp->expr.clear();
    wp->next = free_;
    free_ = wp;
}

bool WatchPointManager::freeWatchPointByNo(int w_no) 
{
    WatchPoint* temp = head;
    while (temp != nullptr) 
    {
        if (temp->NO == w_no) 
        {
            freeWatchPoint(temp);
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void WatchPointManager::printWatchPoints() const 
{
    std::cout << "\033[41mNum\tType\tEnb\tExpr\033[0m\n";
    WatchPoint* temp = head;
    while (temp != nullptr) 
    {
        std::cout << temp->NO << "\twp\t" 
                    << temp->enb << "\t" 
                    << temp->expr << "\n";
        temp = temp->next;
    }
}

// void WatchPointManager::run() 
// {
//     uint32_t val;
//     bool success;
    
//     while (true)
//     {
//         if(stop_flag)       // 停止线程
//         {
//             std::cout<<"stop wp\n";
//             break;
//         }
           
//         WatchPoint* temp = head;
//         while (temp != nullptr) 
//         {
//             if(monitor->sdb->expr->evaluate(temp->expr, success))
//             {
//                 std::cout << "\033[;41mNO:" << temp->NO << "|EXPR:" << temp->expr << "\033[0m\t";
//                 mNPC->stop_flag = true;
//                 stop_flag = true;
//                 break;
//             }
//             temp = temp->next;
//         }
         
//     }
    
// }