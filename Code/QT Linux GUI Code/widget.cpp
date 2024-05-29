#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <iostream>

#include <QLabel>
#include <QListView>
#include <QMessageBox>


#include "IPAddress.h"
#include <string>
#include <cstring>

#include <mosquitto.h>

#include <string.h>
#include <thread>
#include "Socket_Comms.h"
#include <QTimer>
#include <filesystem>
#include "File_Monitor.h"

#include "Custom_Slider.h"
//-----Need for Qt Wrapper for mosquitto-----
#include <QCoreApplication>
#include <QTime>
#include "QMMqttClient.h"
//-------------------------------------------


#include <QModelIndexList>
#include <QAbstractItemModel>
#include <QFileDialog>
#include <QProcess>
#include <QThread>


#include <QDebug>
#include <QDir>

#include <QtConcurrent/QtConcurrentRun>

#include <QIcon>

using std::cout;
using std::endl;
using std::string;

//namespace Socket_Comms_Parameters {
const char* RaspberryPi_Retrived_IP_Address;
//}

namespace PWM_Parameters{ //Brightness for all colours, PWM Pins, Frequency
int Cyan_LED_Brightness;
int Red_LED_Brightness;
int Green_LED_Brightness;
int Blue_LED_Brightness;

int PWM_Frequency;

int Cyan_LED_PWM_Pin;
int Red_LED_PWM_Pin;
int Green_LED_PWM_Pin;
int Blue_LED_PWM_Pin;
}


namespace Advanced_Image_Processing_Options_Parameters{
QString Custom_Gamma_Value;
QString Custom_Brightness_Value;
QString Custom_White_Balance_Value;
}

namespace Error_Checking_Parameters{
bool Automatic_Brightness_Flag;
bool White_Balance_Flag;
}

namespace Directory_Parameters{
QString Linux_PC_Save_Directory;
QString Confirmed_Correct_Linux_PC_Save_Directory;
QString PC_Save_Directory;
}

File_Monitor* file_Monitor = nullptr;
std::thread monitor_files_thread;

void runMonitoringThread() {
    file_Monitor = new File_Monitor(Directory_Parameters::Linux_PC_Save_Directory.toStdString());
    file_Monitor->Monitor_Directory();
}


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->Currently_Set_PC_Directory_Location_Line_Edit->setReadOnly(true);
    ui->Currently_Set_RPI_Directory_Location_Line_Edit->setReadOnly(true);


    //--------------------Init for the LED brightness sliders--------------------
    ui->Cyan_Brightness_horizontalSlider->setMinimum(0);
    ui->Cyan_Brightness_horizontalSlider->setMaximum(100);
    connect(ui->Cyan_Brightness_horizontalSlider, &QSlider::valueChanged, this, &Widget::on_Cyan_Brightness_horizontalSlider_valueChanged);

    ui->Red_Brightness_horizontalSlider->setMinimum(0);
    ui->Red_Brightness_horizontalSlider->setMaximum(100);
    connect(ui->Red_Brightness_horizontalSlider, &QSlider::valueChanged, this, &Widget::on_Red_Brightness_horizontalSlider_valueChanged);

    ui->Green_Brightness_horizontalSlider->setMinimum(0);
    ui->Green_Brightness_horizontalSlider->setMaximum(100);
    connect(ui->Green_Brightness_horizontalSlider, &QSlider::valueChanged, this, &Widget::on_Green_Brightness_horizontalSlider_valueChanged);

    ui->Blue_Brightness_horizontalSlider->setMinimum(0);
    ui->Blue_Brightness_horizontalSlider->setMaximum(100);
    connect(ui->Blue_Brightness_horizontalSlider, &QSlider::valueChanged, this, &Widget::on_Blue_Brightness_horizontalSlider_valueChanged);
    //--------------------------------------------------------------------------

    ui->Image_Processing_listWidget->setSelectionMode(QAbstractItemView::MultiSelection); //allow the list view to have multiple selections

    ui->frame->setStyleSheet("QFrame {border: 2px solid white; }");

    ui->label_11->setStyleSheet("border: none;");
    ui->label_12->setStyleSheet("border: none;");
    ui->label_13->setStyleSheet("border: none;");
    ui->label_14->setStyleSheet("border: none;");

    ui->label_15->setStyleSheet("border: none;");
    ui->label_16->setStyleSheet("border: none;");
    ui->label_17->setStyleSheet("border: none;");
    ui->label_18->setStyleSheet("border: none;");
    ui->label_19->setStyleSheet("border: none;");
    ui->label_20->setStyleSheet("border: none;");
    ui->label_21->setStyleSheet("border: none;");
    ui->label_22->setStyleSheet("border: none;");
    ui->label_23->setStyleSheet("border: none;");
    ui->label_24->setStyleSheet("border: none;");
    ui->label_25->setStyleSheet("border: none;");
    ui->label_26->setStyleSheet("border: none;");
    ui->label_27->setStyleSheet("border: none;");
    ui->label_29->setStyleSheet("border: none;");
    ui->label_30->setStyleSheet("border: none;");
    ui->Cyan_LED_Value_from_Slider_label->setStyleSheet("border: none;");
    ui->Red_LED_Value_from_Slider_label->setStyleSheet("border: none;");
    ui->Green_LED_Value_from_Slider_label->setStyleSheet("border: none;");
    ui->Blue_LED_Value_from_Slider_label->setStyleSheet("border: none;");

    ui->Cyan_Brightness_horizontalSlider->setEnabled(false);
    ui->Red_Brightness_horizontalSlider->setEnabled(false);
    ui->Green_Brightness_horizontalSlider->setEnabled(false);
    ui->Blue_Brightness_horizontalSlider->setEnabled(false);



}


