#include "tools.h"

//реализация конструкторов класса tools
tools::tools()
{}

tools::tools(const QVector<QVector<double>> &tariff_traffic_0,
             const QVector<double> &vector_bi_0,
             const QVector<double> &vector_ai_0,
             const QVector<double> &vector_pi_0,
             bool adding_pi0):
    tariff_traffic(tariff_traffic_0),
    vector_bi(vector_bi_0),
    vector_ai(vector_ai_0),
    vector_pi(vector_pi_0),
    adding_pi(adding_pi0)
{}

//реализация метода перехода к закрытой модели
void tools:: equalizer()
{
    //суммирование элементов вектора ai
    double sum_ai = std::accumulate(std::begin(vector_ai), std::end(vector_ai), 0.);
    //суммирование элементов вектора bi
    double sum_bi = std::accumulate(std::begin(vector_bi), std::end(vector_bi), 0.);

    //если сумма элементов вектора ai больше чем вектора bi
    if(sum_ai > sum_bi)
    {
        //обход по строкам матрицы тарифов перевозок
        for(QVector<double> &line: tariff_traffic)
        {
            //добавление дополнительного нуля в строку матрицы тарифов перевозок
            line.push_back(0.);
        }
        //добавление разницы между суммами ai и bi в вектор bi
        vector_bi.push_back(sum_ai-sum_bi);
    }
    //если сумма элементов вектора ai больше чем вектора bi
    else
    {
        tariff_traffic.push_back(QVector<double>(tariff_traffic.front().size()));
        //добавление разницы между суммами ai и bi в вектор ai
        vector_ai.push_back(sum_bi-sum_ai);
        //добавление дополнительного нуля в вектор pi
        vector_pi.push_back(0.);
    }
}

//реализация метода нахождения опорного плана по методу северо-западного угла
void tools::north_west_corner()
{
    uint32_t idx_row = 0u;
    uint32_t idx_column = 0u;

    //создание матрицы количества единиц перевозимого сырья от источников к потребителям(x)
    transport_reference = QVector<QVector<std::optional<double>>>(tariff_traffic.size(),
                                          QVector<std::optional<double>>(tariff_traffic.front().size()));
    //создание копий векторов ai и bi
    QVector<double> vector_ai_copy(vector_ai.size());
    QVector<double> vector_bi_copy(vector_bi.size());

    //копирование вектора ai
    std::copy(std::begin(vector_ai), std::end(vector_ai),
              std::begin(vector_ai_copy));

    //копирование вектора bi
    std::copy(std::begin(vector_bi), std::end(vector_bi),
              std::begin(vector_bi_copy));

    for (;;)
    {
        //взятие значение первого элемента вектора ai
        double value_ai = vector_ai_copy[idx_row];

        //взятие значение первого элемента вектора bi
        double value_bi = vector_bi_copy[idx_column];

        //если значение элемента ai меньше элемента bi
        if(value_ai < value_bi)
        {
            //записывание значения ai в матрицу x
            transport_reference[idx_row][idx_column]= value_ai;
            //вычитание из копии векторов ai и bi значение элемента вектора ai как меньшего по значению
            vector_ai_copy[idx_row]-=value_ai;
            vector_bi_copy[idx_column]-=value_ai;

            //увеличение индекса строки
            ++idx_row;
        }
        //если значение элемента bi меньше элемента ai
        else
        {
            //записывание значения bi в матрицу с
            transport_reference[idx_row][idx_column]= value_bi;
            //вычитание из копии векторов ai и bi значение элемента вектора ai как меньшего по значению
            vector_ai_copy[idx_row]-=value_bi;
            vector_bi_copy[idx_column]-=value_bi;

            //увеличение индекса колонки
            ++idx_column;
        }

        //проверка выхода индексов за пределы
        if(idx_row >= vector_ai_copy.size() || idx_column >= vector_bi_copy.size())
            break;

    }
}

