#ifndef APICOVTOOL_H
#define APICOVTOOL_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QFileDialog>
#include "commentextractor.h"
#include "headerparser.h"
#include "apitemplate.h"

namespace Ui {
    class ApiCovTool;
}

class ApiCovTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit ApiCovTool(QWidget *parent = 0);
    ~ApiCovTool();
    void on_apiCov_clicked();
    void on_template_clicked();
    void on_genMap_clicked();
    void on_valMap_clicked();
    void on_genTests_clicked();
    void on_example_clicked();
    void on_settings_clicked();    

    QString headerFileName;
    QString className;
    QStringList classNames;
    QStringList enumVariables;
    QStringList enumTstVariables;
    QMap<QString ,QStringList> publicMethods;
    QStringList publicTstMethods;
    QMap<QString ,QStringList> slotMethods;
    QStringList slotTstMethods;
    QMap<QString ,QStringList> signalMethods;
    QStringList signalTstMethods;
    //slots
    int checkEnumData(QStringList totalEnumsData ,QStringList & unUsedEnums, QByteArray cppFileByteData);
    int checkEnumTstData(QStringList totalEnumsData ,QStringList & unUsedEnums ,QByteArray cppFileByteData);
    int checkPublicMethodsData(QMap<QString , QStringList> /*QStringList*/ publicMethodsDataList,QStringList &unUsedPublicMethodsList,QByteArray cppFileByteData);
    int checkPublicTstMethodsData(QStringList publicMethodsDataList, QStringList &unUsedPublicMethodsList, QByteArray cppFileByteData);
    int checkSlotMethodsData(QMap<QString , QStringList> /*QStringList*/ publicMethodsDataList,QStringList &unUsedPublicMethodsList,QByteArray cppFileByteData);
    int checkSlotTstMethodsData(QStringList publicMethodsDataList, QStringList &unUsedPublicMethodsList, QByteArray cppFileByteData);
    int checkSignalMethodsData(QMap<QString , QStringList> /*QStringList*/ publicMethodsDataList,QStringList &unUsedPublicMethodsList,QByteArray cppFileByteData);
    int checkSignalTstMethodsData(QStringList publicMethodsDataList, QStringList &unUsedPublicMethodsList, QByteArray cppFileByteData);
    void getResult(const QString &,QString &, HeaderParser &parseData);
private slots:
    void listItemClicked(QListWidgetItem* );
    void browseButtonClicked();
    void findFiles();
    void on_back_clicked();

private:
    QStringList cppFilesList(QDir directory);
    QStringList headerFilesList(QDir directory);


private:
    Ui::ApiCovTool *ui;
    //ApiTemplate *temp;
    QString headerDirectory;
    QString sourceDirectory;
    QString resultDirectory;

};

#endif // APICOVTOOL_H
