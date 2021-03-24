#include "graph.h"

//Реализация методов класса node

//реализация конструкторов класса node
node::node():
    idx_row(0u), idx_column(0u), value(0.), contacts(QVector<std::pair<uint32_t,uint32_t>>())
{}

node::node(uint32_t idx_row0, uint32_t idx_column0, double value0, const QVector<std::pair<uint32_t,uint32_t>>& contacts0):
    idx_row(idx_row0), idx_column(idx_column0), value(value0), contacts(contacts0)
{}

//реализация геттеров класса node
QVector<std::pair<uint32_t, uint32_t>> node::get_neighbors() const
{
    return contacts;
}

std::pair<uint32_t, uint32_t> node::get_coords() const
{
    return {idx_row, idx_column};
}

//Реализация методов класса edge

//реализация конструкторов класса edge
edge::edge():
    v1(0u),v2(0u)
{}

edge::edge(uint32_t v1_input, uint32_t v2_input):
    v1(v1_input), v2(v2_input)
{}

//реализация геттеров класса edge
void edge::get_all(uint32_t &v1_output, uint32_t &v2_output) const
{
    v1_output = v1;
    v2_output = v2;
}

uint32_t edge::get_v1() const
{
    return v1;
}

uint32_t edge::get_v2() const
{
    return v2;
}

//Реализация методов класса graph

//реализация конструкторов класса graph
graph::graph()
{}

graph::graph(const QVector<QVector<std::optional<double>>> &matrix0,
             bool calculate_nodes_edges_search):
    matrix(matrix0)
{
    if(calculate_nodes_edges_search)
    {
        create_nodes();

        create_edges();
    }
}

//реализация метода заполнения контейнера узлов по матрице
void graph::create_nodes()
{
    //инициализация контейнера узлов
    nodes = QVector<node>(matrix.size()*matrix.front().size());
    uint32_t idx_nodes = 0u;

    //обход матрицы и заполнение по ней контейнера узлов
    for(uint32_t idx_row =0u; idx_row<matrix.size();++idx_row)
        for(uint32_t idx_column =0u; idx_column<matrix.front().size(); ++idx_column)
        {
            //выгрузка текущего значения из матрицы
            std::optional<double> value_opt = matrix[idx_row][idx_column];
            //если ячейка опорного плана содержит значение
            if(value_opt.has_value())
            {
                //объявление контейнера координат соседей текущего узла
                QVector<std::pair<uint32_t,uint32_t>> neighbors;
                //поиск соседей по строчке
                for(uint32_t idx_c=0u; idx_c<matrix.front().size(); ++idx_c)
                {
                    //если сосед нашелся, то он добавляется в соответствующий контейнер
                    if(idx_c!=idx_column && matrix[idx_row][idx_c].has_value())
                        neighbors.push_back({idx_row, idx_c});
                }

                //поиск соседей по столбику
                for(uint32_t idx_r=0u; idx_r<matrix.size(); ++idx_r)
                {
                    //если сосед нашелся, то он добавляется в соответствующий контейнер
                    if(idx_r!=idx_row && matrix[idx_r][idx_column].has_value())
                        neighbors.push_back({idx_r, idx_column});
                }

                //создание узла на основе найденных данных
                node node_now(idx_row, idx_column, value_opt.value(),neighbors);
                //помещение узла в контейнер узлов
                nodes[idx_nodes] = node_now;
                ++idx_nodes;
            }
        }
    //удаление неиспользованных ячеек в контейнере nodes
    nodes.erase(std::begin(nodes)+idx_nodes, std::end(nodes));
}

//реализация метода заполнения контейнера рёбер по матрице
void graph::create_edges()
{
    uint32_t idx_node =0u;

    //обход контейнера узлов
    for(const node &n: nodes)
    {
        QVector<std::pair<uint32_t, uint32_t>> neighbors_pack = n.get_neighbors();

        //обход соседей по их координатам
        for(const std::pair<uint32_t, uint32_t> &coord: neighbors_pack)
        {
            //создание ребра и помещение его в соответствующий контейнер
            QVector<node>::iterator it = find_iterator_node_from_coords(coord);

            uint32_t idx_neighbor = std::distance(std::begin(nodes), it);

            edge edge_now(idx_node, idx_neighbor);

            edges.push_back(edge_now);
        }
        ++idx_node;
    }
}

