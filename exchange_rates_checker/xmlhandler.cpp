#include "xmlhandler.h"


void readXml(QXmlStreamReader &inp, QMap<QString, QStringList> &map)
{
    QString str;
    QStringList lst;
    map.clear();

    while (!inp.atEnd() && !inp.hasError())
    {
        QXmlStreamReader::TokenType token = inp.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {
            if(inp.name() == "Valuta" || inp.name() == "ValCurs")
            {
                str+=inp.name().toString();
                foreach(const QXmlStreamAttribute &attr, inp.attributes())
                {
                    lst.push_back(attr.value().toString());
                }
                if(!lst.empty())//проверяем есть ли аттрибуты, если нету то выходим из функции
                {
                    map.insert(str, lst);
                    lst.clear();
                    str.clear();
                }
                else
                {
                    return;
                }
            }

            if (inp.name() == "Item" || inp.name() == "Valute")
            {
                while (  (!(inp.tokenType() == QXmlStreamReader::EndElement && inp.name() == "Item"))  &&  (!(inp.tokenType() == QXmlStreamReader::EndElement && inp.name() == "Valute")) )
                {
                    if (inp.tokenType() == QXmlStreamReader::StartElement)
                    {
                        if(inp.name() == "Item" || inp.name() == "Valute")
                        {
                            foreach(const QXmlStreamAttribute &attr, inp.attributes())
                            {
                                if(attr.name()=="ID")
                                    str+=attr.value().toString();
                            }
                        }
                        else
                        {
                            inp.readNext();
                            lst.push_back(inp.text().toString());
                        }
                    }
                    inp.readNext();
                }
                map.insert(str, lst);
                lst.clear();
                str.clear();
            }
        }
    }
}
