#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QMetaType>
#include "maindialog.h"
#include "ui_maindialog.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{  
    ui->setupUi(this);

    SaveIni = new QSettings (QDir::currentPath()+"/"+"Save.ini", QSettings :: IniFormat);

    //Read enable state form ini file
    SaveIni->beginGroup ("ENABLE_STATE");
    DownloadInfo.FPGA_Enable =SaveIni->value("FPGA").toBool();
    DownloadInfo.Interface_Enable = SaveIni->value("INTERFACE").toBool();
    DownloadInfo.ISP_Enable = SaveIni->value("ISP").toBool();
    DownloadInfo.Motor_Enable = SaveIni->value("MOTOR").toBool();
    DownloadInfo.Master_Enable = SaveIni->value("MASTER").toBool();
    SaveIni->endGroup();

    //set enable state
    on_FPGA_checkBox_stateChanged(DownloadInfo.FPGA_Enable);
    on_Interface_checkBox_stateChanged(DownloadInfo.Interface_Enable);
    on_ISP_checkBox_stateChanged(DownloadInfo.ISP_Enable);
    on_Motor_checkBox_stateChanged(DownloadInfo.Motor_Enable);
    on_Master_checkBox_stateChanged(DownloadInfo.Master_Enable);

    //Read file path
    SaveIni->beginGroup ("FILE_PATH");
    DownloadInfo.FPGA_String = SaveIni->value("FPGA").toString();
    DownloadInfo.Interface_String = SaveIni->value("INTERFACE").toString();
    DownloadInfo.ISP_String = SaveIni->value("ISP").toString();
    DownloadInfo.Motor_String = SaveIni->value("MOTOR").toString();
    DownloadInfo.Master_String = SaveIni->value("MASTER").toString();
    SaveIni->endGroup();

    ui->FPGA_lineEdit->insert(DownloadInfo.FPGA_String);
    ui->Interface_lineEdit->insert(DownloadInfo.Interface_String);
    ui->ISP_lineEdit->insert(DownloadInfo.ISP_String);
    ui->Motor_lineEdit->insert(DownloadInfo.Motor_String);
    ui->Master_lineEdit->insert(DownloadInfo.Master_String);


    //Comport
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->ComPort_comboBox->insertItem(256,info.portName());
    }
    ui->ComPort_comboBox->setCurrentIndex(0);

    //BaudRate    
    ui->BaudRate_comboBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->BaudRate_comboBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->BaudRate_comboBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    //Baudrate
    SaveIni->beginGroup ("BAUDRATE");
    ui->BaudRate_comboBox->setCurrentIndex(SaveIni->value("Baudrate").toInt());
    SaveIni->endGroup();

    //=====================//
    //    Set connection   //
    //=====================//
    connect(&Masterthread,SIGNAL(ProcessFinishSignals()),this,SLOT(ProcessFinishSolt()));
    connect(&Masterthread,SIGNAL(PercentageSignals(char,int)),this,SLOT(ShowPercentage(char,int)));
    connect(&Masterthread.Master,SIGNAL(PercentageSignals(char,int)),this,SLOT(ShowPercentage(char,int)));
    connect(&Masterthread.Motor,SIGNAL(PercentageSignals(char,int)),this,SLOT(ShowPercentage(char,int)));
}

MainDialog::~MainDialog()
{
    //Baudrate
    SaveIni->beginGroup ("BAUDRATE");
    SaveIni->setValue("Baudrate",ui->BaudRate_comboBox->currentIndex());
    SaveIni->endGroup();

    //Enable state
    DownloadInfo.FPGA_Enable = ui->FPGA_checkBox->checkState();
    DownloadInfo.Interface_Enable = ui->Interface_checkBox->checkState();
    DownloadInfo.ISP_Enable = ui->ISP_checkBox->checkState();
    DownloadInfo.Motor_Enable = ui->Motor_checkBox->checkState();
    DownloadInfo.Master_Enable = ui->Master_checkBox->checkState();

    SaveIni->beginGroup ("ENABLE_STATE");
    SaveIni->setValue("FPGA",DownloadInfo.FPGA_Enable);
    SaveIni->setValue("INTERFACE",DownloadInfo.Interface_Enable);
    SaveIni->setValue("ISP",DownloadInfo.ISP_Enable);
    SaveIni->setValue("MOTOR",DownloadInfo.Motor_Enable);
    SaveIni->setValue("MASTER",DownloadInfo.Master_Enable);
    SaveIni->endGroup();

    //File path
    SaveIni->beginGroup ("FILE_PATH");
    if(DownloadInfo.FPGA_Enable)
    {
        SaveIni->setValue("FPGA",DownloadInfo.FPGA_String);
    }
    if(DownloadInfo.Interface_Enable)
    {
        SaveIni->setValue("INTERFACE",DownloadInfo.Interface_String);
    }
    if(DownloadInfo.ISP_Enable)
    {
        SaveIni->setValue("ISP",DownloadInfo.ISP_String);
    }
    if(DownloadInfo.Motor_Enable)
    {
        SaveIni->setValue("MOTOR",DownloadInfo.Motor_String);
    }
    if(DownloadInfo.Master_Enable)
    {
        SaveIni->setValue("MASTER",DownloadInfo.Master_String);
    }
    SaveIni->endGroup();
    SaveIni->sync();
    delete ui;
}