//реализация метода проверки плана
QString tools::check_reference_plan()
{
    uint32_t idx=0u;
    const double eps = 1e-6;

    //обход по строкам матрицы x
    for(const QVector<std::optional<double>> &line: transport_reference)
    {
        double sum_line = 0.;

        //обход по элементам в строке
        for(std::optional<double> value: line)
        {
            //проверка имеется ли значение в ячейке, при наличии суммировать его
            if(value.has_value())
                sum_line += value.value();
        }
        //проверка совпадения суммы значений матрицы x по строке c соответствующем значением матрицы ai
        if(std::abs(vector_ai[idx] - sum_line) > eps)
            return QString("Сумма значений матрицы x по строке %1 не совпадет c соответствующем "
                           "значением матрицы ai").arg(idx);
        ++idx;
    }

    //транспонирование матрицы x
    QVector<QVector<std::optional<double>>> transport_reference_transpose = matrix_solve::transpose_matrix(transport_reference);

    idx =0u;

    //обход по строкам транспонированной матрицы x
    for(const QVector<std::optional<double>> &line: transport_reference_transpose)
    {
        double sum_line = 0.;

        //обход по элементам в строке
        for(std::optional<double> value: line)
        {
            //проверка имеется ли значение в ячейке, при наличии суммировать его
            if(value.has_value())
                sum_line += value.value();
        }
        //проверка совпадения суммы значений матрицы x по столбцу c соответствующем значением матрицы bi
        if(std::abs(vector_bi[idx] - sum_line) > eps)
            return QString("Сумма значений матрицы x по столбцу %1 не совпадет c соответствующем "
                           "значением матрицы bi").arg(idx);
        ++idx;
    }

    uint32_t count =0u;

    //предикат проверки наличия значения в ячейке
    auto not_null = [](std::optional<double> value)
    {
        return value.has_value();
    };

    //обход по строкам матрицы x
    for(const QVector<std::optional<double>> &line: transport_reference)
    {
        //подсчет количества ячеек, имеющих значение
        count+= std::count_if(std::begin(line), std::end(line), not_null);
    }

    //проверка вырожденности опорного плана
    if(count != transport_reference.size() + transport_reference.front().size() - 1)
    {
        return QString ("Опорный план вырожден: количество базисных клеток не соотвествует m + n - 1, где m - количество строк, n - количество столбцов");
    }

    //создание графа по матрице x
    graph matrix_graph(transport_reference);

    //поиск всех возможных циклов в графе matrix_graph
    matrix_graph.cycles_search();

    //исключение коротких циклов в графе matrix_graph
    matrix_graph.filter_chain_shot_3();

    //исключение циклов, которые располагаются только в одной строке или в одном столбике матрицы x
    matrix_graph.filter_chain_only_one_line_or_column();

    //вызов геттера, возвращающего количество найденных циклов в графе
    uint32_t count_cycles_in_matrix_graph = matrix_graph.count_cycles_in_graph();

    //проверка на наличие хотя бы одного цикла в опорном плане
    if(count_cycles_in_matrix_graph)
    {
        return QString("Исходный опорный план содержит хотя бы один цикл");
    }

    return QString();
}

