//Подключение окон
#include "mainwindow.h"
#include "ui_mainwindow.h"

//Реализация конструкторов класса MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Инициализация объекта окна
    ui->setupUi(this);

    //Список возможных методов нахождения опорного плана
    QStringList modes_basic_plan
    {
        "Северо-западный угол",
        "Минимальный элемент"
    };
    ui->comboBox_basic_plan->addItems(modes_basic_plan);
    ui->comboBox_basic_plan->setCurrentIndex(0u);

    //Метод по умолчанию
    mode_cbp = NORTHWEST_CORNER;

    //Если есть бинарный файл с сохранённой конфигурацией,
    //то производится чтение данных и заполнение формы прошлыми значениями
    QFile file("binary_file_save.bin");

    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            bool flag_check_box_use_pi;
            int mode;
            QDataStream stream(&file);
            stream.setVersion(QDataStream::Qt_4_2);
            stream>>sources_count_number
                  >>customers_count_number
                  >>tariff_traffic
                  >>vector_ai
                  >>vector_bi
                  >>vector_pi
                  >>flag_check_box_use_pi
                  >>mode;
            ui->checkBox_use_pi->setChecked(flag_check_box_use_pi);
            ui->comboBox_basic_plan->setCurrentIndex(mode);
        }
    }
    else
    {
        sources_count_number = 3;
        customers_count_number = 4;
    }

    //Установка на форму значений колиства источников и потребителей
    ui->sources_count->setText(QString::number(sources_count_number));
    ui->customers_count->setText(QString::number(customers_count_number));

    model = new QStandardItemModel();

    set_table();

    //Если бинарный файл удалось прочесть, то создаётся на форме таблица
    if(file.exists())
        set_table_data();

    //Установка на форму таблицы
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    //Установка стилей отображения заголовков таблицы
    ui->tableView->horizontalHeader()->setStyleSheet(
      "QHeaderView::section {"
      "  border-width: 0.7px;"
      "  border-style: solid;"
      "  border-color: black;"
      "}");

    ui->tableView->verticalHeader()->setStyleSheet(
      "QHeaderView::section {"
      "  border-width: 0.7px;"
      "  border-style: solid;"
      "  border-color: black;"
      "}");

    file.close();
}

//Реализация деструктора класса MainWindow
MainWindow::~MainWindow()
{
    delete model;
    delete item;
    delete rsw;
    delete ui;
}

//Реализация метода приведения вещественного числа к строке
std::string MainWindow::double_to_string(double value)
{
    //Создание строкового потка
    std::ostringstream oss;

    //Помещение числа в поток
    oss << value;

    //Приведение потока к строковому типу
    std::string result = oss.str();

    //Возврат полученной строке
    return result;
}

//Реализация слота обработки сигнала снятия фокуса поля ввода количества источников
void MainWindow::on_sources_count_editingFinished()
{
    //Получение с формы количества источников в виде строки
    QString sources_count_str = ui->sources_count->text();

    //Проверка корректности ввода и преобразование строки в целое число
    bool flag_sources_count = false;

    int sources_count_number_copy = sources_count_number;

    //Парсинг строки в целочисленный тип данных
    sources_count_number = sources_count_str.toInt(&flag_sources_count);

    if(!flag_sources_count || sources_count_number <=0)
    {
        //Оповещение пользователя о некорректном вводе
        ui->sources_count->setText(QString::number(sources_count_number_copy));
        sources_count_number = sources_count_number_copy;
        QMessageBox::information(nullptr, "Предупреждение", "Введено не число, или оно отрицательное");
        return;
    }

    //Вызов метода создания таблицы
    set_table();
}

