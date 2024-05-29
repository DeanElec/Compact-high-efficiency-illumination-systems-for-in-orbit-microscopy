#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QMMqttClient.h"
#include <QListWidgetItem>
#include <QListView>
#include <QProcess>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QMMqttClient client;


protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_RPI_Directory_Location_Line_Edit_returnPressed();

    void on_Take_Image_Push_Button_clicked();

    void on_Directory_PC_Line_Edit_returnPressed();

    void on_Image_Processing_listWidget_itemSelectionChanged();

    void on_Custom_Brightness_Line_Edit_returnPressed();

    void on_Custom_White_Balance_Line_Edit_returnPressed();

    void on_Custom_Gamma_Line_Edit_returnPressed();

    void on_Cyan_Brightness_horizontalSlider_valueChanged(int value);

    void on_Red_Brightness_horizontalSlider_valueChanged(int value);

    void on_Green_Brightness_horizontalSlider_valueChanged(int value);

    void on_Blue_Brightness_horizontalSlider_valueChanged(int value);

    void on_Blue_Brightness_horizontalSlider_sliderReleased();

    void on_Cyan_Brightness_horizontalSlider_sliderReleased();

    void on_Red_Brightness_horizontalSlider_sliderReleased();

    void on_Green_Brightness_horizontalSlider_sliderReleased();

    void on_PWM_Frequency_Line_Edit_returnPressed();

    void Linux_IP_Address();

    void on_Restart_Rpi_System_Service_Push_Button_pressed();

    void on_Open_Image_in_Napri_Push_Button_pressed();

    //void napariClosed();

    void on_PWM_Pins_Line_Edit_returnPressed();

private:
    Ui::Widget *ui;
    QSet<int> SelectedIndexesSet;
    //QProcess* process; // Declare the QProcess pointer here


};
#endif