// void Widget::MQTT_Init(QString IP_Address){ //initialise the MQTT
//     qDebug() << IP_Address;
//     client.initialize("1", IP_Address, 1883); //initialise connection with ID 1, R_Pi IP, and port 1883
//     client.connect(); // connect with above parameters
//     //client.publishMesage("Image_Format", "0");

// }


Widget::~Widget()
{
    delete ui;
}

//-----emit signal when closed to do final processes-----
void Widget::closeEvent(QCloseEvent *event)
{
    client.publishMesage("Program_Closed", "1");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    cout << "closed" << endl;
    QWidget::closeEvent(event);
}
//-------------------------------------------------------


string removeExtension(const string& filename) {
    //find the last occurrence of '.' as this is the start of the file extension
    size_t lastDot = filename.find_last_of(".");
    if (lastDot == string::npos) return filename; //if somehow no extension found
    //remove from the last dot to the end of the string
    return filename.substr(0, lastDot);
}

void Widget::Linux_IP_Address(){
    //QString Linux_PC_IP_Address = IPAddress::getInstance().getAddress();
    client.publishMesage("Linux_PC_IP", IPAddress::getInstance().getAddress());
}

string executeCommand(const char* cmd) { //recives the command to execute as a parameter
    std::array<char, 128> buffer; //needed as cant store the result in string result one by one as fgets requires an char array
    string result; //stores the final result from the command
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose); //this opens a pipe which is needed for the popen to get the data from the command run
    if (!pipe) {                                                            //smart pointer is used so when it goes out of scope the memory mangment is done for me
        return "ERROR"; //if couldnt establish a pipe connection/ open the pipe
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { //this reads the output from the command into the buffer one character at a time
        result += buffer.data();                                         // it loops until it gets a nullptr meaning that its the end of the output
    }
    //remove trailing newline character as other wise the command looks like username\n
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

bool Is_Input_A_Number(string String_Input)
{
    for (uint8_t i = 0; i < String_Input.length(); i++)
        if (isdigit(String_Input[i]) == false)
            return false;

    return true;
}


//-------------------------Set Directory to Save Images in on RPI-------------------------
void Widget::on_RPI_Directory_Location_Line_Edit_returnPressed()
{
    QString RPI_Directory_Save_Location = ui->RPI_Directory_Location_Line_Edit->text();
    ui->Currently_Set_RPI_Directory_Location_Line_Edit->setText(RPI_Directory_Save_Location);
    client.publishMesage("RPI_Save_Directory", RPI_Directory_Save_Location);
    ui->RPI_Directory_Location_Line_Edit->clear();
}
//---------------------------------------------------------------------------------------


//---------------Send Take Image CMD---------------
void Widget::on_Take_Image_Push_Button_clicked()
{
    QStringList SelectedIndexesStrings;
    for (int Index : SelectedIndexesSet) { //SelectedIndexesSet is declared as private in the widget.h
        QString IndexAsString = QString::number(Index);
        SelectedIndexesStrings.append(IndexAsString);
    }

    QString IndexesAsString = SelectedIndexesStrings.join(","); //concat the indexes into a csv format
    qDebug() << "All Selected Indexes as QString: " << IndexesAsString;

    // Filtering out empty values and concatenating non-empty values for Advanced Processing Options
    QStringList advancedOptions;
    if (!Advanced_Image_Processing_Options_Parameters::Custom_Brightness_Value.isEmpty()) {
        advancedOptions.append(Advanced_Image_Processing_Options_Parameters::Custom_Brightness_Value);
    }
    if (!Advanced_Image_Processing_Options_Parameters::Custom_White_Balance_Value.isEmpty()) {
        advancedOptions.append(Advanced_Image_Processing_Options_Parameters::Custom_White_Balance_Value);
    }
    if (!Advanced_Image_Processing_Options_Parameters::Custom_Gamma_Value.isEmpty()) {
        advancedOptions.append(Advanced_Image_Processing_Options_Parameters::Custom_Gamma_Value);
    }
    QString All_Advanced_Processing_Options_Concat = advancedOptions.join(";");

    qDebug() << "Filtered Advanced Options: " << All_Advanced_Processing_Options_Concat;

    client.publishMesage("Image_Processing_Options", IndexesAsString); //publish the concat value

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //this was needed as without it the second publish would never execute

    //publish advanced image processing options
    if (!All_Advanced_Processing_Options_Concat.isEmpty()) {
        client.publishMesage("Advanced_Image_Processing_Options", All_Advanced_Processing_Options_Concat);
    }

    QString Linux_PC_Username = QString::fromStdString(executeCommand("whoami"));
    client.publishMesage("WhoAmI", Linux_PC_Username);
    //qDebug() << "Output: " << Linux_PC_Username;

    Linux_IP_Address(); //gets and sends the Linux IP address to raspberry pi via MQTT

    client.publishMesage("Take_Image", "1"); //might need to delay the execution of this with a timer
}
//-------------------------------------------------





void Widget::on_Directory_PC_Line_Edit_returnPressed()
{
    QString PC_Directory_Save_Location = ui->Directory_PC_Line_Edit->text();
    string PC_Directory_Save_Location_String = PC_Directory_Save_Location.toStdString();
    string Confirmed_Correct_PC_Directory_Save_Location = PC_Directory_Save_Location_String;
    ui->Currently_Set_PC_Directory_Location_Line_Edit->setText(PC_Directory_Save_Location);

    //get the current users home directory from the HOME environment variable
    const char* Home_Directory = std::getenv("HOME");
    string Home_Directory_String (Home_Directory);

    //check if the path the user entered starts with the current users home directory
    if (PC_Directory_Save_Location_String.find(Home_Directory_String) != 0) {
        //if the path does not start with the user's home directory without this check the program would crash
        cout << "The entered path is invalid or does not belong to the current user." << endl;
        QMessageBox::warning(this, "Invalid Input", "The entered path is invalid or does not belong to the current user");
        ui->Directory_PC_Line_Edit->clear();
        ui->Currently_Set_PC_Directory_Location_Line_Edit->setText("Invalid path does not belong to current user");
        return;
    }

    Confirmed_Correct_PC_Directory_Save_Location.c_str(); //publish the verified correct directory path

    Directory_Parameters::Linux_PC_Save_Directory = ui->Directory_PC_Line_Edit->text();
    qDebug() << "Linux PC: " << Directory_Parameters::Linux_PC_Save_Directory;

    std::filesystem::path dirPath(Directory_Parameters::Linux_PC_Save_Directory.toStdString());

    //check if the path is absolute and either doesnt exist or already exists
    if (dirPath.is_absolute()) {
        if (!std::filesystem::exists(dirPath)) {
            //the path is absolute but does not exist, so create it
            if (std::filesystem::create_directories(dirPath)) {
                cout << "The directory was successfully created." << endl;
                Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory = Directory_Parameters::Linux_PC_Save_Directory;

                //make a thread for monitoring files
                std::thread monitor_files_thread(runMonitoringThread);
                monitor_files_thread.detach(); //detach the thread to allow it to run independently

            }
        } else {
            //here the path is absolute and already exists on the system
            cout << "The path already exists on the system." << endl;
            Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory = Directory_Parameters::Linux_PC_Save_Directory;

            //make a thread for monitoring files
            std::thread monitor_files_thread(runMonitoringThread);
            monitor_files_thread.detach(); //detach the thread to allow it to run independently
        }

        //subdirectory names
        string Sub_Directories[] = {"tiff", "dng", "png"};

        //check and create subdirectories if they don't exist
        for (const string& Sub_Dir : Sub_Directories) {
            std::filesystem::path subdirPath = dirPath / Sub_Dir; //this line performs the concat for the directory to add the usb directory
            if (!std::filesystem::exists(subdirPath)) {           //as in the filesystem library / is overloaded
                if (std::filesystem::create_directory(subdirPath)) {
                    cout << "Subdirectory for " << Sub_Dir << " was successfully created." << endl;
                }
            } else {
                cout << "Subdirectory for " << Sub_Dir << " already exists." << endl;
            }
        }
    } else {
        //the path entered was not absolute thus is invalid
        cout << "This is an invalid path, lease provide a full, absolute path" << endl;
        QMessageBox::warning(this, "Invalid Input", "This is an invalid path, please provide a full absolute path");
    }

    client.publishMesage("PC_Save_Directory", Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory); //publish the verified correct directory path
    ui->Directory_PC_Line_Edit->clear();
}





void Widget::on_Image_Processing_listWidget_itemSelectionChanged() {
    static int lastDemosaicIndex = -1; //store the last valid demosaic selection
    QListWidget* listWidget = ui->Image_Processing_listWidget;

    // Determine the current selections
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    QSet<int> currentSelections;
    for (auto item : selectedItems) { //auto needed as needs to be able to be assigned a QListWidgetItem
        currentSelections.insert(listWidget->row(item));
    }

    const QSet<int> demosaicIndexes = {6, 7, 8, 9}; //indexes of the demosaic algorithms
    QSet<int> currentDemosaicSelections = currentSelections.intersect(demosaicIndexes); //use itersect to populate selectedDemosaicIndexes with pnly
        //the elements that are present in both sets (SelectedIndexesSet)
        //and demosaicIndexes

    //-----if more than one demosaic algorithm is selected then revert to the last valid selection-----
    if (currentDemosaicSelections.size() > 1) {
        //deselect the invalid selection
        for (int index : currentDemosaicSelections) {
            if (index != lastDemosaicIndex) {
                listWidget->item(index)->setSelected(false);
            }
        }
    } else if (currentDemosaicSelections.size() == 1) {
        lastDemosaicIndex = *currentDemosaicSelections.begin();//update the last valid demosaic selection
    }
    //---------------------------------------------------------------------------------------------------

    //-----update the SelectedIndexesSet to reflect the current (valid) selections-----
    SelectedIndexesSet.clear();
    for (auto item : selectedItems) {
        if (item->isSelected()) { //double check the selection state in case changes were made
            SelectedIndexesSet.insert(listWidget->row(item));
        }
    }
    //---------------------------------------------------------------------------------



    QListWidgetItem* Brightness_Option = listWidget->item(2); //get the item at index 2 (Use automatic brightness)
    if (Brightness_Option && Brightness_Option->isSelected()) {
        //the item at index 2 is selected
        Error_Checking_Parameters::Automatic_Brightness_Flag = true;
        qDebug() << "flag val: " << Error_Checking_Parameters::Automatic_Brightness_Flag;
        ui->Custom_Brightness_Line_Edit->setReadOnly(true);
        ui->Custom_Brightness_Line_Edit->setText("Cant use custom and auto brightness together");
    } else {
        //the item at index 2 is not selected
        Error_Checking_Parameters::Automatic_Brightness_Flag = false; //reset flag
        qDebug() << "Item at index 2 is deselected.";
        ui->Custom_Brightness_Line_Edit->setText("");
        ui->Custom_Brightness_Line_Edit->setReadOnly(false);
    }

    QListWidgetItem* White_Balance_Option = listWidget->item(4); //get the item at index 4 (use camera white balance)
    if(White_Balance_Option && White_Balance_Option->isSelected()){
        Error_Checking_Parameters::White_Balance_Flag = true;
        ui->Custom_White_Balance_Line_Edit->setReadOnly(true);
        ui->Custom_White_Balance_Line_Edit->setText("Cant use custom and camera white balance");
    }else{
        Error_Checking_Parameters::White_Balance_Flag = true;
        ui->Custom_White_Balance_Line_Edit->setText("");
        ui->Custom_White_Balance_Line_Edit->setReadOnly(false);

    }


    //print valid selections - just for debug
    qDebug() << "Selected indexes:";
    for (int index : SelectedIndexesSet) {
        qDebug() << index;
    }


}


void Widget::on_Custom_Brightness_Line_Edit_returnPressed()
{
    if(Error_Checking_Parameters::Automatic_Brightness_Flag == false){
        QString Custom_Brightness_Value = ui->Custom_Brightness_Line_Edit->text();

        if(Is_Input_A_Number(Custom_Brightness_Value.toStdString()) && !Custom_Brightness_Value.toStdString().empty() ){
            Advanced_Image_Processing_Options_Parameters::Custom_Brightness_Value = Custom_Brightness_Value;
            qDebug() << "Brightness Value: " << Advanced_Image_Processing_Options_Parameters::Custom_Brightness_Value;
            ui->Custom_Brightness_Line_Edit->clear();
        }else{
            QMessageBox::warning(this, "Invalid Input", "Please enter one numerical value");
            ui->Custom_White_Balance_Line_Edit->clear();
        }

    }else{
        ui->Custom_Brightness_Line_Edit->setReadOnly(true);
    }
}



void Widget::on_Custom_White_Balance_Line_Edit_returnPressed() {
    QString Custom_White_Balance_Value = ui->Custom_White_Balance_Line_Edit->text();

    //check if the string ends with a comma
    if (Custom_White_Balance_Value.endsWith(',')) {
        QMessageBox::warning(this, "Invalid Input", "Input should not end with a comma");
        ui->Custom_White_Balance_Line_Edit->clear();
        return;
    }

    auto values = Custom_White_Balance_Value.split(',', Qt::SkipEmptyParts);

    bool Is_Input_Valid = values.count() == 4; //check if exactly 4 values are provided
    for (const QString& value : values) {
        bool ok;
        value.toFloat(&ok);
        if (!ok) {
            Is_Input_Valid = false;
            break;
        }
    }

    if (Is_Input_Valid) {
        Advanced_Image_Processing_Options_Parameters::Custom_White_Balance_Value = Custom_White_Balance_Value;
        qDebug() << "Custom White Balance Value: " << Advanced_Image_Processing_Options_Parameters::Custom_White_Balance_Value;
        ui->Custom_White_Balance_Line_Edit->clear();
    } else {
        QMessageBox::warning(this, "Invalid Input", "Please enter 4 numeric values for white balance, separated by commas (e.g, 1,1,1,1)");
        ui->Custom_White_Balance_Line_Edit->clear();
    }
}




void Widget::on_Custom_Gamma_Line_Edit_returnPressed() {
    QString Custom_Gamma_Values = ui->Custom_Gamma_Line_Edit->text();

    //check if the string ends with a comma
    if (Custom_Gamma_Values.endsWith(',')) {
        QMessageBox::warning(this, "Invalid Input", "Input should not end with a comma.");
        ui->Custom_Gamma_Line_Edit->clear();
        return;
    }

    auto values = Custom_Gamma_Values.split(',', Qt::SkipEmptyParts);

    bool isValidInput = values.count() == 2; //check if exactly 2 values are provided
    for (const QString& value : values) {
        bool ok;
        value.toFloat(&ok);
        if (!ok) {
            isValidInput = false;
            break;
        }
    }

    if (isValidInput) {
        Advanced_Image_Processing_Options_Parameters::Custom_Gamma_Value = Custom_Gamma_Values;
        qDebug() << "Custom Gamma: " << Advanced_Image_Processing_Options_Parameters::Custom_Gamma_Value;
        ui->Custom_Gamma_Line_Edit->clear();
    } else {
        QMessageBox::warning(this, "Invalid Input", "Please enter 2 numeric values for gamma, separated by a comma (e.g, 1,1)");
        ui->Custom_Gamma_Line_Edit->clear();
    }
}



//----------valueChanged functions needed to update the line edit more granuarly than with the sliderReleased----------
void Widget::on_Cyan_Brightness_horizontalSlider_valueChanged(int value)
{
    //qDebug() << "The Cyan current slider value is:" << value;
    ui->Cyan_LED_Value_from_Slider_label->setText(QString::number(value) + "%");

}


void Widget::on_Red_Brightness_horizontalSlider_valueChanged(int value)
{
    //qDebug() << "The Red current slider value is:" << value;
    ui->Red_LED_Value_from_Slider_label->setText(QString::number(value) + "%");
}


void Widget::on_Green_Brightness_horizontalSlider_valueChanged(int value)
{
    //qDebug() << "The Green current slider value is:" << value;
    ui->Green_LED_Value_from_Slider_label->setText(QString::number(value) + "%");
}


void Widget::on_Blue_Brightness_horizontalSlider_valueChanged(int value)
{
    //qDebug() << "The Blue current slider value is:" << value;
    ui->Blue_LED_Value_from_Slider_label->setText(QString::number(value) + "%");
}
//-------------------------------------------------------------------------------------------------------------------




//-----sliderRelased functions needed as only want to send final value after release via MQTT-----
void Widget::on_Cyan_Brightness_horizontalSlider_sliderReleased()
{
    int Cyan_LED_Slider_Value = ui->Cyan_Brightness_horizontalSlider->value();
    PWM_Parameters::Cyan_LED_Brightness = Cyan_LED_Slider_Value;
    qDebug() << PWM_Parameters::Cyan_LED_Brightness;
    qDebug() << "PWM Param Cyan: " << Cyan_LED_Slider_Value;
    QString Cyan_LED_Slider_Value_Qstring = QString::number(Cyan_LED_Slider_Value);
    client.publishMesage("Cyan_LED_Brightness_Value", Cyan_LED_Slider_Value_Qstring);
    client.publishMesage("Cyan_LED_Brightness_Value", Cyan_LED_Slider_Value_Qstring); //had to be sent twice as for some reason if dont the very 1st message doesnt set the duty
                                                                                      //cycle properly always lower than requested
}

void Widget::on_Red_Brightness_horizontalSlider_sliderReleased()
{
    int Red_LED_Slider_Value = ui->Red_Brightness_horizontalSlider->value();
    PWM_Parameters::Red_LED_Brightness = Red_LED_Slider_Value;
    qDebug() << PWM_Parameters::Red_LED_Brightness;
    qDebug() <<"PWM Param Red: " << Red_LED_Slider_Value;
    QString Red_LED_Slider_Value_Qstring = QString::number(Red_LED_Slider_Value);
    client.publishMesage("Red_LED_Brightness_Value", Red_LED_Slider_Value_Qstring);
    client.publishMesage("Red_LED_Brightness_Value", Red_LED_Slider_Value_Qstring);//had to be sent twice as for some reason if dont the very 1st message doesnt set the duty
                                                                                   //cycle properly always lower than requested
}


void Widget::on_Green_Brightness_horizontalSlider_sliderReleased()
{
    int Green_LED_Slider_Value = ui->Green_Brightness_horizontalSlider->value();
    PWM_Parameters::Green_LED_Brightness = Green_LED_Slider_Value;
    qDebug() <<"PWM Param Green: " << PWM_Parameters::Green_LED_Brightness;
    qDebug() << Green_LED_Slider_Value;

    QString Green_LED_Slider_Value_Qstring = QString::number(Green_LED_Slider_Value);
    client.publishMesage("Green_LED_Brightness_Value", Green_LED_Slider_Value_Qstring);
    client.publishMesage("Green_LED_Brightness_Value", Green_LED_Slider_Value_Qstring);//had to be sent twice as for some reason if dont the very 1st message doesnt set the duty
                                                                                       //cycle properly always lower than requested
}

void Widget::on_Blue_Brightness_horizontalSlider_sliderReleased()
{
    int Blue_LED_Slider_Value = ui->Blue_Brightness_horizontalSlider->value();
    PWM_Parameters::Blue_LED_Brightness = Blue_LED_Slider_Value;
    qDebug() << "PWM Param Blue: " << PWM_Parameters::Blue_LED_Brightness;
    qDebug() << Blue_LED_Slider_Value;
    QString Blue_LED_Slider_Value_Qstring = QString::number(Blue_LED_Slider_Value);
    client.publishMesage("Blue_LED_Brightness_Value", Blue_LED_Slider_Value_Qstring);
    client.publishMesage("Blue_LED_Brightness_Value", Blue_LED_Slider_Value_Qstring);//had to be sent twice as for some reason if dont the very 1st message doesnt set the duty
                                                                                     //cycle properly always lower than requested
}
//-------------------------------------------------------------------------------------------------


void Widget::on_PWM_Frequency_Line_Edit_returnPressed()
{
    QString PWM_Frequency = ui->PWM_Frequency_Line_Edit->text();
    int PWM_Frequency_Value = PWM_Frequency.toInt();

    if(Is_Input_A_Number(PWM_Frequency.toStdString()) && !PWM_Frequency.toStdString().empty()){

        if(PWM_Frequency_Value >= 0 && PWM_Frequency_Value <= 100000){ //these are the limits I set in the PWM library I wrote
            client.publishMesage("PWM_Frequency", PWM_Frequency);
            ui->PWM_Frequency_Line_Edit->clear();
        }else{
            QMessageBox::warning(this, "Invalid Range", "Enter a frequency value between 0-100KHz");
            ui->PWM_Frequency_Line_Edit->clear();
        }
    }else{
        QMessageBox::warning(this, "Invalid Input", "Please enter 1 numeric value for PWM frequency");
        ui->PWM_Frequency_Line_Edit->clear();
    }
}


void Widget::on_Restart_Rpi_System_Service_Push_Button_pressed()   //NOT FINISHED THIS FUNCTION NEED TO GET PI WORKING ON START UP FOR THIS
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Action", "Are you sure you want to restart the Raspberry Pi system service?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        //add the raspberry pi name before that @ to get it to work
        string command = "ssh @raspberrypi.local 'systemctl is-active Microscope_Controller.service'";
        string status = executeCommand(command.c_str());

        // Convert string to QString
        QString qStatus = QString::fromStdString(status);

        // Now, qStatus contains "active", "inactive", "failed", "unknown", or "ERROR" in case of failure
        qDebug() << "Service status:" << qStatus;

        // Update the UI or take action based on the status
        // For example: statusLabel->setText("Service Status: " + qStatus);
    } else {
        //user clicked 'No', so you might want to do nothing or handle it differently
        qDebug() << "Action canceled by user.";
    }
}