//Реализация слота обработки сигнала снятия фокуса поля ввода количества потребителей
void MainWindow::on_customers_count_editingFinished()
{
    //Получение с формы количества источников в виде строки
    QString customers_count_str = ui->customers_count->text();

    //Проверка корректности ввода и преобразование строки в целое число
    bool flag_customers_count = false;

    int customers_count_number_copy = customers_count_number;

    //Парсинг строки в целочисленный тип данных
    customers_count_number = customers_count_str.toInt(&flag_customers_count);

    if(!flag_customers_count || customers_count_number<=0)
    {
        //Оповещение пользователя о некорректном вводе
        ui->customers_count->setText(QString::number(customers_count_number_copy));
        customers_count_number = customers_count_number_copy;
        QMessageBox::information(nullptr, "Предупреждение", "Введено не число, или оно отрицательное");
        return;
    }

    //Вызов метода создания таблицы
    set_table();
}

//Реализация метода обработки сигнала нажатия на кнопку "Результат"
void MainWindow::on_enter_result_clicked()
{
    //Чтение значений таблицы с формы
    QVariant result = read_table();

    //Проверка успешности чтения
    if(result.canConvert<QString>())
    {
        QMessageBox::critical(nullptr, "Предупреждение", result.toString());
        return;
    }

    //Проверка наличия отрицательных значений в матрице c
    if(matrix_solve::matrix_has_negative(tariff_traffic))
    {
        QMessageBox::critical(nullptr, "Предупреждение", "Отрицательные значения недопустимы в матрице тарифов перевозок");
        return;
    }

    //Проверка наличия отрицательных значений в векторе значений ai
    if(matrix_solve::vector_has_negative(vector_ai))
    {
        QMessageBox::critical(nullptr, "Предупреждение", "Отрицательные значения недопустимы в векторе ai");
        return;
    }

    //Проверка наличия отрицательных значений в векторе значений bi
    if(matrix_solve::vector_has_negative(vector_bi))
    {
        QMessageBox::critical(nullptr, "Предупреждение", "Отрицательные значения недопустимы в векторе bi");
        return;
    }

    //Проверка наличия отрицательных значений в векторе значений pi
    if(matrix_solve::vector_has_negative(vector_pi))
    {
        QMessageBox::critical(nullptr, "Предупреждение", "Отрицательные значения недопустимы в векторе pi");
        return;
    }

    //Проверка состояния переключателя "Учитывать pi"
    QVector<QVector<double>> tariff_traffic_add;
    if(ui->checkBox_use_pi->isChecked())
    {
        tariff_traffic_add = matrix_solve::adding_vector_to_lines_matrix(tariff_traffic, vector_pi);
    }
    else
    {
        tariff_traffic_add = tariff_traffic;
    }

    //Инициализация объекта tool класса tools, который отвечает за построение и оптимизацию опорного плана
    tool = tools(tariff_traffic_add, vector_bi, vector_ai, vector_pi, ui->checkBox_use_pi->isChecked());

    //Вызов метода приведения задачи к закрытому виду
    tool.equalizer();

    //Запрос с формы режима построения опорного плана
    int int_mode_cbp = ui->comboBox_basic_plan->currentIndex();

    //Построение опорного плана
    //Используется метод Северо-Западного угла
    if(int_mode_cbp == NORTHWEST_CORNER)
    {
        tool.north_west_corner();
    }
    //Используется метод минимального элемента
    else if(int_mode_cbp == MINIMUM_ELEMENT)
    {
        QString min_elemnet_result = tool.min_element_matrix();

        if(!min_elemnet_result.isEmpty())
        {
            QMessageBox::critical(nullptr, "Что-то пошло не так",
                                  "Расчёт опорного плана выполнен некорретно. " + min_elemnet_result);
            return;
        }
    }
    //Если выбран индекс, который не соотвествует возможным методам
    else
    {
        QMessageBox::critical(nullptr, "Что-то пошло не так",
                              "Метод расчёта опорного плана не задан");
        return;
    }

    //Проверка корректности найденного опорного плана
    QString check_result = tool.check_reference_plan();

    //Обработка в случае некорректного плана
    if(!check_result.isEmpty())
    {
        QMessageBox::critical(nullptr, "Что-то пошло не так",
                              "Расчёт опорного плана выполнен некорретно. " + check_result);
        return;
    }

    //Получение и фиксация опрного плана
    QVector<QVector<double>> c_base;
    QVector<QVector<std::optional<double>>> x_base;
    QVector<QVector<std::optional<double>>> res_base;
    tool.get_matrixes_opt(c_base, x_base, res_base);

    //Вызов метода оптимизации опорного плана по методу потенциалов
    uint32_t count_iter = 0u;
    QString err = tool.method_potential_optimization(count_iter);

    //Обраотка в случае возникновения ошибки при работе метода
    if(!err.isEmpty())
    {
        QMessageBox::critical(nullptr, "Что-то пошло не так",
                              "Расчёт опорного плана выполнен некорретно. " + err);
        return;
    }

    //Фиксация контейнера матриц x в ходе решения и вектора значений сумм
    QVector<QVector<QVector<std::optional<double>>>> x_collection;
    QVector<double> sum_collection;
    tool.get_pack_x_and_sums_traffic(x_collection, sum_collection);

    //Запись в бинарный файл состояний полей формы
    QFile file("binary_file_save.bin");

    if(file.open(QIODevice::WriteOnly))
    {
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_4_2);
        stream<<sources_count_number
              <<customers_count_number
              <<tariff_traffic
              <<vector_ai
              <<vector_bi
              <<vector_pi
              <<ui->checkBox_use_pi->isChecked()
              <<ui->comboBox_basic_plan->currentIndex();

        file.close();
    }

    //Фиксайия конечного состояния плана перевозок
    QVector<QVector<double>> c_optimization;
    QVector<QVector<std::optional<double>>> x_optimization;
    QVector<QVector<std::optional<double>>> res_optimization;
    tool.get_matrixes_opt(c_optimization, x_optimization, res_optimization);

    //Инициалдизация окна результатов и передача ему найденных результатов
    rsw = new ResultShowWindow(x_base, x_optimization, sum_collection, x_collection);

    //Конфигурация и открытие окна результатов
    rsw->setWindowTitle("Результат расчёта");
    rsw->setModal(true);
    rsw->exec();
}