//реализация метода поиска итератора на элемент контейнера nodes по координатам узла
QVector<node>::iterator graph::find_iterator_node_from_coords(const std::pair<uint32_t,uint32_t> &coord)
{
    //обход контейнера nodes по итератору
    for(QVector<node>::iterator it = std::begin(nodes); it!=std::end(nodes); ++it)
    {
        std::pair<uint32_t, uint32_t> coord_now = (*it).get_coords();

        //если найден узел с заданными координатами, то возвращается итератор на него
        if(coord_now.first == coord.first && coord_now.second == coord.second)
            return it;
    }
}

//реализация метода поиска циклов в графе
void graph::cycles_search()
{
    //контейнеров окрашенивания узлов
    QVector<int> color(nodes.size());

    //обход узлов контейнера nodes
    for (uint32_t idx = 0u; idx < nodes.size(); ++idx)
    {
        for (uint32_t k = 0u; k < nodes.size(); ++k)
            color[k] = 1;

        //создание контейнера для описания цикла в графе
        QVector<int> cycle;

        cycle.push_back(idx);

        //вызов рекурсивного метода поиска цикла в графе
        DFScycle(idx, idx, edges, color, -1, cycle);
    }
}

//реализация метода поиска цикла в графе
void graph::DFScycle(int u, int endV, const QVector<edge> &E, QVector<int> color, int unavailableEdge, QVector<int> cycle)
{
    //проверка необходимости перекрашивания вершины
    if (u != endV)
        color[u] = 2;
    //необходимо перекрасить
    else if (cycle.size() >= 2)
    {
        bool flag_forward = false;

        //обход циклов на текущей итерации рекурсии
        for(const QVector<int> &chain: chains)
        {
            if(chain == cycle)
            {
                flag_forward = true;
                break;
            }
        }

        //проверка на палиндром и удаление его при обнаружении
        QVector<int> cycle_reverse = cycle;

        std::reverse(std::begin(cycle_reverse), std::end(cycle_reverse));

        bool flag_pol = false;

        for(const QVector<int> &chain: chains)
        {
            if(chain == cycle_reverse)
            {
                flag_pol = true;
                break;
            }
        }

        //если цикл не является палиндромом в отношении циклов, которые уже имеются, то сохраняем его
        if (!flag_pol && !flag_forward)
        {
            chains.push_back(cycle);
        }
        return;
    }

    //обход по ребрам
    for (int w = 0; w < E.size(); w++)
    {
        if (w == unavailableEdge)
            continue;
        //закрашивание вершин
        if (color[E[w].get_v2()] == 1 && E[w].get_v1() == u)
        {
            QVector<int> cycleNEW = cycle;
            cycleNEW.push_back(E[w].get_v2());

            DFScycle(E[w].get_v2(), endV, E, color, w, cycleNEW);
            color[E[w].get_v2()] = 1;
        }
        else if (color[E[w].get_v1()] == 1 && E[w].get_v2() == u)
        {
            QVector<int> cycleNEW = cycle;
            cycleNEW.push_back(E[w].get_v1());

            DFScycle(E[w].get_v1(), endV, E, color, w, cycleNEW);
            color[E[w].get_v1()] = 1;
        }
    }

}

//реализация метода использования всех фильтров
void graph::use_all_filters()
{
    filter_chain_shot_3();

    filter_chain_only_one_line_or_column();

    filter_chain_jumping();

    filter_chain_mirror();
}

//реализация метода исключения коротких циклов
void graph::filter_chain_shot_3()
{
    //предикат обнаружения коротких циклов
    auto predicate_delete = [](const QVector<int> &chain)
    {
        return chain.size() == 3 && chain.front() == chain.back();
    };

    //поиск и удаление из контейнера коротких циклов
    auto it_new_end = std::remove_if(std::begin(chains), std::end(chains), predicate_delete);
    chains.erase(it_new_end, std::end(chains));
}

