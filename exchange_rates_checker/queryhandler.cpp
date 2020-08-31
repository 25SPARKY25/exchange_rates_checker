#include "queryhandler.h"


void fillDB(QSqlQuery &query, const QMap<QString, QStringList> &map, const bool &isDictionary)
{
    QString strQuery;
    auto itr = map.begin();
    if (isDictionary)
    {
        if(map.size()!=0)
        {
            while (itr!=map.end())
            {
                if(itr.key()!="Valuta")
                {
                    query.addBindValue(itr.key());
                    auto litr = itr.value().begin();
                    while(litr !=itr.value().end())
                    {
                        query.addBindValue(*litr);
                        ++litr;
                    }
                    query.exec();
                }                
                ++itr;
            }
        }
    }
    else
    {
        QString date, money;
        if(map.size()!=0)
        {
            date+= map.last().value(0); //получаем дату
            while (itr!=map.end())
            {
                if(itr.key()!="ValCurs")
                {
                    query.addBindValue(date);
                    query.addBindValue(itr.key());

                    money = itr.value().back();

                    for(auto mitr=money.begin(); mitr!=money.end(); ++mitr) //заменяем символ у стоимости валюты
                    {
                        if(*mitr==',')
                            *mitr='.';
                    }
                    query.addBindValue(money);
                    query.exec();
                }               
                ++itr;
            }
        }
    }
}
