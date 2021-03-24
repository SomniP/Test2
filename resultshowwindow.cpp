//Подключение окон
#include "resultshowwindow.h"
#include "ui_resultshowwindow.h"

//Реализация конструктора класса окна вывода результатов
ResultShowWindow::ResultShowWindow(const QVector<QVector<std::optional<double>>>& x_inp,
                                   const QVector<QVector<std::optional<double>>>& res_inp,
                                   const QVector<double> &vector_sum_traffic0,
                                   const QVector<QVector<QVector<std::optional<double>>>> &vector_transport_reference0,
                                   QWidget *parent):
    //Заполнение полей объекта класса переданными в конструктор значениями
    QDialog(parent),
    ui(new Ui::ResultShowWindow),
    x_opt(x_inp),
    res_opt(res_inp),
    vector_sum_traffic(vector_sum_traffic0),
    vector_transport_reference(vector_transport_reference0)
{
    //Инициализация окна
    ui->setupUi(this);

    //Аллоцирование на куче объектов моделей
    model_x = new QStandardItemModel();
    model_res = new QStandardItemModel();
    //Вызов методов создания заполнения таблиц опорного и конечного плана
    set_table(model_x, x_opt);
    set_table(model_res, res_opt);
    set_table_data(model_x, x_opt);
    set_table_data(model_res, res_opt);

    //Установка полученных моделей в таблицы на форму
    ui->tableView_basic_plan->setModel(model_x);
    ui->tableView_basic_plan->resizeColumnsToContents();
    ui->tableView_basic_plan->resizeRowsToContents();

    ui->tableView_res->setModel(model_res);
    ui->tableView_res->resizeColumnsToContents();
    ui->tableView_res->resizeRowsToContents();

    //Назначение стиля отображения границ хедеров таблиц
    ui->tableView_basic_plan->horizontalHeader()->setStyleSheet(
      "QHeaderView::section {"
      "  border-width: 0.7px;"
      "  border-style: solid;"
      "  border-color: black;"
      "}");
    ui->tableView_basic_plan->verticalHeader()->setStyleSheet(
      "QHeaderView::section {"
      "  border-width: 0.7px;"
      "  border-style: solid;"
      "  border-color: black;"
      "}");

    ui->tableView_res->horizontalHeader()->setStyleSheet(
      "QHeaderView::section {"
      "  border-width: 0.7px;"
      "  border-style: solid;"
      "  border-color: black;"
      "}");
    ui->tableView_res->verticalHeader()->setStyleSheet(
      "QHeaderView::section {"
      "  border-width: 0.7px;"
      "  border-style: solid;"
      "  border-color: black;"
      "}");

    //Объявление объекта моноширинного шрифта
    QFont fixedFont("monospace", 14);
    fixedFont.setStyleHint(QFont::Monospace);

    //Конфигурация окна вывода промежуточных расчётов
    ui->view_temperaly_calculation->setFont(fixedFont);

    //Формирование и вывод промежуточных расчётов (хода оптимизации опорного плана)
    for(uint32_t idx=0u; idx<vector_sum_traffic.size(); ++idx)
    {
        //Подпись текущей итерации
        ui->view_temperaly_calculation->setTextBackgroundColor(QColor(0,255,0));
        if(idx == 0u)
            ui->view_temperaly_calculation->append(QString("Опорный план оптимазации"));
        else
            ui->view_temperaly_calculation->append(QString("Итерация оптимазации "
                                                           + QString::number(idx)));

        ui->view_temperaly_calculation->setTextBackgroundColor(QColor(255,255,255));

        //Вывод матрицы x на текущей итерации оптимизации
        QVector<QVector<std::optional<double>>> m = vector_transport_reference[idx];

        for(const QVector<std::optional<double>> &line: m)
        {
            std::string line_matrix_str;

            for(const std::optional<double> &val_opt: line)
            {

                std::string value_str = val_opt.has_value() ? double_to_string(val_opt.value()) :
                                                              std::string("      -");
                line_matrix_str += value_str;

            }
            QString line_matrix_qstr = QString::fromStdString(line_matrix_str);
            ui->view_temperaly_calculation->append(line_matrix_qstr);
        }

        //Установка цвета вывода и простановка итоговой стоимости на текущей итарации оптимизации
        ui->view_temperaly_calculation->setTextColor(QColor(0,0,255));

        ui->view_temperaly_calculation->append(QString("Суммарная стоимость перевозок по данному плану составила: ")
                                               + QString::number(vector_sum_traffic[idx]));

        ui->view_temperaly_calculation->setTextColor(QColor(0,0,0));

        ui->view_temperaly_calculation->append(QString());

    }

    //Вывод на форму конечной стоимости перевозки по конечному плану
    double res_sum = vector_sum_traffic.back();
    QString res_sum_qstr = QString::number(res_sum);
    ui->result_price_line_edit->setText(res_sum_qstr);
    ui->result_price_line_edit->setEnabled(false);
}

//Реаплизация деструктора класса ResultShowWindow
ResultShowWindow::~ResultShowWindow()
{
    delete model_x;
    delete model_res;
    delete ui;
}

//Реализация метода приведения вещественного числа к строке
std::string ResultShowWindow::double_to_string(double value)
{
    //Объявление стрового потка
    std::ostringstream oss;

    //Вывод в поток значения с форматированием
    oss << std::setw(7) <<value;

    //Представление потока в форме строки
    std::string result = oss.str();

    //Возврат полученной строки
    return result;
}

//Реализация метода создания таблицы
void ResultShowWindow::set_table(QStandardItemModel *model,
                                 const QVector<QVector<std::optional<double>>>& matrix)
{
    //Очитска состояния модели таблицы
    model->clear();

    //Подписи горизонтальные и вертикальные
    QStringList horizontal_header;
    QStringList vertical_header;

    //Если матрица не пустая
    if(matrix.size())
    {
        //Из матрицы проставляются имена строк
        for(uint32_t idx = 0u; idx < matrix.front().size(); ++idx)
            horizontal_header.append(QString("B%1").arg(idx+1));

        //Из матрицы проставляются имена столбцов
        for(uint32_t idx = 0u; idx < matrix.size(); ++idx)
            vertical_header.append(QString("A%1").arg(idx+1));
    }

    //Хедеры устанавливаются на модель таблицы
    model->setHorizontalHeaderLabels(horizontal_header);
    model->setVerticalHeaderLabels(vertical_header);
}

//Реализация метода заполнения таблицы значениями
void ResultShowWindow::set_table_data(QStandardItemModel *model,
                                      const QVector<QVector<std::optional<double>>>& matrix)
{
    //Объявление указателя на объект класса ячейки таблицы
    QStandardItem *item;

    //Проход по матрице
    for(uint32_t idx_row = 0u; idx_row < matrix.size(); ++idx_row)
        for(uint32_t idx_column = 0u; idx_column < matrix.front().size(); ++idx_column)
        {
            //Получение значения элемента матрицы по индексам idx_row и idx_column
            std::optional<double> value_opt = matrix[idx_row][idx_column];

            //Если есть значение в текущей ячейке, оно выводится в таблицу
            if(value_opt.has_value())
                item = new QStandardItem(QString::number(value_opt.value()));
            //Иначе в ячейку таблицы ставится прочерк
            else
                item = new QStandardItem(QString ("-"));

            //Устанока на модель текущей сформированной ячейки
            model->setItem(idx_row, idx_column, item);
        }
}
