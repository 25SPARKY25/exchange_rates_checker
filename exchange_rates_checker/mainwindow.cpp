#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::fileIsReady, Qt::DirectConnection);
    connect(this, &MainWindow::fillcurs, this, &MainWindow::fillValCurs);
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
       vachk = new ValuteChecker(db);
       thr = new QThread();
        vachk->moveToThread(thr);
        thr->start();
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

    //тут я решил вытащить ID валюты из словаря, потому что в словаре некоторые наименования валют прописаны в единственном числе,
    //а в курсе валют во множественном. Если строго по имени сравнивать то курс для некоторых валют может не отобразиться.
    //Например: в словаре - "Японская Иена", а в курсе валют - "Японских Иен".
    //поэтому буду сравнивать ID вместо имени
    QSqlQuery q(db);
    q.exec("SELECT  \"ID_VAL\" FROM public.\"TVALDIC\" WHERE \"Name\" = '" + valuta+ "';");
    while(q.next())
    {
        valuta = q.value(0).toString();
    }


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

//        QtConcurrent::run( [this]()
//        {
//            QVariant val;
//            QSqlQuery q(db);
//            QString query;
//            QString valuta;
//            date = this->ui->dateEdit->date().toString("dd.MM.yyyy");
//            valuta = this->ui->comboBox->currentText();



//                    q.exec("SELECT COUNT(*) FROM public.\"TVALCURS\" WHERE \"Date\" = '" + date + "';");
//                    while(q.next())
//                    {
//                        val = q.value(0);
//                    }
//                    if(val.toInt()==0)
//                    {
////                         fillValCurs();
//                        emit fillcurs();
//                    }

//                        query+="SELECT public.\"getKurs\"('"+date+"', '"+ valuta+"')";
//                        q.prepare(query);

//                        if( q.exec())
//                        {
//                            q.next();
//                            QVariant var = q.value(0);
//                            if(var!=0)
//                            {
//                                this->ui->label->setText("Курс валюты");
//                                this->ui->label_2->setText(var.toString());
//                            }
//                            else
//                            {
//                                this->ui->label->setText("На данную дату ничего не найдено!");
//                                this->ui->label_2->setText("");
//                            }
//                        }
//        } );

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

            if( q.exec())
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


        if(!map.isEmpty())
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
    if(!map.isEmpty())
    {
        if(datePr == map.last().value(0)) //совпадает ли введённая дата с датой из xml-дока
        {
            auto itr = map.begin();
            while(itr!=map.end())
            {
                if(itr.key()==valutaPr)
                {
                    money = itr.value().back();
                    for(auto mitr=money.begin(); mitr!=money.end(); ++mitr) //заменяем символ у стоимости валюты
                    {
                        if(*mitr==',')
                        {
                            *mitr='.';
                            break;
                        }

                    }

                    return  money.toDouble();
                }
                ++itr;
            }
        }
    }
    return 0;
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    this->ui->label->clear();
    this->ui->label_2->clear();
}

void MainWindow::on_dateEdit_userDateChanged(const QDate &date)
{
    Q_UNUSED(date);
    this->ui->label->clear();
    this->ui->label_2->clear();
}