//реализация метода нахождения опорного плана по методу минимального элемента
QString tools::min_element_matrix()
{
    //инициализация матрицы x
    transport_reference = QVector<QVector<std::optional<double>>>(tariff_traffic.size(),
                                                   QVector<std::optional<double>>(tariff_traffic.front().size()));
    //создание контейнера для копирования вектора ai
    QVector<double> vector_ai_copy(vector_ai.size());

    //создание контейнера для копирования вектора bi
    QVector<double> vector_bi_copy(vector_bi.size());

    //создание матрицы для копирования матрицы с
    QVector<QVector<double>> c_copy(tariff_traffic.size());

    //копирование вектора ai в контейнер vector_ai_copy
    std::copy(std::begin(vector_ai), std::end(vector_ai),
              std::begin(vector_ai_copy));

    //копирование вектора bi в контейнер vector_bi_copy
    std::copy(std::begin(vector_bi), std::end(vector_bi),
              std::begin(vector_bi_copy));

    //копирование матрицы с в c_copy
    std::copy(std::begin(tariff_traffic), std::end(tariff_traffic),
              std::begin(c_copy));

    bool flag_ok = false;

    for(uint32_t idx=0u; idx< 2000u; ++idx)
    {
        //проверка необходимости дальнейшей оптимизации модели
        if(std::abs(std::accumulate(std::begin(vector_ai_copy),std::end(vector_ai_copy), 0.) +
           std::accumulate(std::begin(vector_bi_copy),std::end(vector_bi_copy), 0.)) < 1e-6)
        {
            flag_ok = true;
            break;
        }

        uint32_t idx_row=0u;
        uint32_t idx_column=0u;

        //если в матрице с_сopy остались только бесконечности и нули
        if(matrix_solve::is_only_infinity_and_only_zero(c_copy))
        {
            //находим индекс нулевого элемента
            for (;idx_row < c_copy.size();++idx_row)
            {
               auto it = std::find(std::begin(c_copy[idx_row]),std::end(c_copy[idx_row]), 0.);
               //если нулевой элемент нашелся, то записывается его индекс
               if(it!= std::end(c_copy[idx_row]))
               {
                   idx_column = std::distance(std::begin(c_copy[idx_row]), it);
                   break;
               }
            }
        }
        //если в матрице с_сopy остались не только бесконечности и нули
        else
            //поиск координат минимального элемента в матрице
            matrix_solve::idxs_min_element(c_copy, idx_row, idx_column);

        //зачеркивание столбца
        if(vector_ai_copy[idx_row] < vector_bi_copy[idx_column])
        {
            //запись значения элемента вектора ai в матрицу x
            transport_reference[idx_row][idx_column] = vector_ai_copy[idx_row];

            //вычитание значения элемента вектора ai из элемента вектора bi
            vector_bi_copy[idx_column]-=vector_ai_copy[idx_row];

            //зануление элемента вектора ai
            vector_ai_copy[idx_row]=0.;

            //заполнение столбика матрицы c бесконечностями
            for(uint32_t idx_column_delete= 0u; idx_column_delete < c_copy.front().size(); ++idx_column_delete)
                c_copy[idx_row][idx_column_delete] = std::numeric_limits<double>::infinity();
        }
        //зачеркивание строки
        else
        {
            //запись значения элемента вектора bi в матрицу x
            transport_reference[idx_row][idx_column] = vector_bi_copy[idx_column];

            //вычитание значения элемента вектора bi из элемента вектора ai
            vector_ai_copy[idx_row]-=vector_bi_copy[idx_column];

            //зануление элемента вектора bi
            vector_bi_copy[idx_column]=0.;

            //заполнение строки матрицы c бесконечностями
            for(uint32_t idx_row_delete= 0u; idx_row_delete < c_copy.size(); ++idx_row_delete)
                c_copy[idx_row_delete][idx_column] = std::numeric_limits<double>::infinity();
        }
    }

    //проверка наличия решения
    if(flag_ok)
        return QString();
    else
    {
        return QString("Решение не удалось найти");
    }
}

//реализация геттеров класса tools
void tools::get_matrixes(QVector<QVector<double>>& c,
                  QVector<QVector<double>>& x,
                  QVector<QVector<double>>& res)
{
    //получение матрицы с
    c = tariff_traffic;
    //получение матрицы x
    x = matrix_solve::matrix_extract(transport_reference);
    //получение матрицы результата оптимизации
    res = matrix_solve::matrix_extract(result_optimization);
}

