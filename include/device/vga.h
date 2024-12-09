#ifndef __NPCVGA_H__
#define __NPCVGA_H__

#include <QThread>
#include <cstdint>
#include<QImageReader>

class NPCVGA:public QThread
{
    Q_OBJECT
public:
    NPCVGA(uint32_t w = 400, uint32_t h = 300, QObject* parent = nullptr);
    ~NPCVGA();
    static uint32_t sync_signal;
    static uint32_t screen_width;
    static uint32_t screen_height;
    static uint32_t *vmem;
    bool stop_flag;
    void stop();
    
protected:
    void run() override;

private:
    void convertToRGB(const uint32_t* src, uint8_t* dest, uint32_t pixelCount);

signals:
    void sendVGAInfoToUI(const QImage &image);

};

#endif
