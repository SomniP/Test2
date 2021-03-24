#include "matrix_solve.h"

//реализация конструктора класса matrix_solve
matrix_solve::matrix_solve()
{}

//предикат проверки наличия в векторе хотя бы одного отрицательного элемента
bool matrix_solve::vector_has_negative(const QVector<double>&vector)
{
    //предикат проверки отрицательный ли элемент
    auto is_negative = [](double value)
    {
        return value < -1e-6;
    };

    QVector<double>::const_iterator it = std::find_if(std::begin(vector), std::end(vector), is_negative);
    //если отрицательный элемент нашёлся
    return it != std::end(vector);
}
//реализация метода дописывания вектора к матрице
QVector<QVector<double>> matrix_solve::adding_vector_to_lines_matrix(const QVector<QVector<double>> &matrix,
                                              const QVector<double> &vector)
{
    //если размер матрицы не соответствует размеру добавляемого вектора, то возвращаем пустую матрицу
    if(matrix.size() != vector.size())
        return QVector<QVector<double>>();
    //матрица для дописывания вектора
    QVector<QVector<double>> result = QVector<QVector<double>>(
                                      matrix.size(),
                                      QVector<double>(matrix.front().size())
                                      );
    //проход по заданной матрице
    for(uint32_t idx_row = 0u; idx_row < matrix.size(); ++idx_row)
    {
        for(uint32_t idx_column = 0u; idx_column < matrix.front().size(); ++idx_column)
        {
            //помещение матрицы и вектора в единую матрицу
            result[idx_row][idx_column] = matrix[idx_row][idx_column] + vector[idx_row];
        }
    }
    return result;
}

//реализация методов транспонирования матрицы
QVector<QVector<double>> matrix_solve::transpose_matrix(const QVector<QVector<double>> &matrix)
{
    //матрица, в которую будет помещаться результат транспонирования
    QVector<QVector<double>> matrix_result (matrix.front().size(), QVector<double> (matrix.size()));

    //проход по исходной матрице, её транспонирование(замена индексов)
    for(uint32_t idx_row =0u; idx_row < matrix.size(); ++idx_row)
        for(uint32_t idx_column =0u; idx_column < matrix.front().size(); ++idx_column)
            matrix_result[idx_column][idx_row] = matrix[idx_row][idx_column];

    return matrix_result;
}

QVector<QVector<std::optional<double>>> matrix_solve::transpose_matrix(const QVector<QVector<std::optional<double>>> &matrix)
{
    //матрица, в которую будет помещаться результат транспонирования
    QVector<QVector<std::optional<double>>> matrix_result (matrix.front().size(), QVector<std::optional<double>> (matrix.size()));

    //проход по исходной матрице, её транспонирование(замена индексов)
    for(uint32_t idx_row =0u; idx_row < matrix.size(); ++idx_row)
        for(uint32_t idx_column =0u; idx_column < matrix.front().size(); ++idx_column)
            matrix_result[idx_column][idx_row] = matrix[idx_row][idx_column];

    return matrix_result;
}

//реализация метода поиска координат минимального элемента в матрице
void matrix_solve::idxs_min_element(const QVector<QVector<double>> &matrix, uint32_t &idx_row, uint32_t &idx_column)
{
    //максимальное значение для типа double
    double min_value = std::numeric_limits<double>::max();

    //поиск минимального элемента и его координат в матрице
    for(uint32_t idx_r =0u; idx_r < matrix.size(); ++idx_r)
        for(uint32_t idx_c =0u; idx_c < matrix.front().size(); ++idx_c)
        {
            //сравнение значения текущего элемента с минимумом и проверка на то, что элемент не равен нулю
            if (matrix[idx_r][idx_c]< min_value && std::abs(matrix[idx_r][idx_c]) > 1e-6)
            {
                min_value = matrix[idx_r][idx_c];
                idx_row = idx_r;
                idx_column = idx_c;
            }
        }
}
//предикат проверки матрицы на наличие только бесконечностей и только нулей
bool matrix_solve::is_only_infinity_and_only_zero(const QVector<QVector<double>> &matrix)
{
    //предикат проверки является ли число бесконечностью
    auto is_inf=[](double value)
    {
        return std::isinf(value);
    };

    //предикат проверки является ли число нулем
    auto is_zero=[](double value)
    {
        return std::abs(value) < 1e-6;
    };

    uint32_t count_inf =0u;
    uint32_t count_zero =0u;

    //подсчет количества бесконечностей и нулей в матрице
    for(uint32_t idx_row=0u; idx_row < matrix.size();++idx_row)
    {
        count_inf += std::count_if (std::begin(matrix[idx_row]), std::end(matrix[idx_row]), is_inf);
        count_zero += std::count_if(std::begin(matrix[idx_row]), std::end(matrix[idx_row]), is_zero);
    }
    //если все элементы только нули и бесконечности, то возвращается true, иначе false
    return count_zero + count_inf == matrix.size()*matrix.front().size();
}

