#ifndef APITEMPLATE_H
#define APITEMPLATE_H

#include <QMainWindow>
#include <QFileDialog>
#include "headerparser.h"
#include "commentextractor.h"
#include "ui_apicovtool.h"

/* this file use to creating the template of given module class. */
class ApiTemplate : public QWidget
{
    Q_OBJECT

public:
    explicit ApiTemplate(QWidget *parent = 0);
    ~ApiTemplate();

private:
    QStringList headerFilesList(QDir directory);
    void generateTemplate(const QString &);

private slots:
    void browseButtonClicked();
    void getResult();
    //void on_tempback_clicked();

private:
    Ui_ApiCovTool *ui;
    QString headerDirectory ;
    QString saveInDirectory;
    QWidget *parent;
    QLineEdit *ipHeadDirLineEdit_2;
    QLineEdit *resultDirLineEdit_2;
};

#endif // APITEMPLATE_H
