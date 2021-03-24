//страж включения
#ifndef RESULTSHOWWINDOW_H
#define RESULTSHOWWINDOW_H

//подключение Qt библиотек
#include <QDialog>
#include <QVector>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFontDatabase>

//подключение STL библиотек
#include <string>
#include <sstream>
#include <iomanip>

//помещение класса ResultShowWindow в пространство Ui
namespace Ui {
class ResultShowWindow;
}

//класс окна вывода результата
class ResultShowWindow : public QDialog
{
    Q_OBJECT

public:
    //конструкторы класса
    explicit ResultShowWindow(const QVector<QVector<std::optional<double>>>& x,
                              const QVector<QVector<std::optional<double>>>& res,
                              const QVector<double> &vector_sum_traffic0,
                              const QVector<QVector<QVector<std::optional<double>>>> &vector_transport_reference0,
                              QWidget *parent = nullptr);

    //деструктор класса
    ~ResultShowWindow();

private:
    //метод создания таблицы
    static void set_table(QStandardItemModel *model,
                               const QVector<QVector<std::optional<double>>>& matrix);

    //метод заполнения таблицы
    static void set_table_data(QStandardItemModel *model,
                               const QVector<QVector<std::optional<double>>>& matrix);

    //метод приведения вещественного значения в строку
    static std::string double_to_string(double value);

    //указатель на объект класса ResultShowWindow
    Ui::ResultShowWindow *ui;

    //матрица x(опорный план)
    QVector<QVector<std::optional<double>>> x_opt;

    //матрица x(оптимизированный план)
    QVector<QVector<std::optional<double>>> res_opt;

    //указатели на модели
    QStandardItemModel *model_x;
    QStandardItemModel *model_res;

    //вектор значений сумм перевозок по планам в ходе оптимизации
    QVector<double> vector_sum_traffic;

    //контейнер матриц x, отражающих процесс оптимизации
    QVector<QVector<QVector<std::optional<double>>>> vector_transport_reference;
};

#endif // RESULTSHOWWINDOW_H
