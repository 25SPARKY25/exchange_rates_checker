#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlTableModel>


void fillDB(QSqlQuery &query, const QMap<QString, QStringList> &map, const bool &isDictionary=false);



#endif // QUERYHANDLER_H
