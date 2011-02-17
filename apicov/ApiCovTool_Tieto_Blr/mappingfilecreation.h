#ifndef MAPPINGFILECREATION_H
#define MAPPINGFILECREATION_H

#include <QMainWindow>
#include <QFileDialog>
#include "ui_apicovtool.h"

//namespace Ui {
  //  class MappingFileCreation;
//}

class MappingFileCreation : public QWidget
{
    Q_OBJECT

public:
    explicit MappingFileCreation(QWidget *parent = 0);
    ~MappingFileCreation();

private:
    QStringList cppFilesList(QDir );
    void generateMappingFile(const QString &);

private slots:
    void browseButtonClicked();
    void on_buttonResult_clicked(); //auto generate signal

private:
    Ui_ApiCovTool *ui;
    QString sourceDirectory ;
    QString saveInDirectory;
    QWidget *parent;
    QLineEdit *ipSrcDirLineEdit_3;
    QLineEdit *resultDirLineEdit_3;
};

#endif // MAPPINGFILECREATION_H