void Widget::on_Open_Image_in_Napri_Push_Button_pressed() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Images (*.png *.tiff)")); //only allows .png and .tiff to be selected
    if (!filePath.isEmpty()) {

        QtConcurrent::run([filePath, this]() {
            QProcess process;

            QString pythonCommand = "python3"; //python interpreter to use
            QString scriptPath = "Open_Napari_With_User_Image.py"; //add the full path to this file for it to work

            //set the process environment to inherit the applications environment
            process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());

            //start the process with the python script and the selected file path
            process.start(pythonCommand, QStringList() << scriptPath << filePath);
            process.waitForFinished();

            qDebug() << process.readAllStandardOutput();
            qDebug() << process.readAllStandardError();

        });
    }
}

void Widget::on_PWM_Pins_Line_Edit_returnPressed()
{
    QString PWM_Pins = ui->PWM_Pins_Line_Edit->text().trimmed(); //remove spaces

    //check if the string ends with a comma
    if (PWM_Pins.endsWith(',')) {
        QMessageBox::warning(this, "Invalid Input", "Input should not end with a comma.");
        ui->PWM_Pins_Line_Edit->clear();
        return;
    }

    auto Raw_Values = PWM_Pins.split(',', Qt::SkipEmptyParts);
    QStringList User_Entered_PWM_Pins_Values;

    //remove spaces from each value and add to the new list if it is not empty
    for (const QString& Raw_Value : Raw_Values) {
        QString value = Raw_Value.trimmed();
        if (!value.isEmpty()) {
            User_Entered_PWM_Pins_Values.append(value);
        }
    }

    bool isValidInput = User_Entered_PWM_Pins_Values.count() == 4; //check if exactly 4 different/unique values are entered
    QSet<int> Unique_Pins;

    for (const QString& value : User_Entered_PWM_Pins_Values) {
        bool ok;
        int Pin_Value = value.toInt(&ok);

        //check user input not below or greater than 27, as these pins are reserved on RPi
        if (!ok || Pin_Value < 2 || Pin_Value > 27) {
            isValidInput = false;
            break;
        }

        //check to see if the user already entered that pin
        if (Unique_Pins.contains(Pin_Value)) {
            isValidInput = false;
            break;
        } else {
            Unique_Pins.insert(Pin_Value);
        }
    }

    if (isValidInput) { //if the input is valid
        QString PWM_Pins = User_Entered_PWM_Pins_Values.join(","); //join the cleaned values with commas

        qDebug() << "PWM Pins:" << PWM_Pins;

        client.publishMesage("PWM_Pins", PWM_Pins);

        ui->Cyan_Brightness_horizontalSlider->setEnabled(true);
        ui->Red_Brightness_horizontalSlider->setEnabled(true);
        ui->Green_Brightness_horizontalSlider->setEnabled(true);
        ui->Blue_Brightness_horizontalSlider->setEnabled(true);

        ui->PWM_Pins_Line_Edit->clear();
    } else {
        QMessageBox::warning(this, "Invalid Input", "Please enter 4 unique numeric values between 2 and 27, separated by commas (e.g., 2,3,4,5) without any spaces.");
        ui->PWM_Pins_Line_Edit->clear();
    }
}

