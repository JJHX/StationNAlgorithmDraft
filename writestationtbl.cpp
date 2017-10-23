#include "writestationtbl.h"
#include <QSettings>
#include "ui_writestationtbl.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <qfileinfo.h>
WriteStationTBL::WriteStationTBL(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WriteStationTBL)
{

    ui->setupUi(this);
//    isStationFileFound=findStationKey();
    isConfigFileFound=findConfigfile();
    ShowCurrentConfig();
}

WriteStationTBL::~WriteStationTBL()
{
    delete ui;
}

bool WriteStationTBL::findConfigfile()
{

    //load ip tbl
    QFile *m_IPFile = new QFile("./IP.tbl");
    if (!m_IPFile->open(QIODevice::ReadOnly))
    {
//        ui->textBrowser_IPFilePath->setText("Not found");
//        ui->textBrowser_IPFilePath->setStyleSheet("color:red");
        return false;
    }
    else
    {
//        ui->textBrowser_IPFilePath->setText(QDir::currentPath()+"/IP.tbl");
//        ui->textBrowser_IPFilePath->setStyleSheet("color:green");
        //read config file choices for setting part
        QTextStream in(m_IPFile);
        while (!in.atEnd())
        {
            //read a line each time and record in qmap
            QString line = in.readLine();
            if (line.length() > 0 && line.at(0) != '#' && line.at(0) != ' ')
            {
                QStringList fields = line.split(QRegExp("\\s"),QString::SkipEmptyParts);
                while (fields.length()<5)//wipe ass
                    fields.append("---");
                if (!mList_carId.contains(fields.at(0)))
                {
                    mList_carId.append(fields.at(0));
                    m_map.insert(fields.at(0),fields.at(1));
                    if (fields.length()>2)
                        for ( int index =2 ; index < fields.length(); ++index)
                            m_map.insertMulti(fields.at(0),fields.at(index));
                }
                else
                {
                    for ( int index =1 ; index < fields.length(); ++index)
                        m_map.insertMulti(fields.at(0),fields.at(index));
                }
//                qDebug()<<fields;
            }
        }
        QString item;
        QStringList tmpStrLst;
        std::sort(mList_carId.begin(),mList_carId.end());
        foreach(item,mList_carId)
        {
            if (tmpStrLst.isEmpty())
                tmpStrLst.append(item);
            else if (tmpStrLst.last() != item)
                tmpStrLst.append(item);
        }
        QList<QString>::iterator itr = tmpStrLst.begin();
        for (;itr!=tmpStrLst.end();++itr)
            ui->comboBox_CarID->addItem(*itr);

//        qDebug()<<m_map;
//        qDebug()<<m_map.values("1");
    }
    m_IPFile->close();
    return true;
}


void WriteStationTBL::on_comboBox_CarID_currentIndexChanged(const QString &arg1)
{
   ui->comboBox_Terminal->clear();
   ui->comboBox_Terminal->addItem(m_map.values(arg1).at(9));
   ui->comboBox_Terminal->addItem(m_map.values(arg1).at(4));
}

void WriteStationTBL::on_comboBox_Terminal_currentIndexChanged(const QString &arg1)
{
    QList<QString> str = m_map.values(ui->comboBox_CarID->currentText());
    if (arg1 == "A")
    {
        ui->label_IP_eth0->setText("Eth0: " + str.at(7));
        ui->label_IP_eth1->setText("Eth1: " + str.at(6));
        ui->label_TodID->setText("TodID: " + str.at(8));
        ui->label_Subnet->setText("Subnet: " + str.at(5));
    }
    else
    {
        ui->label_IP_eth0->setText("Eth0: " + str.at(1));
        ui->label_IP_eth1->setText("Eth1: " + str.at(2));
        ui->label_TodID->setText("TodID: " + str.at(3));
        ui->label_Subnet->setText("Subnet: " + str.at(0));
    }
}

void WriteStationTBL::on_pushButton_clicked()
{
    //!save current setting to Todconfig and update
    QSettings setting("tod.conf",QSettings::IniFormat);
    setting.beginGroup("Communication");
    setting.setValue("TodIface0Address",ui->label_IP_eth0->text().remove(0,6));
    setting.setValue("TodIface1Address",ui->label_IP_eth1->text().remove(0,6));
    setting.setValue("TodNetmask",ui->label_currentSubnet->text().remove(0,9));
    setting.setValue("TodPort",ui->label_currentTodPort->text().remove(0,9));
    setting.setValue("TodId",ui->label_currentTodID->text().remove(0,7));
    ShowCurrentConfig();
}

