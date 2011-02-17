
#include "commentextractor.h"
#include <QtGlobal>


CommentRemover::CommentRemover(QString fileName /* QWidget *parent */)
   // : QWidget(parent)
{
    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<__FILE__<<__LINE__;
        return;
    }

    inputFileData = inputFile.readAll();
    if(inputFileData.isEmpty())
    {
        qDebug()<<"EMPTY CPP FILE";
        qDebug()<<__FILE__<<__LINE__;
        return;
    }
}

QByteArray CommentRemover::removeComments()
{

    int firstIndexVar = 0;
    int lastIndexVar = 0;
   // QDebug()<<inputFileData;
    while ((firstIndexVar = inputFileData.indexOf("/*", firstIndexVar))!= -1)
    {
        lastIndexVar = firstIndexVar;
        if((lastIndexVar = (inputFileData.indexOf("*/" , lastIndexVar))))
        {
              inputFileData.remove(firstIndexVar,(lastIndexVar - firstIndexVar + 2));
        }

    }

    firstIndexVar = 0;
    lastIndexVar = 0;

    while((firstIndexVar = inputFileData.indexOf("//", firstIndexVar))!= -1)
    {

        lastIndexVar = firstIndexVar;
//Used Qstring to sort out comparision problem and fixed the singleline comment issue
        if(QString(inputFileData.at((firstIndexVar-1))) == QString(':') ) //Skips  if this is occured in string Literal  ftp://drivehq.com
        {
            firstIndexVar++;
            continue;
        }

        if((lastIndexVar = (inputFileData.indexOf("\n", lastIndexVar))))
        {
              inputFileData.remove(firstIndexVar,(lastIndexVar - firstIndexVar + 2));
        }
    }

    firstIndexVar = 0;
    lastIndexVar = 0;
    while((firstIndexVar = inputFileData.indexOf("#if 0",firstIndexVar))!= -1)
    {

        lastIndexVar = firstIndexVar;
        if(lastIndexVar = (inputFileData.indexOf("#endif",lastIndexVar)))
        {
            inputFileData.remove(firstIndexVar,(lastIndexVar-firstIndexVar + 6/*Size of Endif*/));
        }
    }

  //headerWithNoComments = inputFileData;

  return inputFileData;
}

CommentRemover::~CommentRemover()
{
    //
}