//реализация метода удаления колонки по индексу
QString matrix_solve::delete_column(QVector<QVector<double>> &matrix, int32_t &idx)
{
    //проверка матрицы на заполненность
    if(!matrix.size())
        return QString("Матрица пустая");
    //проверка индекса на выход за пределы матрицы
    if(idx >= matrix.front().size())
        return QString("Индекс столбика за пределами матрицы");

    //транспонирование matrix
    matrix = transpose_matrix(matrix);

    if(idx == -1)
    {
        matrix.pop_back();
        //получение индекса
        idx = matrix.size() - 1;
    }
    else
    {
        //удаление колонки по индексу
        matrix.erase(std::begin(matrix) + idx);
    }

    //возврат матрицы в исходное состояние, путем повторного транспонирование
    matrix=transpose_matrix(matrix);
    return QString();
}
//реализация методов нахождения детерминанта квадратной матрицы
int matrix_solve::search(double** a, int m, int n, double what,
    bool match, unsigned int& uI, unsigned int& uJ, unsigned int starti, unsigned int startj)
{
    // Поиск в матрице a[m][n] элемента с указанным значением what
    // Возвращаеются его номер строки и столбца uI, uJ, если элемент найден.
    // match - искать равный элемент или отличный от указанного.
    // Вернёт 0 - не найдено, не 0 - найдено
    if ((!m) || (!n)) return 0;
    if ((starti >= n) || (startj >= m)) return 0;
    for (unsigned int i = starti; i < n; i++)
        for (unsigned int j = startj; j < m; j++)
        {
            if (match == true)
            {
                if (a[i][i] == what)
                {
                    uI = i; uJ = j;
                    return 1;
                }
            }
            else if (a[i][j] != what)
            {
                uI = i; uJ = j;
                return 1;
            }
        }
    return 0;
}

void matrix_solve::swaprows(double** a, int n, int m, unsigned int x1, unsigned int x2)
{
    //Меняет в матрице a[n][m] строки с номерами x1 и x2 местами
    if ((!n) || (!m))
        return;
    if ((x1 >= n) || (x2 >= n) || (x1 == x2))
        return;
    double tmp;
    for (unsigned int x = 0; x < m; x++)
    {
        tmp = a[x1][x];
        a[x1][x] = a[x2][x];
        a[x2][x] = tmp;
    }
    return;
}

void matrix_solve::swapcolumns(double** a, int n, int m, unsigned int x1, unsigned int x2)
{
    //Меняет в матрице a[n][m] столбцы с номерами x1 и x2 местами
    if ((!n) || (!m))
        return;
    if ((x1 >= m) || (x2 >= m) || (x1 == x2))
        return;
    double tmp;
    for (unsigned int x = 0; x < n; x++)
    {
        tmp = a[x][x1];
        a[x][x1] = a[x][x2];
        a[x][x2] = tmp;
    }
    return;
}