void WriteStationTBL::ShowCurrentConfig()
{
    //!load tod.conf and display orignal settings
//    QSettings m_TodSetting("/mnt/TOD/tod.conf", QSettings::IniFormat);
    QSettings m_TodSetting("tod.conf", QSettings::IniFormat);
    m_TodSetting.beginGroup("Communication");
    ui->label_currentEth0->setText("Eth0: " + m_TodSetting.value("TodIface0Address").toString());
    ui->label_currentEth1->setText("Eth1: " + m_TodSetting.value("TodIface1Address").toString());
    ui->label_currentSubnet->setText("NetMask: " + m_TodSetting.value("TodNetmask").toString());
    ui->label_currentTodID->setText("TodID: " + m_TodSetting.value("TodId").toString());
    ui->label_currentTodPort->setText("TodPort: " + m_TodSetting.value("TodPort").toString());
}

void WriteStationTBL::on_pushButton_CarIDUp_pressed()
{
    int index = ui->comboBox_CarID->currentIndex();
    if (++index <= ui->comboBox_CarID->count()-1)
        ui->comboBox_CarID->setCurrentIndex(index);
}

void WriteStationTBL::on_pushButton_CarIDDown_clicked()
{
    int index = ui->comboBox_CarID->currentIndex();
    if (--index >= 0)
        ui->comboBox_CarID->setCurrentIndex(index);
}

void WriteStationTBL::on_pushButton_CarTermUp_clicked()
{
    int index = ui->comboBox_CarID->currentIndex();
    if (++index <= ui->comboBox_Terminal->count())
        ui->comboBox_Terminal->setCurrentIndex(index);
}

void WriteStationTBL::on_pushButton_CarTermDown_clicked()
{
    int index = ui->comboBox_Terminal->currentIndex();
    if (--index >= 0)
        ui->comboBox_Terminal->setCurrentIndex(index);
}

void WriteStationTBL::on_pushButton_Update_clicked()
{
    //! Read project name
    //! Goto update folder which contains prgs
    //! Take partical name to rename the new folder on display
    //! EXP: "update_NCL1", the new folder on display would be named as "NCL1"
    QString testPath("./test");
    QString usbPath("/mnt/TOD");
//    QString destPath(targetPath+projectName);

    QDir updatePath(testPath);
    if (updatePath.exists())
    {
        updatePath.setFilter(QDir::AllDirs);

        //Identify the update folder
        foreach (QString str, updatePath.entryList())
        {
            if (str.contains("update"))
            {
                QStringList strList = str.split("_");
                projectName = strList.at(1);
                qDebug()<<"projectName"<<projectName;
            }
        }

        //create new folder and name it as project
        if (!QFile::exists("./"+projectName))
            QDir().mkdir(projectName);
        else
        {
        }

        //copy files to target
//        updatePath.setFilter(QDir::Files);//set filter to hide folders
        updatePath.setFilter(QDir::AllEntries);//set filter to hide folders
        QStringList fldContentList = updatePath.entryList();
//        qDebug()<<"The following content are in the folder:"<<fldContentList;
        foreach (QString str, fldContentList)
        {
            //skip the content start with "." ".." and the one end with "~"
            if (str.contains(".") || str.contains("..") || str.contains("~"))
                continue;
//            qDebug()<<"following output shall not include . .. or ~"<< str;

            //deal with directory
            QDir isStrDir("./test/"+str);
//            qDebug()<<"is"<<str <<"a directory?"<< isStrDir.exists();

            //copy each file to destination
            QFile destinationFile, sourceFile;
            QDir destination("./" + projectName);
            QString fileName(str);
            if (!isStrDir.exists())
            {

                sourceFile.setFileName("./test/"+str);
                destinationFile.setFileName("./"+projectName+"/"+str);
                sourceFile.copy(destinationFile.fileName());
            }

            else
            {
                QDir subFolder("./test/"+str+"/");
                subFolder.setFilter(QDir::AllEntries);
                QStringList subFldContentList = subFolder.entryList();
//                qDebug()<<"subfolder content:"<<subFldContentList;
                foreach (QString subStr, subFldContentList)
                {
                    if (str.contains(".") || str.contains("..") || str.contains("~"))
                        continue;
                    sourceFile.setFileName("./test/"+str+"/"+subStr);
                    destinationFile.setFileName("./"+projectName+"/"+subStr);
                    sourceFile.copy(destinationFile.fileName());
                }
            }


        }
    }

//    else
//    {
//        ui->pushButton_Update->setText("Lack of files");
//    }

}

bool WriteStationTBL::removeDir(const QString & directory)
{
    bool result = true;
    QDir m_dir(directory);
    if (m_dir.exists())
    {
        foreach(QFileInfo info,m_dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Files )    )
        {
            if (info.isDir())
                result = removeDir(info.absoluteFilePath());
            else
                result = QFile::remove(info.absoluteFilePath());
        }
    }

    return result;
}

void WriteStationTBL::on_pushButton_2_clicked()
{
    removeDir("./"+projectName);

}
