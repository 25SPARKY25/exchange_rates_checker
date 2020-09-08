#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QEventLoop>
#include <QThread>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#include "valutechecker.h"

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

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_dateEdit_userDateChanged(const QDate &date);

signals:
  void  done(); //для event loop`а
  void fillcurs();


private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QXmlStreamReader *xml;
    QSqlDatabase db;
    bool dbIsOpen;
    QMap<QString, QStringList> map;
    QEventLoop loop;    //для подготовки xml перед отправкой обработчику sql-запросов
    QString date;
    ValuteChecker * vachk; //класс, который отвечает за ежедневную загрузку курса валют

    QThread *thr;

    QFuture<void> fut;
    QFutureWatcher<void> futWatch;


    void checkValDictionary();
    void fillValDIctionary();
    void fillValComboBox();
    void fillValCurs();
    double getProgrCurs(QString &datePr, QString &valutaPr); //программный метод получения курса заданной валюты
};
#endif // MAINWINDOW_H
