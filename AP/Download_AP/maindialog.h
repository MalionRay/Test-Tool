#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QSettings>
#include <QDialog>
#include <QFileDialog>
#include <QtSerialPort/QSerialPort>
#include <QtCore/QThread>
#include "datatype.h"
#include "masterthread.h"

QT_BEGIN_NAMESPACE
class QComboBox;
QT_END_NAMESPACE

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

private slots:
    void on_FPGA_pushButton_clicked();
    void on_Interface_pushButton_clicked();
    void on_ISP_pushButton_clicked();
    void on_Motor_pushButton_clicked();
    void on_Master_pushButton_clicked();
    void on_Process_pushButton_clicked();
    void on_FPGA_checkBox_stateChanged(int arg1);
    void on_Interface_checkBox_stateChanged(int arg1);
    void on_ISP_checkBox_stateChanged(int arg1);
    void on_Motor_checkBox_stateChanged(int arg1);
    void on_Master_checkBox_stateChanged(int arg1);

public slots:
    void ProcessFinishSolt();
    void ShowPercentage(char type,int size);

private:
    Ui::MainDialog *ui;
    MasterThread Masterthread;
    sDownloaadInfo DownloadInfo;
    QSettings * SaveIni;
};

#endif // MAINDIALOG_H
