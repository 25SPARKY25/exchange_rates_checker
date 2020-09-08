#include "valutechecker.h"

ValuteChecker::ValuteChecker(QObject *parent) : QObject(parent)
{

}

ValuteChecker::ValuteChecker(QSqlDatabase &db)
{
    currentDay = QDate::currentDate().toString("dd.MM.yyyy"); //получаем текущий день
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ValuteChecker::fileIsReady, Qt::DirectConnection);
    connect(this, &ValuteChecker::done, &loop, &QEventLoop::quit);
    xml = new QXmlStreamReader();
    this->db = db;
    isOpen = this->db.open();
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &ValuteChecker::checkDay);
    connect(this, &ValuteChecker::On_dayChanged, this, &ValuteChecker::checkCurs);
    timer->start(1000);
    checkCurs();
}

void ValuteChecker::fillValCurs()
{
    if(isOpen)
    {
        QString str = "http://www.cbr.ru/scripts/XML_daily.asp?date_req=" + currentDay;
        manager->get(QNetworkRequest(QUrl(str)));
        QSqlQuery q(db);
        QString query;

        loop.exec();


        if(!map.isEmpty())
        {
            QVariant val;
            QString dateFromPage;
            dateFromPage = map.last().value(0);

            if(dateFromPage==currentDay)
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
                    dateFromBD = dateFromPage; //в случае успешного добавления записей обновляем дату из бд
                }
            }
        }
    }
}

void ValuteChecker::checkCurs()
{   
    if(isOpen)
    {
        if(dateFromBD.isEmpty()) //если дата из бд пустая то пытаемся получить её
        {
            q.exec("SELECT MAX(\"Date\") FROM public.\"TVALCURS\";"); //получаем дату последнего добавления курса валют
            while(q.next())
            {
                dateFromBD = q.value(0).toDate().toString("dd.MM.yyyy");
            }
        }

        if(dateFromBD!=currentDay || dateFromBD.isEmpty()) //если даты разные или не нету записей в бд то пытаемся добавить их
        {
            fillValCurs();
        }
    }
}

void ValuteChecker::checkDay()
{
    if(currentDay!=QDate::currentDate().toString("dd.MM.yyyy"))
    {
        currentDay = QDate::currentDate().toString("dd.MM.yyyy");
        emit On_dayChanged();
    }
}

void ValuteChecker::fileIsReady(QNetworkReply *reply)
{
    xml->clear();
    xml->addData(reply->readAll());
    readXml(*xml, map);
    emit done();
}
