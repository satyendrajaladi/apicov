#ifndef COMMENTEXTRACTOR_H
#define COMMENTEXTRACTOR_H
#include <QtGui/QWidget>
#include <qfile.h>
#include <qdebug.h>

class CommentRemover //:public QWidget
{
    QByteArray inputFileData;

  public:
    CommentRemover(QString  /*QWidget *parent = 0*/);
    //void removeComments();
    QByteArray removeComments();

    ~CommentRemover();
};


#endif // COMMENTEXTRACTOR_H