double matrix_solve::determinant(double** a, unsigned int n)
{
    //Вычисление определителя квадратной матрицы a[n][n]
    unsigned int m = n;
    if (m == 0) return 0;
    if (m == 1) return a[0][0];
    if (m == 2) return (a[0][0] * a[1][1] - a[1][0] * a[0][1]);
    bool sign = false; // смена знака определителя. по умолчанию - нет
    double det = 1; // определитель
    double tmp;
    unsigned int x, y;
    for (unsigned int i = 0; i < n; i++)
    { // цикл по всей главной диагонали
        if (a[i][i] == 0)
        { // если элемент на диагонали равен 0, то ищем ненулевой элемент в матрице
            if (!search(a, m, n, 0, false, y, x, i, i))
                return 0; // если все элементы нулевые, то опр. = 0
            if (i != y)
            { // меняем i-ую строку с y-ой
                swaprows(a, m, n, i, y);
                sign = !sign;
            }
            if (i != x)
            { // меняем i-ый столбец с x-ым
                swapcolumns(a, m, n, i, x);
                sign = !sign;
            }
            // таким образом, в a[i][i], теперь ненулевой элемент.
        }
        // выносим элемент a[i][i] за определитель
        det *= a[i][i];
        tmp = a[i][i];
        for (x = i; x < m; x++)
        {
            a[i][x] = a[i][x] / tmp;
        }
        // таким образом a[i][i] теперь равен 1
        // зануляем все элементы стоящие под (i, i)-ым,
        // при помощи вычитания с опр. коеффициентом
        for (y = i + 1; y < n; y++)
        {
            tmp = a[y][i];
            for (x = i; x < m; x++)
                a[y][x] -= (a[i][x] * tmp);
        }
    }
    if (sign)
        return det * (-1);
    return det;
}

//реализация метода решения СЛАУ по методу Крамера
QString matrix_solve::solve_system_by_Kramer(const QVector<QVector<double>> &matrix,
                                      const QVector<double> &addition,
                                      QVector<double> &result)
{
    double main_delta;
    //вызов метода нахождения детерминанта(главная дельта)
    QString err = get_det(matrix, main_delta);

    //проверка на наличие ошибки
    if(!err.isEmpty())
        return err;

    //транспонирование матрицы
    QVector<QVector<double>> trans_matrix = transpose_matrix(matrix);

    result = QVector<double>(trans_matrix.size());

    for(uint32_t idx =0u; idx<trans_matrix.size(); ++idx)
    {
        //копирование транспонированной матрицы
        QVector<QVector<double>> trans_matrix_copy = trans_matrix;

        //помещение вектора дополнений в транспонированную матрицу
        trans_matrix_copy[idx] = addition;

        //копирование повторно транспонированной матрицы
        QVector<QVector<double>> matrix_copy = transpose_matrix(trans_matrix_copy);

        double secondary_delta;

        //вызов метода нахождения детерминанта(вторичная дельта)
        QString err = get_det(matrix_copy, secondary_delta);

        //проверка на наличие ошибки
        if(!err.isEmpty())
            return err;

        //подсчет результата решения СЛАУ
        result[idx] = secondary_delta/main_delta;
    }

    return QString();
}

//реализация метода нахождения разности матриц
QVector<QVector<double>> matrix_solve::div_matrix(const QVector<QVector<double>> &matrix_left,
                                      const QVector<QVector<double>> &matrix_right)
{   //одинаковые ли матрицы по размеру
    if(matrix_left.size()!= matrix_right.size())
        return {};

    //матрица для записи вычисления разности матриц
    QVector<QVector<double>> result(matrix_left.size(), QVector<double>(matrix_left.front().size()));

    for(uint32_t idx_row =0u; idx_row< matrix_left.size(); ++idx_row)
    {
        //одинаковые ли строки матриц по размеру
        if(matrix_left[idx_row].size()!=matrix_right[idx_row].size())
            return {};

        //вычисление разности матриц
        for(uint32_t idx_column =0u; idx_column< matrix_left.front().size(); ++idx_column)
        {
            result[idx_row][idx_column]= matrix_left[idx_row][idx_column]-matrix_right[idx_row][idx_column];
        }
    }
    return result;
}

