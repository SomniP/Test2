//страж включения
#ifndef GRAPH_H
#define GRAPH_H

//подключение библиотек
#include <QVector>
#include <optional>
#include <QDebug>

//класс узел
class node
{
public:
    //конструкторы класса
    node();
    node(uint32_t idx_row0, uint32_t idx_column0, double value0, const QVector<std::pair<uint32_t,uint32_t>>& contacts0);

    //геттеры класса
    QVector<std::pair<uint32_t, uint32_t>> get_neighbors() const;
    
    std::pair<uint32_t, uint32_t> get_coords() const;

private:
    //индексы расположения узла в матрице плана
    uint32_t idx_row;
    uint32_t idx_column;

    //значение, хранимое в узле
    double value;

    //контейнер с перечислением координат соседей текущего узла
    QVector<std::pair<uint32_t,uint32_t>> contacts;
};

//класс ребро
class edge
{
public:
    //конструкторы класса
    edge();
    edge(uint32_t v1_input, uint32_t v2_input);

    //геттеры класса
    void get_all(uint32_t &v1_output, uint32_t &v2_output) const;
    uint32_t get_v1() const;
    uint32_t get_v2() const;

private:
    //индексы соединяемых узлов
     uint32_t v1;
     uint32_t v2;
};

//класс граф
class graph
{
public:
    //конструкторы класса
    graph();
    graph(const QVector<QVector<std::optional<double>>> &matrix0,
          bool calculate_nodes_edges_search = true);

    //метод заполнения контейнера узлов
    void create_nodes();

    //метод заполения контейнера рёбер
    void create_edges();

    //метод нахождения узла по координатам
    node find_node_from_coords(const std::pair<uint32_t,uint32_t> &coord);

    //метод нахождения итератора узла по координатам
    QVector<node>::iterator find_iterator_node_from_coords(const std::pair<uint32_t,uint32_t> &coord);

    //метод нахождения всех возможных циклов в графе
    void cycles_search();

    //метод исключения коротких циклов
    void filter_chain_shot_3();

    //метод исключения циклов, которые располагаются только в одной строке или в одном столбике матрицы
    void filter_chain_only_one_line_or_column();

    //метод исключения петлистых циклов
    void filter_chain_jumping();

    //метод исключения циклов, которые являются палиндромами относительно друг друга
    void filter_chain_mirror();

    //метод использования всех фильтров, объявленных выше
    void use_all_filters();

    //геттер, возвращающий количество найденных циклов в графе
    uint32_t count_cycles_in_graph();

    //геттер, возвращающий контейнер циклов, в которых каждый узел представляется координатами
    QVector<QVector<std::pair<uint32_t, uint32_t>>> get_nodes_chains_cycles();

    //геттер, возвращающий контейнер циклов, которые начинаются с узла, заданного посредством координат
    QVector<QVector<std::pair<uint32_t, uint32_t>>> find_cycles_for_start_node_coords
    (uint32_t start_row, uint32_t start_column);

private:
    //матрица, по которой строится граф
    QVector<QVector<std::optional<double>>> matrix;

    //контейнер узлов
    QVector<node> nodes;

    //контейнер рёбер
    QVector<edge> edges;

    //контейнер циклов графа
    QVector<QVector<int>> chains;

    //метод нахождения циклов в графе
    void DFScycle(int u, int endV, const QVector<edge> &E, QVector<int> color, int unavailableEdge, QVector<int> cycle);
};

#endif // GRAPH_H