void MainDialog::on_FPGA_pushButton_clicked()
{
    QFileInfo file_info(ui->FPGA_lineEdit->displayText());
    QDir tmp_dir(file_info.dir());
    QString FPGA_Path = QFileDialog::getOpenFileName(this, tr("Select file"),tmp_dir.absolutePath(), tr("Binary Files(*.rpd)"));
    if(false == FPGA_Path.isEmpty())
    {
        ui->FPGA_lineEdit->clear();
        ui->FPGA_lineEdit->insert(FPGA_Path);
        DownloadInfo.FPGA_String = ui->FPGA_lineEdit->text();
    }
}

void MainDialog::on_Interface_pushButton_clicked()
{
    QFileInfo file_info(ui->Interface_lineEdit->displayText());
    QDir tmp_dir(file_info.dir());
    QString Interface_Path = QFileDialog::getOpenFileName(this, tr("Select File"), tmp_dir.absolutePath(), tr("Binary Files(*.bin)"));
    if(false == Interface_Path.isEmpty())
    {
        ui->Interface_lineEdit->clear();
        ui->Interface_lineEdit->insert(Interface_Path);
        DownloadInfo.Interface_String = ui->Interface_lineEdit->text();
    }
}

void MainDialog::on_ISP_pushButton_clicked()
{
    QFileInfo file_info(ui->ISP_lineEdit->displayText());
    QDir tmp_dir(file_info.dir());
    QString ISP_Path = QFileDialog::getOpenFileName(this, tr("Select File"), tmp_dir.absolutePath(), tr("Binary Files(*.bin)"));
    if(false == ISP_Path.isEmpty())
    {
        ui->ISP_lineEdit->clear();
        ui->ISP_lineEdit->insert(ISP_Path);
        DownloadInfo.ISP_String = ui->ISP_lineEdit->text();
    }
}

void MainDialog::on_Motor_pushButton_clicked()
{
    QFileInfo file_info(ui->Motor_lineEdit->displayText());
    QDir tmp_dir(file_info.dir());
    QString Motor_Path = QFileDialog::getOpenFileName(this, tr("Select File"), tmp_dir.absolutePath(), tr("Binary Files(*.S19)"));
    if(false == Motor_Path.isEmpty())
    {
        ui->Motor_lineEdit->clear();
        ui->Motor_lineEdit->insert(Motor_Path);
        DownloadInfo.Motor_String = ui->Motor_lineEdit->text();
    }
}

void MainDialog::on_Master_pushButton_clicked()
{
    QFileInfo file_info(ui->Master_lineEdit->displayText());
    QDir tmp_dir(file_info.dir());
    QString Master_Path = QFileDialog::getOpenFileName(this, tr("Select File"), tmp_dir.absolutePath(), tr("Binary Files(*.S19)"));
    if(false == Master_Path.isEmpty())
    {
        ui->Master_lineEdit->clear();
        ui->Master_lineEdit->insert(Master_Path);
        DownloadInfo.Master_String = ui->Master_lineEdit->text();
    }
}

void MainDialog::on_FPGA_checkBox_stateChanged(int arg1)
{
    if(0 == arg1) //unchecked
    {
        ui->FPGA_lineEdit->setEnabled(false);
        ui->FPGA_pushButton->setEnabled(false);        
    }
    else
    {
        ui->FPGA_lineEdit->setEnabled(true);
        ui->FPGA_pushButton->setEnabled(true);
    }    
    ui->FPGA_checkBox->setChecked((bool)arg1);
    DownloadInfo.FPGA_Enable = (bool)arg1;
}

