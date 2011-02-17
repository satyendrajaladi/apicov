#include "mappingfilecreation.h"
#include "ui_apicovtool.h"
#include "QDebug"

/* constructor implementation */
MappingFileCreation::MappingFileCreation(QWidget *parent) :
        QWidget(parent)
{
    QPushButton *ipSrcDirButton_3 = parent->parent()->findChild<QPushButton *>("ipSrcDirButton_3");
    qDebug()<<ipSrcDirButton_3<<__LINE__;
    connect(ipSrcDirButton_3, SIGNAL(clicked()),this,SLOT(browseButtonClicked()));

    QPushButton *resultDirButton_3 = parent->parent()->findChild<QPushButton *>("resultDirButton_3");
    connect(resultDirButton_3, SIGNAL(clicked()),this,SLOT(browseButtonClicked()));

    QPushButton *resultMapGen = parent->parent()->findChild<QPushButton *>("resultMapGen");
    connect(resultMapGen, SIGNAL(clicked()),this,SLOT(on_buttonResult_clicked()));

    ipSrcDirLineEdit_3 = parent->parent()->findChild<QLineEdit *>("ipSrcDirLineEdit_3");
    resultDirLineEdit_3 = parent->parent()->findChild<QLineEdit *>("resultDirLineEdit_3");

    qDebug()<<__FILE__<<__LINE__;

}

/* destructor implementation */
MappingFileCreation::~MappingFileCreation()
{
    //delete ui;
}

/***************** implementation of private slots*******************/
/* slot implementation for browsing the source directory on button click */
void MappingFileCreation::browseButtonClicked()
{
    qDebug()<<__FILE__<<__LINE__;

    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), QDir::currentPath());
    if (directory.isEmpty())
         return;
    else
    {
        if(clickedButton->objectName() == "ipSrcDirButton_3")
        {
           ipSrcDirLineEdit_3->setText(directory);
            sourceDirectory = directory;
        }
        else if(clickedButton->objectName() == "resultDirButton_3")
        {
            resultDirLineEdit_3->setText(directory);
            saveInDirectory = directory;
        }
    }
    qDebug()<<__FILE__<<__LINE__;
}

/* mapping.tst file generation implementation */
void MappingFileCreation::on_buttonResult_clicked()
{
    QDir sourceDir(sourceDirectory);

    QStringList validCppFiles = cppFilesList(sourceDir);
    int fileCount = validCppFiles.count();

    for(int iCount =0; iCount< fileCount; iCount++)
    {
          generateMappingFile(validCppFiles.at(iCount));
    }
}

/* to get the valid cpp files */
QStringList MappingFileCreation::cppFilesList(QDir directory)
{
    QStringList cppFileType("*.cpp");

    QStringList cppFilesList = directory.entryList(cppFileType, QDir::Files);
    QStringList validCppFiles;

    int fileCount = cppFilesList.count();
    for(int i = 0 ; i < fileCount; i++)
    {
        QString dir = directory.absolutePath();
        dir = dir.append("/");
        dir = dir.append(cppFilesList.at(i));
        validCppFiles.append(dir);
    }
    return validCppFiles;
}

/* mapping.txt creation for input file */
void MappingFileCreation::generateMappingFile(const QString &cppFileName)
{
    //DocParser b(cppFileName);
    //*************inplace of doc parser
    QFile file(cppFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<__FILE__<<__LINE__;
        return;
    }

    QByteArray cppFileData = file.readAll();
    cppFileData.simplified();
    //******************

    //getClassName start
    int classIndexNumber = 0;
    QString className;
    while((classIndexNumber = cppFileData.indexOf("class" , classIndexNumber)) != -1 )
    {
        if((cppFileData.indexOf(";",classIndexNumber)) < (cppFileData.indexOf("{",classIndexNumber))) {
            classIndexNumber  = cppFileData.indexOf(";",classIndexNumber);
            continue;
        }
        else
        {
            char *dataPtr = cppFileData.data();
            dataPtr = dataPtr + classIndexNumber + 6;  //why, don't know (lenght of class)
            while((*dataPtr != '{') && (*dataPtr != ':' ))
            {
                className += QString(QString(dataPtr)).at(0);
                dataPtr++;
            }
            className.remove(' ');
            qDebug()<< "@@@@@@@@@@@@@@@@@@"<< className;
            break;
        }
   }//getClassName end

    //getApiTestCaseWithId start
    int index = 0;
    QMap<QString, QString> apiTestCaseMap;
    QString apiTestCaseName;
    QString apiTestCaseId;
    index= cppFileData.indexOf("};", index);
    index= cppFileData.indexOf("MaemoAPI-", index);

    while(index != -1)
    {
        apiTestCaseId.clear();
        apiTestCaseName.clear();

        char *dataPtr = cppFileData.data();
        dataPtr = dataPtr + index + 9;
        while(int(*dataPtr)>= 48 && int(*dataPtr)<= 57)
        {
            apiTestCaseId += *dataPtr;
            dataPtr++;
            index++;
        }

        index= cppFileData.indexOf("::", index);

        char *dataPtr1 = cppFileData.data();
        dataPtr1 = dataPtr1 + index + 2;
        while(*dataPtr1 != '(')
        {
            apiTestCaseName += *dataPtr1;
            dataPtr1++;
            index++;
        }

        apiTestCaseMap.insertMulti(apiTestCaseName, apiTestCaseId);
        index= cppFileData.indexOf("MaemoAPI-", index);
    }

    //creating the path string to save the generated mapping file
    QString outputFileName;
    (((outputFileName.append("/")).append(className)).append("MappingFile.txt")).prepend(saveInDirectory);

    //creation of mapping.txt file for given .cpp file.
    QFile outputFile(outputFileName);
    outputFile.open(QIODevice::WriteOnly | QIODevice::Text);

    //variable to store mapping file internal text data.
    QByteArray fileData;

    QStringList totaApiTestCase = apiTestCaseMap.uniqueKeys();
    for(int i=0; i< totaApiTestCase.count(); i++)
    {

        //reviewer comment data
        fileData.append("\n" + className.toLower() + "-");
        fileData.append(totaApiTestCase.at(i) + ":");
        fileData.append("MaemoAPI-" + apiTestCaseMap.value(totaApiTestCase.at(i)));
    }

    outputFile.write(fileData);
    outputFile.close();
}