void tools::get_matrixes_opt(QVector<QVector<double>>& c,
                  QVector<QVector<std::optional<double>>>& x,
                  QVector<QVector<std::optional<double>>>& res)
{
    //получение матрицы с
    c = tariff_traffic;
    //получение матрицы x типа optional
    x = transport_reference;
    //получение матрицы результата оптимизации типа optional
    res = result_optimization;
}

void tools::get_pack_x_and_sums_traffic(QVector<QVector<QVector<std::optional<double>>>> &vector_transport_reference0,
                                 QVector<double> &vector_sum_traffic0)
{
    //получение полей
    vector_transport_reference0 = vector_transport_reference;
    vector_sum_traffic0 = vector_sum_traffic;
}

//реализация метода выполнения одной итерации метода потенциалов
QString tools::method_potential(bool &need_optimization)
{
    //проверка на наличие пустоты в хотя бы одной из матриц
    if(!tariff_traffic.size() || !transport_reference.size())
        return QString("Одна из матриц пустая");

    if(!tariff_traffic.front().size() || !transport_reference.front().size())
        return QString("Одна из матриц пустая");

    //подсчет стоимости перевозок по выбранному плану
    double sum_traffic = matrix_solve::calculate_sum_price(transport_reference,
                                                           tariff_traffic,
                                                           vector_pi,
                                                           adding_pi);

    //добавление стоимости перевозок по выбранному плану в контейнер сумм
    vector_sum_traffic.push_back(sum_traffic);

    //добавление текущей матрицы x в контейнер матриц
    vector_transport_reference.push_back(transport_reference);

    //инициализация матрицы результата оптимизации пустотами
    result_optimization = QVector<QVector<std::optional<double>>>(transport_reference.size(),
                          QVector<std::optional<double>>(transport_reference.front().size(),
                          std::nullopt));

    //обход по матрице x
    for(uint32_t idx_row =0u; idx_row<transport_reference.size(); ++idx_row)
        for(uint32_t idx_column =0u; idx_column<transport_reference.front().size(); ++idx_column)
        {
            //проверка на наличие значения в ячейке
            if(transport_reference[idx_row][idx_column])
                result_optimization[idx_row][idx_column] = tariff_traffic[idx_row][idx_column];
        }

    uint32_t count_not_inf = 0u;

    //предикат проверки наличия значения в ячейке
    auto predicate = [](std::optional<double> value)
    {
        return value.has_value();
    };

    //подсчет количества значений не равных бесконечности
    for(const QVector<std::optional<double>>&line: result_optimization)
        count_not_inf+=std::count_if(std::begin(line), std::end(line), predicate);

    //подсчет количества потенциалов
    uint32_t count_u_and_v = result_optimization.size() + result_optimization.front().size();

    //создание матриц
    QVector<QVector<double>> matrix(count_not_inf, QVector<double>(count_u_and_v));
    QVector<double> matrix_addition(count_not_inf);

    uint32_t number_equation =0u;

    //обход по матрице x
    for(uint32_t idx_row =0u; idx_row<result_optimization.size(); ++idx_row)
        for(uint32_t idx_column =0u; idx_column<result_optimization.front().size(); ++idx_column)
            //проверка на наличие значения в ячейке
            if(predicate(result_optimization[idx_row][idx_column]))
            {
                //элемент матрицы matrix(u), соответствующий наличию значения в ячейке матрицы result_optimization
                matrix[number_equation][idx_row] = 1.;
                //элемент матрицы matrix(v), соответствующий наличию значения в ячейке матрицы result_optimization
                matrix[number_equation][result_optimization.size()+idx_column] = 1.;

                matrix_addition[number_equation] = result_optimization[idx_row][idx_column].value();

                ++number_equation;
            }

    int32_t idx_delete_column = -1;

    //удаление из матрицы столбика, так как уравнений больше, чем неизвестных
    QString err = matrix_solve::delete_column(matrix, idx_delete_column);

    if(!err.isEmpty())
        return err;

    QVector<double> result_u_and_v;

    //решение СЛАУ методом Крамера
    QString err_kramer = matrix_solve::solve_system_by_Kramer(matrix, matrix_addition, result_u_and_v);

    if(!err_kramer.isEmpty())
        return err_kramer;

    //вставка нуля вместо удаленного столбика
    result_u_and_v.insert(idx_delete_column + 1, 0.);

    //запись результата result_u_and_v отдельно для u, и отдельно для v
    QVector<double> result_u(result_optimization.size());
    QVector<double> result_v(result_optimization.front().size());

    for(uint32_t idx=0u; idx< result_optimization.size(); ++idx)
        result_u[idx]= result_u_and_v[idx];

    for(uint32_t idx=0u; idx< result_optimization.front().size(); ++idx)
        result_v[idx]= result_u_and_v[idx+result_optimization.size()];

    //заполнение пустух ячеек матрицы result_optimization
    for(uint32_t idx_row=0u; idx_row< result_optimization.size(); ++idx_row)
        for(uint32_t idx_col=0u; idx_col< result_optimization.front().size(); ++idx_col)
        {
            std::optional<double> &value_opt = result_optimization[idx_row][idx_col];

            if(!value_opt.has_value())
            {
                value_opt = result_u[idx_row]+result_v[idx_col];
            }
            else
            {
                if(std::abs(value_opt.value() - result_u[idx_row]-result_v[idx_col])> 1e-6)
                    return QString("Сумма потенциалов не равна соответствующей ячейке матрицы оптимизации");
            }
        }

    //приведение матрицы с optional в тип без optional
    QVector<QVector<double>> result_optimization_not_optional = matrix_solve::matrix_extract(result_optimization);

    if(result_optimization_not_optional.isEmpty())
        return QString("Матрица result_optimization содержит хотя бы одно пустое значение");

    //поиск разниц матриц
    QVector<QVector<double>> matrix_c_div_result = matrix_solve::div_matrix(tariff_traffic,result_optimization_not_optional);

    //если матрица имеет хотя бы один отрицательный потенциал, то необходимо повторить цикл оптимазации
    if(matrix_solve::matrix_has_negative(matrix_c_div_result))
    {
        //поиск координат отрицательного элемента с самым минимальным по модулю значением
        auto[row_fiction, column_fiction] = matrix_solve::get_coords_very_min_element(matrix_c_div_result);

        if(transport_reference[row_fiction][column_fiction].has_value())
        {
            return QString("Была предпринята попытка записи фиктивного значения в непустую клетку");
        }
        transport_reference[row_fiction][column_fiction] = 0.;

        //создание графа по матрице x
        graph graph_matrix_x(transport_reference);

        //поиск всех возможных циклов в графе
        graph_matrix_x.cycles_search();

        //исключение циклов, неподходящих для оптимизации
        graph_matrix_x.use_all_filters();

        //если после исключения циклов ни одного из них не осталось в контейнере
        if(!graph_matrix_x.count_cycles_in_graph())
        {
            return QString("Матрица имеет хотя бы одно отрицательное значение, но не удалось найти циклы "
                           "для оптимизации опорного плана");
        }

        //получение циклов в виде координат их узлов
        QVector<QVector<std::pair<uint32_t, uint32_t>>> nodes_chains_cycles =
                graph_matrix_x.get_nodes_chains_cycles();

        //получение циклов, начинающихся в узле с заданными координатами
        QVector<QVector<std::pair<uint32_t, uint32_t>>> cycles_for_start_node_coords =
                graph_matrix_x.find_cycles_for_start_node_coords
                (row_fiction, column_fiction);

        if(cycles_for_start_node_coords.isEmpty())
            return QString("Не найдено циклов оптимизации опорного плана");

        if(cycles_for_start_node_coords.size()!=1)
        {
            return QString("Найдено больше одного цикла оптимизации опорного плана");
        }

        //формирование маски, в которой задается цикл смещения для оптимизации плана
        QVector<QVector<double>> mask = setting_signals_in_transport_reference
                                        (cycles_for_start_node_coords.front());

        if(!matrix_solve::check_matrix_sum_only_null_in_rows_and_in_columns(mask))
        {
            return QString("Сумма знаков по строкам или столбикам не равна нулю");
        }

        //поиск координат минимального элемента в матрице x среди значений, соответствующих значению -1 по маске
        std::pair<uint32_t, uint32_t> coord_min_element_has_minus = matrix_solve::get_coords_min_element_has_minus_in_mask
                                                        (transport_reference, mask);

        //поиск дельты
        double delta;

        if(transport_reference[coord_min_element_has_minus.first][coord_min_element_has_minus.second].has_value())
            delta = transport_reference[coord_min_element_has_minus.first][coord_min_element_has_minus.second].value();
        else
            return QString("В маске -1 указало в пустую клетку матрицы x");

        //оптимизация плана по циклу
        matrix_solve::optimization_matrix_by_cycle_permutation
                (transport_reference, mask, delta);

        //проверка корректности плана после оптимизации
        QString check_result = check_reference_plan();

        if(!check_result.isEmpty())
        {
            return QString("В ходе оптимизации по методу потенциалов был получен некорректный опорный план"
                           + check_result);
        }

        //если в двух ближайших итерациях матрица одинакова, то оптимизация прекращается
        bool eq_matrix_transport_reference = true;
        QString err_cmp = matrix_solve::matrix_comparator
                (transport_reference,
                 vector_transport_reference.back(),
                 eq_matrix_transport_reference);
        if(!err_cmp.isEmpty())
        {
            return err_cmp;
        }

        //перемещение нуля по циклу оптимизации при повторении матрицы
        if(eq_matrix_transport_reference)
        {
            if(std::abs(delta) > 1e-6)
            {
                need_optimization = false;
                return QString();
            }
            else
            {
                //перемещение нуля
                QString err_move = matrix_solve::move_null
                        (transport_reference, mask, delta);
                if(!err_move.isEmpty())
                {
                    return err_move;
                }
            }
        }

        //необходима еще оптимизация плана
        need_optimization = true;
    }
    else
    {
        //план оптимизирован
        need_optimization = false;
    }

    return QString();
}

