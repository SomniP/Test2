//подключение главного окна
#include "mainwindow.h"

//подключение Qt библиотек
#include <QApplication>

//главная функция программы
int main(int argc, char *argv[])
{
    //объявление объекта приложения
    QApplication a(argc, argv);

    //объявление объекта главного окна
    MainWindow w;
    //отображение окна
    w.show();
    //установка заголовка окна
    w.setWindowTitle("Решение транспортной задачи (InSomnia & Софья)");

    return a.exec();
}
