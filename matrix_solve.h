//страж включения
#ifndef MATRIX_SOLVE_H
#define MATRIX_SOLVE_H

//подключение Qt библиотек
#include <QString>
#include <QVector>
#include <QDebug>

//подключение STL библиотек
#include <optional>
#include <cmath>

//класс обработки матрицы
class matrix_solve
{
private:

    //методы нахождения детерминанта квадратной матрицы
    static int search(double** a, int m, int n, double what,
        bool match, unsigned int& uI, unsigned int& uJ,
        unsigned int starti, unsigned int startj);

    static void swaprows(double** a, int n, int m, unsigned int x1, unsigned int x2);

    static void swapcolumns(double** a, int n, int m, unsigned int x1, unsigned int x2);

    static double determinant(double** a, unsigned int n);

public:
    //конструктор класса
    matrix_solve();

    //предикат проверки наличия в матрице хотя бы одного отрицательного элемента
    static bool matrix_has_negative(const QVector<QVector<double>>&matrix);

    //предикат проверки наличия в векторе хотя бы одного отрицательного элемента
    static bool vector_has_negative(const QVector<double>&vector);

    //метод дописывания вектора к матрице
    static QVector<QVector<double>> adding_vector_to_lines_matrix(
                                              const QVector<QVector<double>>&matrix,
                                              const QVector<double>&vector);

    //методы транспонирования матрицы
    static QVector<QVector<double>> transpose_matrix(const QVector<QVector<double>> & matrix);
    static QVector<QVector<std::optional<double>>> transpose_matrix(const QVector<QVector<std::optional<double>>> & matrix);

    //метод поиска координат минимального элемента в матрице
    static void idxs_min_element(const QVector<QVector<double>> &matrix, uint32_t &idx_row, uint32_t &idx_column);

    //предикат проверки матрицы на наличие только бесконечностей и только нулей
    static bool is_only_infinity_and_only_zero(const QVector<QVector<double>> &matrix);

    //метод удаления колонки по индексу
    static QString delete_column (QVector<QVector<double>> &matrix, int32_t &idx);

    //метод решения СЛАУ по методу Крамера
    static QString solve_system_by_Kramer(const QVector<QVector<double>> &matrix,
                                          const QVector<double> &addition,
                                          QVector<double> &result);

    //метод нахождения разности матриц
    static QVector<QVector<double>> div_matrix(const QVector<QVector<double>> &matrix_left,
                                          const QVector<QVector<double>> &matrix_right);

    //метод избаваления элементов матрицы от optional
    static QVector<QVector<double>> matrix_extract(const QVector<QVector<std::optional<double>>> &matrix);

    //метод одевания элементов матрицы в optional
    static QVector<QVector<std::optional<double>>> matrix_unextract(const QVector<QVector<double>> &matrix);

    //метод нахождения детерминанта
    static QString get_det(const QVector<QVector<double>> &matrix, double &det);

    //метод получения координат самого минимального элемента
    static std::pair<uint32_t, uint32_t> get_coords_very_min_element(const QVector<QVector<double>> &matrix);

    //предикат проверки матрицы на нулевую сумму элементов по строкам и по столбцам
    static bool check_matrix_sum_only_null_in_rows_and_in_columns(const QVector<QVector<double>> &matrix);

    //метод получения координат минимального отрицательного элемента по маске
    static std::pair<uint32_t, uint32_t> get_coords_min_element_has_minus_in_mask
                               (const QVector<QVector<std::optional<double>>> &matrix,
                                const QVector<QVector<double>> &mask);

    //метод оптимизации опорного плана путем пересчета его по выбранному циклу
    static void optimization_matrix_by_cycle_permutation(QVector<QVector<std::optional<double>>> &matrix,
                                                         const QVector<QVector<double>> &mask,
                                                         double delta);

    //метод нахождения стоимости перевозок по выбранному плану
    static double calculate_sum_price(const QVector<QVector<std::optional<double>>> &x,
                                      const QVector<QVector<double>> &c,
                                      const QVector<double> &pi,
                                      bool adding_pi);

    //компаратор матриц
    static QString matrix_comparator(const QVector<QVector<std::optional<double>>> &left,
                                                   const QVector<QVector<std::optional<double>>> &right,
                                                   bool &eq);

    //метод перемещения нуля
    static QString move_null(QVector<QVector<std::optional<double>>> &matrix,
                          const QVector<QVector<double>> &mask,
                          double delta);
};

#endif // MATRIX_SOLVE_H