//реализация метода исключения циклов, которые располагаются только в одной строке или в одном столбике матрицы
void graph::filter_chain_only_one_line_or_column()
{
    //предикат обнаружения циклов, которые требуется удалить
    auto predicate_delete = [this](const QVector<int> &chain)
    {
        //получение первого узла в цикле
        uint32_t idx_first_in_chain = chain.front();

        node n_first = nodes[idx_first_in_chain];

        std::pair<uint32_t, uint32_t> coord_first = n_first.get_coords();

        const uint32_t idx_row = coord_first.first;
        const uint32_t idx_column = coord_first.second;

        //проверка цикла на существование его только в одном столбце или только в одной строке
        bool save_row = true;
        bool save_column = true;
        bool save_line = true;

        for(int node_idx: chain)
        {
            node n = nodes[node_idx];
            std::pair<uint32_t, uint32_t> coord = n.get_coords();

            if(coord.first != idx_row)
            {
                save_row = false;
            }

            if(coord.second != idx_column)
            {
                save_column = false;
            }

            if(!save_row && !save_column)
            {
                save_line = false;
                break;
            }
        }
        return save_line;
    };

    //поиск по предикате и удаление циклов, располагающихся только в одной строке или только в одном столбце
    auto it_new_end = std::remove_if(std::begin(chains), std::end(chains), predicate_delete);
    chains.erase(it_new_end, std::end(chains));
}

//реализации метода исключения петлистых циклов
void graph::filter_chain_jumping()
{
    //предикат нахождения петлистых циклов
    auto predicate_delete = [this](const QVector<int> &chain)
    {
        QVector<uint32_t> rows;
        QVector<uint32_t> columns;

        //получение координат из циклов и представление их в виде контейнеров строк и столбцов
        for(int node_idx: chain)
        {
            node now_node_in_chain = nodes[node_idx];

            std::pair<uint32_t, uint32_t> coord_now_node = now_node_in_chain.get_coords();

            auto [idx_row_now, idx_column_now] = coord_now_node;

            rows.push_back(idx_row_now);
            columns.push_back(idx_column_now);
        }

        //проверка возрастания индексов столбиков при одинаковой строке
        bool has_jump = false;
        QVector<uint32_t> sequence;
        std::optional<uint32_t> row_old;

        for(uint32_t idx_node = 0u; idx_node < rows.size(); ++idx_node)
        {
            //если не имеется предыдущего индекса строки, то записать индекс столбика и
            //старый индекс инициализируется текущим индексом
            if(!row_old.has_value())
            {
                sequence.push_back(columns[idx_node]);
                row_old = rows[idx_node];
                continue;
            }
            //если предыдущий индекс строки равен текущему, то записать индекс столбика
            if(row_old.value() == rows[idx_node])
                sequence.push_back(columns[idx_node]);
            //если предыдущий индекс строки не равен текущему, то проверяем индексы столбиков на монотонное убывание/возрастание
            else
            {
                if(!std::is_sorted(std::begin(sequence), std::end(sequence)) &&
                    !std::is_sorted(std::begin(sequence), std::end(sequence), std::greater<uint32_t>()))
                {
                    has_jump = true;
                    break;
                }
                sequence.clear();
                sequence.push_back(columns[idx_node]);
            }
            row_old = rows[idx_node];
        }
        //если набор индексов столбиков не пустой, то проверяем индексы столбиков на монотонное убывание/возрастание
        if(!sequence.isEmpty())
        {
            if(!std::is_sorted(std::begin(sequence), std::end(sequence)) &&
                !std::is_sorted(std::begin(sequence), std::end(sequence), std::greater<uint32_t>()))
            {
                has_jump = true;
            }
        }
        //если обнаружен хотя бы один прыжок в текущем цикле, то его удаляют
        if(has_jump)
            return true;

        //проверка возрастания индексов строчек при одинаковом столбике
        has_jump = false;
        sequence.clear();
        std::optional<uint32_t> column_old;

        for(uint32_t idx_node = 0u; idx_node < columns.size(); ++idx_node)
        {
            //если не имеется предыдущего индекса столбика, то записать индекс строки и
            //старый индекс инициализируется текущим индексом
            if(!column_old.has_value())
            {
                sequence.push_back(rows[idx_node]);
                column_old = columns[idx_node];
                continue;
            }
            //если предыдущий индекс столбика равен текущему, то записать индекс строки
            if(column_old.value() == columns[idx_node] )
                sequence.push_back(rows[idx_node]);
            //если предыдущий индекс столбика не равен текущему, то проверяем индексы строк на монотонное убывание/возрастание
            else
            {
                if(!std::is_sorted(std::begin(sequence), std::end(sequence)) &&
                    !std::is_sorted(std::begin(sequence), std::end(sequence), std::greater<uint32_t>()))
                {
                    has_jump = true;
                    break;
                }
                sequence.clear();
                sequence.push_back(rows[idx_node]);
            }
            column_old = columns[idx_node];
        }
        //если набор индексов строки не пустой, то проверяем индексы строк на монотонное убывание/возрастание
        if(!sequence.isEmpty())
        {
            if(!std::is_sorted(std::begin(sequence), std::end(sequence)) &&
                !std::is_sorted(std::begin(sequence), std::end(sequence), std::greater<uint32_t>()))
            {
                has_jump = true;
            }
        }

        return has_jump;
    };

    //поиск по предикате и удаление циклов, содеражщих прыжки
    auto it_new_end = std::remove_if(std::begin(chains), std::end(chains), predicate_delete);
    chains.erase(it_new_end, std::end(chains));
}