//предикат проверки наличия в матрице хотя бы одного отрицательного элемента
bool matrix_solve::matrix_has_negative(const QVector<QVector<double>> &matrix)
{
    //предикат на проверку является ли значение отрицательным
    std::function<bool(double)> is_negative = [](double value)
    {
        return value < -1e-6;
    };

    //проход по строкам в матрице
    for(const QVector<double> &line: matrix)
    {
        //поиск отрицательных элементов в матрице
        QVector<double>::const_iterator it = std::find_if(std::begin(line), std::end(line), is_negative);

        if(it!=std::end(line))
            return true;
    }
    return false;
}

//реализация метода избавления элементов матрицы от optional
QVector<QVector<double>> matrix_solve::matrix_extract(const QVector<QVector<std::optional<double>>> &matrix)
{
    QVector<QVector<double>> result(matrix.size(), QVector<double> (matrix.front().size()));

    //перебор элементов матрицы по индексам
    for(uint32_t idx_row =0u; idx_row< matrix.size(); ++idx_row)
        for(uint32_t idx_column =0u; idx_column<matrix.front().size(); ++idx_column)
        {
            //извлечение элемента из матрицы
            std::optional<double> value = matrix[idx_row][idx_column];
            //проверка на существование значения в ячейки матрицы
            if(value.has_value())
            {
                result[idx_row][idx_column]= value.value();
            }
            else
                return {};
        }
    return result;
}

//реализация метода одевания элементов матрицы в optional
QVector<QVector<std::optional<double>>> matrix_solve::matrix_unextract(const QVector<QVector<double>> &matrix)
{
    //создание контейнерa для сохранения и вывода результата
    QVector<QVector<std::optional<double>>> result(matrix.size(), QVector<std::optional<double>>(matrix.front().size()));

    //перебор элементов матрицы matrix с заполнением матрицы result
    for(uint32_t idx_row = 0u; idx_row< matrix.size(); ++idx_row)
        for(uint32_t idx_column = 0u; idx_column<matrix.front().size(); ++idx_column)
        {
            result[idx_row][idx_column]=matrix[idx_row][idx_column];
        }

    return result;
}

//реализация метода нахождения детерминанта
QString matrix_solve::get_det(const QVector<QVector<double>> &matrix, double&det)
{
    //проверка матрицы на то, что она является квадратной
    if(matrix.size()!= matrix.front().size())
        return QString("Матрица не является квадратной");

    uint32_t n = matrix.size();
    double** a = new double*[n];
    for(uint32_t i = 0u; i < n; i++)
        a[i] = new double[n];
    //копирование матрицы в двумерный массив
    for(uint32_t i = 0u; i < n; i++)
        for(uint32_t j = 0u; j < n; j++)
            a[i][j] = matrix[i][j];

    //получение определителя матрицы
    det = determinant(a, n);

    //удаление двумерного массива
    for(uint32_t i = 0u; i < n; i++)
        delete[] a[i];
    delete[] a;

    return QString();
}

//реализация метода получения координат самого минимального элемента
std::pair<uint32_t, uint32_t> matrix_solve::get_coords_very_min_element(const QVector<QVector<double>> &matrix)
{
    uint32_t min_row = 0u;
    uint32_t min_column = 0u;
    //изначальный минимальный элемент
    double min_value = matrix[min_row][min_column];

    //перебор элементов матрицы для нахождения самого минимального значения
    for(uint32_t idx_row = 0u; idx_row < matrix.size(); ++idx_row)
        for(uint32_t idx_column = 0u; idx_column < matrix.front().size(); ++idx_column)
        {
            if(matrix[idx_row][idx_column] < min_value)
            {
                min_value = matrix[idx_row][idx_column];
                min_row = idx_row;
                min_column = idx_column;
            }
        }
    //возвращение пары индексов минимального элемента
    return {min_row, min_column};
}

