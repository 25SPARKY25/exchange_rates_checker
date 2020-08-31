#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define DEBUG 0

#include <QMainWindow>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>


#include "queryhandler.h"
#include "xmlhandler.h"

#if DEBUG<1
#include <QDebug>
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void fileIsReady(QNetworkReply *reply);

    void on_pb_fillValKurs_clicked();

    void on_pushButton_3_clicked();

signals:
  void  done(); //для event loop`а


private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QXmlStreamReader *xml;
    QSqlDatabase db;
    bool dbIsOpen;
    QMap<QString, QStringList> map;
    QEventLoop loop;    //для подготовки xml перед отправкой обработчику sql-запросов
    QString date;
    QTimer *timer;
    QThread *thr;

    QDate getCurrentTime();

    void checkValDictionary();
    void fillValDIctionary();
    void fillValComboBox();
    void fillValCurs();
    double getProgrCurs(QString &datePr, QString &valutaPr); //программный метод получения курса заданной валюты
};
#endif // MAINWINDOW_H