//реализация метода исключения циклов, которые являются палиндромами относительно друг друга
void graph::filter_chain_mirror()
{
    //предикат обнаружения зеркальных циклов
    auto predicate_delete = [this](const QVector<int> &chain)
    {
        QVector<uint32_t> rows_entry(matrix.size());
        QVector<uint32_t> columns_entry(matrix.front().size());

        //подсчет по столбикам и по строкам количества узлов, входящих в цикл
        for(uint32_t idx =0u; idx < chain.size() - 1; ++idx)
        {
            uint32_t node_idx = chain[idx];
            node n = nodes[node_idx];
            std::pair<uint32_t, uint32_t> coord = n.get_coords();
            auto[idx_row, idx_column] = coord;

            ++rows_entry[idx_row];
            ++columns_entry[idx_column];
        }
        //предикат проверки четности
        auto is_odd = [](uint32_t value)
        {
            return !(value&1u);
        };

        //проверка четности количества по строкам и по столбцам узлов цикла
        uint32_t count_odd_in_rows_entry = std::count_if(std::begin(rows_entry), std::end(rows_entry), is_odd);
        uint32_t count_odd_in_columns_entry = std::count_if(std::begin(columns_entry), std::end(columns_entry), is_odd);

        return count_odd_in_rows_entry != rows_entry.size() ||
               count_odd_in_columns_entry != columns_entry.size();
    };

    //поиск по предикате и удаление незеркальных циклов
    auto it_new_end = std::remove_if(std::begin(chains), std::end(chains), predicate_delete);
    chains.erase(it_new_end, std::end(chains));
}

//реализация геттера количества циклов в графе
uint32_t graph::count_cycles_in_graph()
{
    return chains.size();
}

//реализация геттера циклов, в которых узлы представлены координатами
QVector<QVector<std::pair<uint32_t, uint32_t>>> graph::get_nodes_chains_cycles()
{
    //создание контейнера циклов для возврата
    QVector<QVector<std::pair<uint32_t, uint32_t>>> result_cycles =
            QVector<QVector<std::pair<uint32_t, uint32_t>>>(chains.size());
    //представление контейнера узлов в виде контейнера их координат
    for(uint32_t idx_row = 0u; idx_row < chains.size(); ++idx_row)
    {
        QVector<std::pair<uint32_t, uint32_t>> coords_nodes_in_now_chain;
        for(int idx_nodes_in_now_chain : chains[idx_row])
        {
            node n = nodes[idx_nodes_in_now_chain];
            std::pair<uint32_t, uint32_t> coords_n = n.get_coords();
            coords_nodes_in_now_chain.push_back(coords_n);
        }
        result_cycles[idx_row] = coords_nodes_in_now_chain;
    }
    return result_cycles;
}

//реализация метода поиска цикла, который начинается узлом с заданными координатами
QVector<QVector<std::pair<uint32_t, uint32_t>>> graph::find_cycles_for_start_node_coords
(uint32_t start_row, uint32_t start_column)
{
    //объявление контейнера для сохранения искомого цикла
    QVector<QVector<std::pair<uint32_t, uint32_t>>> result;
    //обход найденных циклов
    for(const QVector<int> &chain : chains)
    {
        node n_front = nodes[chain.front()];
        std::pair<uint32_t, uint32_t> coords = n_front.get_coords();
        //если координаты первого узла совпадают с заданными
        if(coords.first == start_row && coords.second == start_column)
        {
            QVector<std::pair<uint32_t, uint32_t>> coords_nodes_in_now_chain;
            //представление подходящего цикла в виде последовательности координат узлов
            for(int idx_nodes_in_now_chain : chain)
            {
                node n = nodes[idx_nodes_in_now_chain];
                std::pair<uint32_t, uint32_t> coords_n = n.get_coords();
                coords_nodes_in_now_chain.push_back(coords_n);
            }
            result.push_back(coords_nodes_in_now_chain);
        }
    }
    return result;
}