//предикат проверки матрицы на нулевую сумму элементов по строкам и по столбцам
bool matrix_solve::check_matrix_sum_only_null_in_rows_and_in_columns(const QVector<QVector<double>> &matrix)
{
    //функция суммирования элементов в строке матрицы
    auto summator = [](double acc, double value)
    {
        return acc + value;
    };

    //перебор строк матрицы для подсчета суммы элементов в каждой из них
    for(const QVector<double> &line: matrix)
    {
        double sum_line = std::accumulate(std::begin(line), std::end(line), 0., summator);

        //если сумма элементов в строке отлична от нуля, то возвращается false
        if(std::abs(sum_line) > 1e-6)
            return false;
    }

    //транспонирование матрицы
    QVector<QVector<double>> matrix_transp = transpose_matrix(matrix);

    //перебор столбцов матрицы для подсчета суммы элементов в каждом из них
    for(const QVector<double> &line: matrix_transp)
    {
        double sum_line = std::accumulate(std::begin(line), std::end(line), 0., summator);

        //если сумма элементов в столбце отлична от нуля, то возвращается false
        if(std::abs(sum_line) > 1e-6)
            return false;
    }

    return true;
}

//реализация метода получения координат минимального отрицательного элемента по маске
std::pair<uint32_t, uint32_t> matrix_solve::get_coords_min_element_has_minus_in_mask
                           (const QVector<QVector<std::optional<double>>> &matrix,
                            const QVector<QVector<double>> &mask)
{
    uint32_t min_row = 0u;
    uint32_t min_column = 0u;
    //максимальное значение для типа double
    double min_value = std::numeric_limits<double>::max();

    //перебор элементов маски
    for(uint32_t idx_row = 0u; idx_row < mask.size(); ++idx_row)
        for(uint32_t idx_column = 0u; idx_column < mask.front().size(); ++idx_column)
        {
            //если данный элемент маски равен -1
            if(std::abs(mask[idx_row][idx_column] + 1.) < 1e-6)
            {
                //поиск минимального значения в матрицы среди её элементов, соответствующих по маске -1
                if(matrix[idx_row][idx_column] < min_value)
                {
                    min_row = idx_row;
                    min_column = idx_column;

                    if(matrix[idx_row][idx_column].has_value())
                        min_value = matrix[idx_row][idx_column].value();
                }
            }
        }
    return {min_row, min_column};
}

//реализация метода оптимизации опорного плана путем пересчета его по выбранному циклу
void matrix_solve::optimization_matrix_by_cycle_permutation(QVector<QVector<std::optional<double>>> &matrix,
                                                         const QVector<QVector<double>> &mask,
                                                         double delta)
{
    bool has_close_cell = false;
    //проход по маске
    for(uint32_t idx_row = 0u; idx_row < mask.size(); ++idx_row)
        for(uint32_t idx_column = 0u; idx_column < mask.front().size(); ++idx_column)
        {
            //проверка элемента на равность +1 или -1
            if(std::abs(mask[idx_row][idx_column]) > 1e-6)
            {
                //проверка на то, содержится ли значение в ячейке матрицы
                if(matrix[idx_row][idx_column].has_value())
                {
                    //сдвиг элементов плана в соответствии с циклом оптимизации, задаваемым в маске
                    matrix[idx_row][idx_column] = mask[idx_row][idx_column]*delta +
                             matrix[idx_row][idx_column].value();

                    //если нуля не было, то закрыть ячейку
                    if(std::abs(matrix[idx_row][idx_column].value()) < 1e-6
                            && !has_close_cell)
                    {
                        matrix[idx_row][idx_column] = std::nullopt;
                        has_close_cell = true;
                    }
                }
                //открытие ячейки в matrix и присваивание в неё дельты
                else
                    matrix[idx_row][idx_column] = mask[idx_row][idx_column]*delta;
            }
        }
}

