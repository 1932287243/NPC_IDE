#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QSettings"
#include <QListWidget>
#include <QImageReader>

#include "filesystemmodel.h"
#include "codeeditor.h"
#include "splitterlayout.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( int argc, char **argv, QWidget *parent = nullptr);
    ~MainWindow();

    SplitterLayout *mSplitterLayout ;
    
    void saveSuccessAction( CodeEditor * codeEditor);
    void addFolder(QString filePath);
    void init(unsigned int * vmem_addr, int inst_num);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void onDestroyWindow();

private slots:
    void on_new_file_triggered();

    void on_open_dir_triggered();

    void on_save_file_triggered();

    void on_save_as_triggered();

    void on_paste_triggered();

    void on_cut_triggered();

    void on_copy_triggered();

    void on_font_triggered();

    void on_about_triggered();

    void on_undo_triggered();

    void on_redo_triggered();

    void on_exit_triggered();

    void on_print_triggered();

    void on_clear_history_triggered();

    void onFinishCreateTab(const QString &filePath);

    void onItemClicked(QListWidgetItem *item);

    void receiveImageFromNPC(const QImage &image);

    void exec();

    void execOne();

    void isDebugMode(bool checked);
    
private:
    Ui::MainWindow *ui;

    QSettings * mSettings;
    QString mFontFamily;
    int mFontSize;

    QString mDir="E:/";

    void initMenu();
    void initFont();
    void initAction(int tabCount);

    void on_open_rencent_file();
    void saveHistory(QString path);
    QList<QString> getHistory();

    void createTab(QString fileName);

    void setTreeViewModel();

    
};
#endif // MAINWINDOW_H
