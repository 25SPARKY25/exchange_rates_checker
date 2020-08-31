#include "valutechecker.h"

ValuteChecker::ValuteChecker(QObject *parent) : QObject(parent)
{

}

ValuteChecker::ValuteChecker(QSqlDatabase &db)
{
    date = QDate::currentDate().toString("dd.MM.yyyy");
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ValuteChecker::fileIsReady, Qt::DirectConnection);
    connect(this, &ValuteChecker::done, &loop, &QEventLoop::quit);
    xml = new QXmlStreamReader();
    this->db = db;
    isOpen = this->db.open();
    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &ValuteChecker::checkCurs);
    timer->start(1000);
}

void ValuteChecker::fillValCurs()
{
    if(isOpen)
    {
        QString str = "http://www.cbr.ru/scripts/XML_daily.asp?date_req=" + date;
        manager->get(QNetworkRequest(QUrl(str)));
        QSqlQuery q(db);
        QString query;

        loop.exec();


        if(map.size()!=0)
        {
            QVariant val;
            QString dateFromPage;
            dateFromPage = map.last().value(0);

            if(dateFromPage==date)
            {
                q.exec("SELECT COUNT(*) FROM public.\"TVALCURS\" WHERE \"Date\" = '" + dateFromPage + "';");
                while(q.next())
                {
                    val = q.value(0);
                }
                if(val.toInt()==0)
                {
                    query+="INSERT INTO public.\"TVALCURS\"(\"Date\", \"ID_VAL\", \"Value\") VALUES (?, ?, ?);";
                    q.prepare(query);

                    fillDB(q, map);
                }
            }
        }
    }
}

void ValuteChecker::checkCurs()
{
    if(isOpen)
    {
        q.exec("SELECT COUNT(*) FROM public.\"TVALCURS\" WHERE \"Date\" = '" + date + "';");
        while(q.next())
        {
            val = q.value(0);
        }
        if(val.toInt()==0)
        {
             fillValCurs();
        }
    }

}

void ValuteChecker::fileIsReady(QNetworkReply *reply)
{
    xml->clear();
    xml->addData(reply->readAll());
    readXml(*xml, map);
    emit done();
}
