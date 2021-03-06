#ifndef VALUTECHECKER_H
#define VALUTECHECKER_H

#include <QObject>
#include <QDate>
#include <QTimer>
#include <QTime>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "queryhandler.h"
#include "xmlhandler.h"

class ValuteChecker : public QObject
{
    Q_OBJECT
public:

    ValuteChecker(QSqlDatabase &db);

private:
    explicit ValuteChecker(QObject *parent = nullptr);
    QTimer *timer;
    QString dateFromBD, currentDay;
    QSqlQuery q;
//    QVariant val;
    QSqlDatabase db;
    QEventLoop loop;
    QMap<QString, QStringList> map;
    QNetworkAccessManager *manager;
    QXmlStreamReader *xml;
    bool isOpen;
//    bool isWeekend{false}; //в выходные дни сайт не выкладывает курсы валют

    void fillValCurs();

private slots:
    void checkCurs();
    void checkDay();
    void fileIsReady(QNetworkReply *reply);

signals:
    void done();
    void On_dayChanged();

};

#endif // VALUTECHECKER_H
