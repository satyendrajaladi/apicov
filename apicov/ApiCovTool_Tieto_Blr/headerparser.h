#ifndef HEADERPARSER_H
#define HEADERPARSER_H

#include <QtGui/QWidget>

/* Contains class info parsed from Header file */
struct ClassData {
    QString title;
    QStringList enumList;
    QStringList publicMethods;
};


class HeaderParser{
   QByteArray headerFileInfo;
public:
     HeaderParser(QByteArray );

     QString getClassName();//later need to be removed
     QStringList getClassNames(); //will give number of classes present in a header file
     QStringList getEnumData();
     QStringList getEnumTstData();

     QMap<QString, QStringList> getPublicMethods(QString className,QStringList &classBlock); // classname should be one for the className from get classNmaes
     QStringList getPublicTstMethodsData();
    // QMap<QString, QStringList> getSlotMethods();
     QMap<QString, QStringList> getSlotMethods(QString className,QStringList &classData);
     QStringList getSlotTstMethodsData();
     //QMap<QString, QStringList> getSignalMethods();
     QMap<QString, QStringList> getSignalMethods(QString className,QStringList & classData);
     QStringList getSignalTstMethodsData();
     //QMap<QString, QStringList> getPublicMethodsData();
//protected:
     QStringList getEnumsRawData();
     QStringList getPublicMethodsRawData(QString className,QStringList &classBlock);
     //QMap <QString,QStringList> getPublicMethodsRawData();
     QMap <QString,QStringList> getClassBocks();
     void removeDefinitions(QString &data);
     //QStringList getSlotMethodsRawData();
     QStringList getSlotMethodsRawData(QString className,QStringList &classBlock);
    // QStringList getSignalMethodsRawData();
     QStringList getSignalMethodsRawData(QString className,QStringList &classBlock);
     QStringList removeProperties(QString className,QString &data);
private:
     QStringList fileInfoLineData;

};















#endif // HEADERPARSER_H
