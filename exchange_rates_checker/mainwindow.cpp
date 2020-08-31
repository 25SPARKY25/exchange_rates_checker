#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::fileIsReady, Qt::DirectConnection);
    connect(this, &MainWindow::done, &loop, &QEventLoop::quit);
    xml = new QXmlStreamReader();

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("Valuta");
       db.setHostName("localhost");
       db.setUserName("postgres");
       db.setPassword("0000");
       dbIsOpen = db.open();

       if(dbIsOpen)
       {
           checkValDictionary();
       }



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fileIsReady(QNetworkReply *reply)
{
    xml->clear();
    xml->addData(reply->readAll());
    readXml(*xml, map);
    emit done();
}

void MainWindow::on_pb_fillValKurs_clicked()
{
    QString progrDate, valuta;
    double rez;
    progrDate = this->ui->dateEdit->date().toString("dd.MM.yyyy");
    valuta = this->ui->comboBox->currentText();
    rez = getProgrCurs(progrDate, valuta);
    if(rez!=0)
    {
        this->ui->label->setText("Курс валюты");
        this->ui->label_2->setText(QString::number(rez));
    }
    else
    {
        this->ui->label->setText("На данную дату ничего не найдено!");
        this->ui->label_2->setText("");
    }

}

void MainWindow::on_pushButton_3_clicked()
{

    if(dbIsOpen)
    {
        QVariant val;
        QSqlQuery q(db);
        QString query;
        QString valuta;
        date = this->ui->dateEdit->date().toString("dd.MM.yyyy");
        valuta = this->ui->comboBox->currentText();

        q.exec("SELECT COUNT(*) FROM public.\"TVALCURS\" WHERE \"Date\" = '" + date + "';");
        while(q.next())
        {
            val = q.value(0);
        }
        if(val.toInt()==0)
        {
             fillValCurs();
        }

            query+="SELECT public.\"getKurs\"('"+date+"', '"+ valuta+"')";
            q.prepare(query);

            if( !q.exec())
            {
                qDebug()<<q.lastError().text();
            }
            else
            {
                q.next();
                QVariant var = q.value(0);

                if(var!=0)
                {
                    this->ui->label->setText("Курс валюты");
                    this->ui->label_2->setText(var.toString());
                }
                else
                {
                    this->ui->label->setText("На данную дату ничего не найдено!");
                    this->ui->label_2->setText("");
                }
            }
    }
}


void MainWindow::checkValDictionary()
{
    QSqlQuery query;
    QVariant val;
    if(dbIsOpen)
    {
        query=QSqlQuery(db);
        query.exec("SELECT COUNT(*) FROM public.\"TVALDIC\";");
        while(query.next())
        {
            val = query.value(0);
        }
        if(val.toInt()==0)
        {
             fillValDIctionary();
        }
        else
        {
            fillValComboBox();
        }
    }
}

void MainWindow::fillValDIctionary()
{
    if(dbIsOpen)
    {
        manager->get(QNetworkRequest(QUrl("http://www.cbr.ru/scripts/XML_valFull.asp")));
        QSqlQuery q(db);
        QString query;

        query+=" INSERT INTO public.\"TVALDIC\"(\"ID_VAL\", \"Name\", \"EngName\", \"Nominal\", \"ParentCode\", \"ISO_Num_Code\", \"ISO_Char_Code\") VALUES (?, ?, ?, ?, ?, ?, ?);";
        q.prepare(query);

        loop.exec();

        fillDB(q, map, true);
        fillValComboBox();
    }
}

void MainWindow::fillValComboBox()
{
    QSqlQuery query;
    QVariant val;
    query=QSqlQuery(db);
    query.exec("SELECT \"Name\" FROM public.\"TVALDIC\";");
    while(query.next())
    {
        val = query.value(0);
        this->ui->comboBox->addItem(val.toString());
    }
}

void MainWindow::fillValCurs()
{
    if(dbIsOpen)
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

double MainWindow::getProgrCurs(QString &datePr, QString &valutaPr)
{
    QString str = "http://www.cbr.ru/scripts/XML_daily.asp?date_req=" + datePr;
    QString money;
    manager->get(QNetworkRequest(QUrl(str)));
    loop.exec();
    if(map.size()!=0)
    {
        if(datePr == map.last().value(0)) //совпадает ли введённая дата с датой из xml-дока
        {
            auto itr = map.begin();
            while(itr!=map.end())
            {
                if(itr->value(3)==valutaPr)
                {
                    money = itr.value().back();
                    for(auto mitr=money.begin(); mitr!=money.end(); ++mitr) //заменяем символ у стоимости валюты
                    {
                        if(*mitr==',')
                            *mitr='.';
                    }

                    return  money.toDouble();
                }
                else
                {
                  return 0;
                }
            }
        }
    }
    return 0;
}