//реализация метода нахождения стоимости перевозок по выбранному плану
double matrix_solve::calculate_sum_price(const QVector<QVector<std::optional<double>>> &x,
                                      const QVector<QVector<double>> &c,
                                      const QVector<double> &pi,
                                      bool adding_pi)
{
    double sum = 0.;

    //обход по матрице с (матрице тарифов)
    for(uint32_t idx_row = 0u; idx_row < c.size(); ++idx_row)
        for(uint32_t idx_column = 0u; idx_column < c.front().size(); ++idx_column)
        {
            //проверка есть ли значение в ячейке матрицы x, и столбик pi был учтен ранее
            if(x[idx_row][idx_column].has_value() && adding_pi)
            {
                sum += x[idx_row][idx_column].value()*c[idx_row][idx_column];
            }

            //проверка есть ли значение в ячейке матрицы x, и столбик pi не был учтен ранее
            else if(x[idx_row][idx_column].has_value() && !adding_pi)
            {
                sum += x[idx_row][idx_column].value()*(c[idx_row][idx_column] + pi[idx_row]);
            }
        }

    return sum;
}

//компаратор матриц
QString matrix_solve::matrix_comparator(const QVector<QVector<std::optional<double>>> &left,
                              const QVector<QVector<std::optional<double>>> &right,
                              bool &eq)
{
    //проверка есть ли в матрицах хотя бы одна строка
    if(!left.size() && !right.size())
    {
        eq = true;
        return QString();
    }
    //проверка отсутствуют ли строки хотя бы в одной матрице
    if(!left.size() || !right.size())
    {
        eq = false;
        return QString("При сравнении одна из матриц имеет значения, а другая - нет");
    }
    //проверка на различие количества строк между матрицами
    if(left.size() != right.size())
    {
        eq = false;
        return QString("В сранение поданы две матрицы с разным количеством строк");
    }

    eq = true;

    //обход по левой матрице
    for(uint32_t idx_row = 0u; idx_row < left.size(); ++idx_row)
    {
        //проверка строки матрицы на совпадение в ней количества элементов
        if(left[idx_row].size() != right[idx_row].size())
        {
            eq = false;
            return QString("В сравнении матриц в их соотвествующих строках разное количество элементов");
        }
        //поэлементное сравнение строк в матрицах
        for(uint32_t idx_column = 0u; idx_column < left.front().size(); ++idx_column)
        {
            if(left[idx_row][idx_column] != right[idx_row][idx_column])
            {
                eq = false;
                break;
            }
        }
        if(!eq)
            break;
    }

    return QString();
}

//реализация метода перемещения нуля
QString matrix_solve::move_null(QVector<QVector<std::optional<double>>> &matrix,
                          const QVector<QVector<double>> &mask,
                          double delta)
{
    //проверка на то, что дельта не является нулем
    if(std::abs(delta) > 1e-6)
    {
        return QString("В методе move_null значение delta не равно нулю");
    }

    uint32_t use_row = -1, use_column = -1;
    //обход по строкам маски
    for(uint32_t idx_row = 0u; idx_row < mask.size(); ++idx_row)
    {
        bool need_break = false;
        //проход по столбцам маски
        for(uint32_t idx_column = 0u; idx_column < mask.front().size(); ++idx_column)
        {
            //если в матрице mask не ноль и соответствующий элемент матрицы matrix не имеет значения,
            //то перемещается ноль
            if(std::abs(mask[idx_row][idx_column]) > 1e-6 &&
                    !matrix[idx_row][idx_column].has_value())
            {
                matrix[idx_row][idx_column] = std::optional(0.);
                need_break = true;
                use_row = idx_row;
                use_column = idx_column;
                break;
            }
        }
        if(need_break)
            break;
    }

    //обход по строкам маски
    for(uint32_t idx_row = 0u; idx_row < mask.size(); ++idx_row)
    {
        bool need_break = false;
        //проход по столбцам маски
        for(uint32_t idx_column = 0u; idx_column < mask.front().size(); ++idx_column)
        {
            if(std::abs(mask[idx_row][idx_column]) > 1e-6 &&
                    matrix[idx_row][idx_column].has_value())
            {
                //если в матрице mask не ноль и соответствующий элемент матрицы matrix имеет значение,
                //то перемещается ноль
                if(std::abs(matrix[idx_row][idx_column].value()) < 1e-6
                        && !(use_row == idx_row && use_column == idx_column))
                {
                    matrix[idx_row][idx_column] = std::nullopt;
                    need_break = true;
                    break;
                }
            }
        }
        if(need_break)
            break;
    }

    return QString();
}