//реализация метода оптимизации опорного плана по методу потенциалов
QString tools::method_potential_optimization(uint32_t &count_iter)
{
    //флаг необходимости дальнейшей оптимизации
    bool need_optimization = true;

    for(;;++count_iter)
    {
        //вызов метода, выполняющий одну итерацию оптимизации плана
        QString err = method_potential(need_optimization);

        //выход при ошибке
        if(!err.isEmpty())
            return err;
        //выход из цикла при завершении оптимизации(все потенциалы положительны)
        if(!need_optimization)
            break;
    }
    return QString();
}

//реализация метода расстановки знаков в цикле оптимизации
QVector<QVector<double>> tools::setting_signals_in_transport_reference(const QVector<std::pair<uint32_t, uint32_t>> &cycle)
{
    //создание маски
    QVector<QVector<double>> mask(transport_reference.size(),
                                  QVector<double>(transport_reference.front().size()));

    double k = 1.;

    //расстановка знаков в маске в соотвествии с выбранным планом оптимизации
    for(uint32_t idx_node =0u; idx_node < cycle.size(); ++idx_node)
    {
        //поиск координат текущего узла в цикле
        std::pair<uint32_t, uint32_t> coord = cycle[idx_node];
        mask[coord.first][coord.second] = k;
        k*=-1.;
    }

    return mask;
}

//реализация метода приведения вещественного значения к строке
std::string tools::double_to_string(double value)
{
    //создание экземпляра строкового потока
    std::ostringstream oss;

    //помещение значения в поток
    oss << value;

    //представление потока в виде строки
    std::string result = oss.str();

    return result;
}
