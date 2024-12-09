#ifndef __MAIN_H__
#define __MAIN_H__

#include "mainwindow.h"
#include "npc.h"
#include "monitor.h"

class MainWindow;
class NPC;
class Monitor;

extern MainWindow *ui;
extern NPC *mNPC;
extern Monitor *monitor;
extern int mode_sel;

enum DisplayMode {
    DISPLAY_IDE_ONLY = 1,       // 1: display IDE, but don't SDB
    DISPLAY_SDB_ONLY = 2,       // 2: don't display IDE, but display SDB
    DISPLAY_BOTH     = 3,       // 3: display IDE and SDB
    BATCH_MODE       = 4        // 4: batch mode
};

#endif