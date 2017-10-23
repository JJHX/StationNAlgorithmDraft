#ifndef WRITESTATIONTBL_H
#define WRITESTATIONTBL_H

#include <QDialog>
#include <QList>
#include <QMap>
#include <QString>

namespace Ui {
class WriteStationTBL;
}

class WriteStationTBL : public QDialog
{
    Q_OBJECT

public:
    explicit WriteStationTBL(QWidget *parent = 0);
    ~WriteStationTBL();

    bool findConfigfile();
private slots:

    void on_comboBox_CarID_currentIndexChanged(const QString &arg1);

    void on_comboBox_Terminal_currentIndexChanged(const QString &arg1);

    void on_pushButton_clicked();

    void ShowCurrentConfig();

    void on_pushButton_CarIDUp_pressed();

    void on_pushButton_CarIDDown_clicked();

    void on_pushButton_CarTermUp_clicked();

    void on_pushButton_CarTermDown_clicked();

    void on_pushButton_Update_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::WriteStationTBL *ui;
//    bool findStationKey();//!This method will find stationKey.tbl in current folder and display it path on UI
    bool findIPfile();
    bool removeDir(const QString &directory );
    bool isStationFileFound;
    bool isConfigFileFound;
    QList<QString> mList_carId,mList_Terminal,mList_subnetMask;
    QMap<QString,QString> m_map;//pair id with related info

    QString projectName;

};

#endif // WRITESTATIONTBL_H