//Реализация метода создания таблицы
void MainWindow::set_table()
{
    //Очистка состояния модели
    model->clear();

    //Список горизонтальных подписей таблицы
    QStringList horizontal_header;
    for(uint32_t idx = 0u; idx < customers_count_number; ++idx)
    {
        horizontal_header.append(QString("B%1").arg(idx+1));
    }

    //Добавление подписей столбцов ai и pi
    horizontal_header.append("ai");
    horizontal_header.append("pi");

    //Список вертикальных подписей таблицы
    QStringList vertical_header;
    for(uint32_t idx = 0u; idx < sources_count_number; ++idx)
    {
        vertical_header.append(QString("A%1").arg(idx+1));
    }

    //Добавление подписей строки bi
    vertical_header.append("bi");

    //Установка на модель этих подписей (хедеров)
    model->setHorizontalHeaderLabels(horizontal_header);
    model->setVerticalHeaderLabels(vertical_header);

    //Простановка прочерков в нижних правых клетках таблицы
    item = new QStandardItem(QString("-"));
    model->setItem(sources_count_number, customers_count_number, item);
    item = new QStandardItem(QString("-"));
    model->setItem(sources_count_number, customers_count_number+1, item);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

//Реализация метода чтения данных из таблицы формы
QVariant MainWindow::read_table()
{
    //Инициализация матрицы c
    tariff_traffic = QVector<QVector<double>>(sources_count_number,
                                              QVector<double>(customers_count_number));

    //Чтение значений матрицы c с формы и помещение их в контейнер-поле объекта текущего окна
    for (uint32_t idx_row = 0u; idx_row < sources_count_number; ++idx_row)
    {
        for (uint32_t idx_column = 0u; idx_column < customers_count_number; ++idx_column)
        {
            QModelIndex qidx = ui->tableView->model()->index(idx_row,idx_column,QModelIndex());
            QVariant data = ui->tableView-> model()->data(qidx);
            bool flag = false;
            //Проверка корректности ввода числа и парсинг его из строки
            double value_double = data.toDouble(&flag);

            if(!flag)
            {
              return QString("Число не double в матрице тарифов перевозки");
            }
            tariff_traffic[idx_row][idx_column] = value_double;
        }
    }

    //Инициализация вектора значений bi
    vector_bi = QVector<double>(customers_count_number);

    //Чтение значений и заполнение ими вектора bi в поле текущего объекта (this)
    for(uint32_t idx_column = 0u; idx_column < customers_count_number; ++idx_column)
    {
        QModelIndex qidx = ui->tableView->model()->index(sources_count_number,idx_column,QModelIndex());
        QVariant data = ui->tableView-> model()->data(qidx);
        bool flag = false;
        //Проверка корректности ввода числа и парсинг его из строки
        double value_double = data.toDouble(&flag);

        if(!flag)
        {
          return QString("Число не double в векторе bi");
        }

        vector_bi[idx_column] = value_double;
    }

    //Инициализация вектора значений ai
    vector_ai = QVector<double>(sources_count_number);

    //Чтение значений и заполнение ими вектора ai в поле текущего объекта (this)
    for(uint32_t idx_row = 0u; idx_row < sources_count_number; ++idx_row)
    {
        QModelIndex qidx = ui->tableView->model()->index(idx_row,customers_count_number,QModelIndex());
        QVariant data = ui->tableView-> model()->data(qidx);
        bool flag = false;
        //Проверка корректности ввода числа и парсинг его из строки
        double value_double = data.toDouble(&flag);

        if(!flag)
        {
          return QString("Число не double в векторе ai");
        }

        vector_ai[idx_row] = value_double;
    }

    //Инициализация вектора значений pi
    vector_pi = QVector<double>(sources_count_number);

    //Чтение значений и заполнение ими вектора pi в поле текущего объекта (this)
    for(uint32_t idx_row = 0u; idx_row < sources_count_number; ++idx_row)
    {
        QModelIndex qidx = ui->tableView->model()->index(idx_row,customers_count_number+1,QModelIndex());
        QVariant data = ui->tableView-> model()->data(qidx);
        bool flag = false;
        //Проверка корректности ввода числа и парсинг его из строки
        double value_double = data.toDouble(&flag);

        if(!flag)
        {
          return QString("Число не double в векторе pi");
        }

        vector_pi[idx_row] = value_double;
    }
    return QVariant();
}

//Реализация метода заполнения таблицы значениями
void MainWindow::set_table_data()
{
    //Из поля объекта tariff_traffic значения вставляются в модель таблицы
    for(uint32_t idx_row = 0u; idx_row< tariff_traffic.size(); ++idx_row)
        for(uint32_t idx_column = 0u; idx_column< tariff_traffic.front().size(); ++idx_column)
        {
            double value = tariff_traffic[idx_row][idx_column];

            item = new QStandardItem(QString::number(value));

            model->setItem(idx_row, idx_column, item);
        }

    //Из поля объекта vector_bi значения вставляются в модель таблицы
    for(uint32_t idx_column = 0u; idx_column< customers_count_number; ++idx_column)
    {
        double value = vector_bi[idx_column];

        item = new QStandardItem(QString::number(value));

        model->setItem(sources_count_number, idx_column, item);
    }

    //Из поля объекта vector_ai и value_pi значения вставляются в модель таблицы
    for(uint32_t idx_row = 0u; idx_row< sources_count_number; ++idx_row)
    {
         double value_ai = vector_ai[idx_row];
         double value_pi = vector_pi[idx_row];

         item = new QStandardItem(QString::number(value_ai));
         model->setItem(idx_row, customers_count_number, item);

         item = new QStandardItem(QString::number(value_pi));
         model->setItem(idx_row, customers_count_number+1, item);
    }

    //Заполненнная модель прописывается в таблицу
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}
