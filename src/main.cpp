#include <QApplication>
#include <iostream>

#include "main.h"
#include "vmem.h"
#include "reg.h"

MainWindow  *ui = nullptr;
NPC *mNPC = nullptr;
Monitor *monitor = nullptr;

int           mode_sel;
std::string   log_file;
std::string   elf_file;
std::string   diff_so_file;
std::string   img_file;

void parseArgs(int argc, char* argv[]) {
  const struct option table[] = {
	{"mode",    required_argument, nullptr, 'm'},
	{"log",     required_argument, nullptr, 'l'},
	{"diff",    required_argument, nullptr, 'd'},
	{"ftrace",  required_argument, nullptr, 'f'},
	{"image",   required_argument, nullptr, 'i'},
	{"help",    no_argument,       nullptr, 'h'},
	{nullptr,   0,                 nullptr,  0},
  };
  int opt;
  while ((opt = getopt_long(argc, argv, "-hl:d:f:m:i:", table, nullptr)) != -1) {
    switch (opt) {
      case 'l': log_file        = optarg;             break;
      case 'd': diff_so_file    = optarg;             break;
      case 'f': elf_file        = optarg;             break;
      case 'i': img_file        = optarg;             break;
      case 'm': mode_sel        = std::stoi(optarg);  break;
      default:
        std::printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        std::printf("\t-l,--log=FILE           output log to FILE\n");
        std::printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        std::printf("\t-f,--ftrace=FILE        output function trace log to FILE\n");
        std::printf("\t-i,--image=image_file   location of iamge file\n");
        std::printf("\t-m,--mode=NUM           1:display IDE, but don't SDB;\n\
                                2: don't display IDE, but display SDB; \n\
                                3: display IDE and SDB \n\
								4: batch mode \n");
        std::printf("\n");
        std::exit(0);
    }
  }
}

int main(int argc, char *argv[])
{
	parseArgs(argc, argv);

	mNPC = new NPC(argc, argv);     
	mNPC->init_npc();    
	
	// std::cout << "is_batch_mode=" << is_batch_mode << std::endl;
	std::cout << "mode_sel=" << mode_sel << std::endl;
	// std::cout << "log_file=" << log_file << std::endl;
	// std::cout << "elf_file=" << elf_file << std::endl;
	// std::cout << "diff_so_file" << diff_so_file << std::endl;
	// std::cout << "img_file=" << img_file << std::endl;

	monitor = new Monitor(log_file, elf_file, diff_so_file, img_file);
	monitor->init_monitor(argc, argv);

	if(mode_sel == DISPLAY_SDB_ONLY || mode_sel == DISPLAY_BOTH){
		monitor->start();
		mNPC->debug_flag = true;
		if(mode_sel == DISPLAY_SDB_ONLY)
			while(!monitor->stop_flag);
	}

	if(mode_sel == BATCH_MODE)   // batch mode
	{
		mNPC->exec(-1);
	}

	if(mode_sel == DISPLAY_IDE_ONLY || mode_sel == DISPLAY_BOTH){
		QApplication a(argc, argv);
		ui = new MainWindow(argc, argv);
		ui->init((unsigned int *)vmem->mem_addr, monitor->inst_num);

		QObject::connect(mNPC->npc_vga, SIGNAL(sendVGAInfoToUI(const QImage &)), ui, SLOT(receiveImageFromNPC(const QImage &)));
		QObject::connect(mNPC, &NPC::sendRegInfo, ui->mSplitterLayout->gprWidget, &GPRMonitor::receiveGPRValue);
		QObject::connect(mNPC, &NPC::sendNPCInfoToUI, ui->mSplitterLayout->instMemWidget, &InstMonitor::receiveNPCInfo);
		QObject::connect(mNPC, &NPC::destroyWindow, ui, &MainWindow::onDestroyWindow);
		QObject::connect(monitor, &Monitor::destroyWindow, ui, &MainWindow::onDestroyWindow);
		QObject::connect(mNPC, &NPC::sendDiffError, ui->mSplitterLayout->instMemWidget, &InstMonitor::receiveDiffError);
		ui->show();
		a.exec();
	}

	delete vmem;
	delete riscv32_reg;
	delete mNPC;
	delete monitor;
	exit(0);

	return 0;
}

