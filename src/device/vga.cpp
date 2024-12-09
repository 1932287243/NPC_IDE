#include "vga.h"
#include <iostream>

uint32_t NPCVGA::sync_signal = 0;
uint32_t NPCVGA::screen_width = 400;
uint32_t NPCVGA::screen_height = 300;
uint32_t*  NPCVGA::vmem = nullptr;

NPCVGA::NPCVGA(uint32_t w, uint32_t h, QObject* parent)
    :QThread(parent)
{
    screen_width = w;
    screen_height = h;
    vmem = new uint32_t[screen_width*screen_height];
    stop_flag = false;
    // memset(NPCVGA::vmem, 220, sizeof(int)*screen_width*screen_height);
}

NPCVGA::~NPCVGA()
{
    std::cout << "VGA" << std::endl;
    delete []vmem;
}


void NPCVGA::convertToRGB(const uint32_t* src, uint8_t* dest, uint32_t pixelCount) 
{
    for (uint32_t i = 0; i < pixelCount; ++i) {
        // 从源数组中获取每个像素的 4 字节 ARGB (0xAARRGGBB)
        uint32_t pixel = src[i];
        
        // 将其转换为 3 字节 RGB (0xRRGGBB)
        uint8_t red   = (pixel >> 16) & 0xFF;  // 提取红色通道
        uint8_t green = (pixel >> 8) & 0xFF;   // 提取绿色通道
        uint8_t blue  = pixel & 0xFF;          // 提取蓝色通道
        
        // 3 字节存储在目标内存中
        dest[i * 3]     = red;
        dest[i * 3 + 1] = green;
        dest[i * 3 + 2] = blue;
    }
}

void NPCVGA::stop() {
    stop_flag = true;  // 设置停止标志
}

void NPCVGA::run() 
{
    while(1)
    {
        if(stop_flag)
            break;
        if(sync_signal != 0)
        {
            // 为目标数据分配内存 (每个像素 3 字节)
            uint8_t* dest = new uint8_t[screen_width*screen_height*3];
            // 调用函数转换 ARGB -> RGB
            convertToRGB(vmem, dest, screen_width*screen_height);
            QImage image(dest, screen_width, screen_height, QImage::Format_RGB888);
            //发送图片到前端
            sync_signal = 0;
            emit sendVGAInfoToUI(image);
            // 释放目标内存
            delete[] dest;
        }
    }
   
}

