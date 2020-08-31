#ifndef XMLHANDLER_H
#define XMLHANDLER_H
#include <QXmlStreamReader>
#include <QMap>

void readXml(QXmlStreamReader &inp, QMap<QString, QStringList> &map);

#endif // XMLHANDLER_H
