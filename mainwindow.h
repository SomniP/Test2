//страж включения
#ifndef MAINWINDOW_H
#define MAINWINDOW_H


//подключение пользовательских классов
#include "tools.h"
#include "resultshowwindow.h"

//подключение Qt библиотек
#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVector>
#include <QFile>
#include <QDataStream>

//подключение STL библиотек
#include <algorithm>
#include <optional>
#include <fstream>
#include <iomanip>
#include <sstream>

//внесение класса MainWindows в пространство имён Ui
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//класс главного окна программы
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //конструктор класса
    MainWindow(QWidget *parent = nullptr);

    //деструктор класса
    ~MainWindow();

    //метод преобразования вещественного числа в строку
    static std::string double_to_string(double value);

private slots:
    //слот обработки сигнала снятия фокуса поля ввода количества источников
    void on_sources_count_editingFinished();

    //слот обработки сигнала снятия фокуса поля ввода количества потребителей
    void on_customers_count_editingFinished();

    //слот обработки сигнала нажатия на кнопку "Результат"
    void on_enter_result_clicked();

private:

    //набор константных значений, определяющий метод получения опорного плана
    enum mode_calc_basic_plan
    {
        NORTHWEST_CORNER = 0,
        MINIMUM_ELEMENT
    };

    //указатель на объект класса MainWindow
    Ui::MainWindow *ui;

    //метод получения опорного плана
    mode_calc_basic_plan mode_cbp;

    //количество источников
    int sources_count_number;

    //количество потребителей
    int customers_count_number;

    //указатели на модель и на ячейку в таблице
    QStandardItemModel *model;
    QStandardItem *item;

    //матрица с
    QVector<QVector<double>> tariff_traffic;

    //вектор значений объема спроса сырья
    QVector<double> vector_bi;

    //вектор значений объема предложений
    QVector<double> vector_ai;

    //вектор значений отпускных цен поставщиков
    QVector<double> vector_pi;

    //объект класса tools
    tools tool;

    //указатель на объект окна вывода результата
    ResultShowWindow *rsw;

    //метод создания таблицы
    void set_table();

    //метод чтения таблицы исходных данных с формы
    QVariant read_table();

    //метод заполнения таблицы
    void set_table_data();
};
#endif // MAINWINDOW_H
