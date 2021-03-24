#include "mainwindow.h"
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
	//Ко создал программу
    w.setWindowTitle("Решение транспортной задачи (InSomnia & Софья)");

    return a.exec();
}