void MainDialog::on_Interface_checkBox_stateChanged(int arg1)
{
    if(0 == arg1) //unchecked
    {
        ui->Interface_lineEdit->setEnabled(false);
        ui->Interface_pushButton->setEnabled(false);
    }
    else
    {
        ui->Interface_lineEdit->setEnabled(true);
        ui->Interface_pushButton->setEnabled(true);
    }    
    ui->Interface_checkBox->setChecked((bool)arg1);
    DownloadInfo.Interface_Enable = (bool)arg1;
}

void MainDialog::on_ISP_checkBox_stateChanged(int arg1)
{
    if(0 == arg1) //unchecked
    {
        ui->ISP_lineEdit->setEnabled(false);
        ui->ISP_pushButton->setEnabled(false);
    }
    else
    {
        ui->ISP_lineEdit->setEnabled(true);
        ui->ISP_pushButton->setEnabled(true);
    }
    ui->ISP_checkBox->setChecked((bool)arg1);
    DownloadInfo.ISP_Enable = (bool)arg1;
}

void MainDialog::on_Motor_checkBox_stateChanged(int arg1)
{
    if(0 == arg1) //unchecked
    {
        ui->Motor_lineEdit->setEnabled(false);
        ui->Motor_pushButton->setEnabled(false);
    }
    else
    {
        ui->Motor_lineEdit->setEnabled(true);
        ui->Motor_pushButton->setEnabled(true);
    }
    ui->Motor_checkBox->setChecked((bool)arg1);
    DownloadInfo.Motor_Enable = (bool)arg1;
}

void MainDialog::on_Master_checkBox_stateChanged(int arg1)
{
    if(0 == arg1) //unchecked
    {
        ui->Master_lineEdit->setEnabled(false);
        ui->Master_pushButton->setEnabled(false);
    }
    else
    {
        ui->Master_lineEdit->setEnabled(true);
        ui->Master_pushButton->setEnabled(true);
    }
    ui->Master_checkBox->setChecked((bool)arg1);
    DownloadInfo.Master_Enable = (bool)arg1;
}

void MainDialog::on_Process_pushButton_clicked()
{
    //===============================//
    // Disable UI contorl panel      //
    //= =============================//
    ui->FPGA_pushButton->setEnabled(false);
    ui->Interface_pushButton->setEnabled(false);
    ui->Motor_pushButton->setEnabled(false);
    ui->ISP_pushButton->setEnabled(false);
    ui->Master_pushButton->setEnabled(false);
    ui->BaudRate_comboBox->setEnabled(false);
    ui->ComPort_comboBox->setEnabled(false);
    ui->Process_pushButton->setEnabled(false);


    //===============================//
    // Send  info to Master Thread   //
    //= =============================//
    DownloadInfo.FPGA_String = ui->FPGA_lineEdit->text();
    DownloadInfo.Interface_String = ui->Interface_lineEdit->text();
    DownloadInfo.ISP_String = ui->ISP_lineEdit->text();
    DownloadInfo.Motor_String = ui->Motor_lineEdit->text();
    DownloadInfo.Master_String = ui->Master_lineEdit->text();
    Masterthread.setParameterAndRun(ui->ComPort_comboBox->currentText(),ui->BaudRate_comboBox->itemText(ui->BaudRate_comboBox->currentIndex()).toInt(),&DownloadInfo);
}

void MainDialog::ProcessFinishSolt()
{
    Masterthread.wait();
    //===============================//
    // Enable UI contorl panel      //
    //= =============================//
    ui->FPGA_pushButton->setEnabled(true);
    ui->Interface_pushButton->setEnabled(true);
    ui->Motor_pushButton->setEnabled(true);
    ui->ISP_pushButton->setEnabled(true);
    ui->Master_pushButton->setEnabled(true);
    ui->BaudRate_comboBox->setEnabled(true);
    ui->ComPort_comboBox->setEnabled(true);
    ui->Process_pushButton->setEnabled(true);
}

void MainDialog::ShowPercentage(char type,int value)
{
    static int totalSize,curSize;
    int percent = 0;
    QString Precent;

    switch(type)
    {
        case RUN_BEGIN:
        {
           totalSize = value;
           curSize = 0;
        }
        break;
        case RUN_START:
        {
            curSize+=value;
        }
        break;
        case RUN_STOP:
        {
            curSize = totalSize;
        }
        break;
    }

    if(totalSize >0)
    {
        percent = curSize*100/totalSize;
    }
    Precent = QString::number(percent)+"%";
    ui->Processing_label->setText(Precent);
}

