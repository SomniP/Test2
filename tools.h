//страж включения
#ifndef TOOLS_H
#define TOOLS_H

//подключение пользовательских классов
#include "graph.h"
#include "matrix_solve.h"

//подключение Qt библиотек
#include <QVector>
#include <QDebug>

//подключение STL библиотек
#include <algorithm>
#include <limits>
#include <cmath>
#include <functional>
#include <optional>
#include <fstream>
#include <iomanip>

//класс построения и оптимизации опорного плана
class tools
{
public:
    //конструкторы класса
    tools();
    tools(const QVector<QVector<double>> &tariff_traffic,
          const QVector<double> &vector_bi,
          const QVector<double> &vector_ai,
          const QVector<double> &vector_pi,
          bool adding_pi0);

    //метод перехода к закрытой модели
    void equalizer();

    //метод нахождения опорного плана по методу северо-западного угла
    void north_west_corner();

    //метод проверки плана
    QString check_reference_plan();

    //метод нахождения опорного плана по методу минимального элемента
    QString min_element_matrix();

    //геттеры класса
    void get_matrixes(QVector<QVector<double>>& c,
                      QVector<QVector<double>>& x,
                      QVector<QVector<double>>& res);

    void get_matrixes_opt(QVector<QVector<double>>& c,
                      QVector<QVector<std::optional<double>>>& x,
                      QVector<QVector<std::optional<double>>>& res);

    void get_pack_x_and_sums_traffic(QVector<QVector<QVector<std::optional<double>>>> &vector_transport_reference0,
                                     QVector<double> &vector_sum_traffic0);


    //метод оптимизации опорного плана по методу потенциалов
    QString method_potential_optimization(uint32_t &count_iter);

    //метод расстановки знаков в цикле оптимизации
    QVector<QVector<double>> setting_signals_in_transport_reference(const QVector<std::pair<uint32_t, uint32_t>> &cycle);

    //метод приведения вещественного значения к строке
    static std::string double_to_string(double value);

private:   
    //метод выполнения одной итерации метода потенциалов
    QString method_potential(bool &need_optimization);

    //флаг, указывающий было ли выполнено добавление отпускных цен поставщиков элементам матрицы с
    bool adding_pi;

    //матрица тарифов перевозок(с)
    QVector<QVector<double>> tariff_traffic;

    //матрица количества единиц перевозимого сырья от источников к потребителям(x)
    QVector<QVector<std::optional<double>>> transport_reference;

    //матрица результата оптимизации
    QVector<QVector<std::optional<double>>> result_optimization;

    //вектор объем спроса сырья
    QVector<double> vector_bi;

    //вектор объем предложения сырья
    QVector<double> vector_ai;

    //вектор отпускных цен поставщиков
    QVector<double> vector_pi;

    //вектор сумм перевозок по планам в ходе оптимизации
    QVector<double> vector_sum_traffic;

    //контейнер матриц x, отражающий ход оптимизации плана
    QVector<QVector<QVector<std::optional<double>>>> vector_transport_reference;
};

#endif // TOOLS_H
