#include "apitemplate.h"
#include "ui_apicovtool.h"
#include <QDebug>
#if 1

/* constructor implementation which display a ui widget to user with required option */
ApiTemplate::ApiTemplate(QWidget *parent) :
    QWidget(parent)
{
    qDebug()<<__FILE__<<__LINE__;

    QPushButton *ipHeaderDirButton_2 = parent->findChild<QPushButton *>("ipHeaderDirButton_2");
    connect(ipHeaderDirButton_2, SIGNAL(clicked()),this,SLOT(browseButtonClicked()));

    QPushButton *resultDirButton_2 = parent->findChild<QPushButton *>("resultDirButton_2");
    connect(resultDirButton_2, SIGNAL(clicked()),this,SLOT(browseButtonClicked()));

    QPushButton *resultTemplate = parent->findChild<QPushButton *>("resultTemplate");
    connect(resultTemplate, SIGNAL(clicked()),this,SLOT(getResult()));

    ipHeadDirLineEdit_2 = parent->findChild<QLineEdit *>("ipHeadDirLineEdit_2");
    resultDirLineEdit_2 = parent->findChild<QLineEdit *>("resultDirLineEdit_2");

    qDebug()<<__FILE__<<__LINE__;
}

/* destructor implementation for reallocating the unwanted memory. */
ApiTemplate::~ApiTemplate()
{
    //delete ui;
}

/***************** implementation of private slots*******************/
/* slot implementation for browsing the source directory on button click */
void ApiTemplate::browseButtonClicked()
{
    qDebug()<<"IN BROWSE"<<__LINE__;
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), QDir::currentPath());
    if (directory.isEmpty())
         return;
    else
    {
        if(clickedButton->objectName() == "ipHeaderDirButton_2")
        {
            qDebug()<<__FILE__<<__LINE__;

            ipHeadDirLineEdit_2->setText(directory);

            headerDirectory = directory;
            qDebug()<<__FILE__<<__LINE__;
        }
        else if(clickedButton->objectName() == "resultDirButton_2")
        {
            resultDirLineEdit_2->setText(directory);

            saveInDirectory = directory;

            qDebug()<<__FILE__<<__LINE__;
        }
    }
}

//void ApiTemplate::on_tempback_clicked()
//{
    //clear all line edits
   // ui->stackedWidget->setCurrentIndex(0);
///}


/* slot implementation for Result generation on result button click */
void ApiTemplate::getResult()
{
    qDebug()<<__FILE__<<__LINE__;
    QDir headerDir(headerDirectory);

    QStringList headerFiles = headerFilesList(headerDir);
    QString className;
    QStringList cppFile;
    int fileCount = headerFiles.count();
    for(int iCount =0; iCount< fileCount; iCount++)
    {
          generateTemplate(headerFiles.at(iCount));
    }
}

/*getting the valid html files rom given documentation directory. */
QStringList ApiTemplate::headerFilesList(QDir directory)
{
    QDir dir(directory);
    static QStringList filesList;

    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    foreach(QFileInfo finfo, list) {

        if(finfo.fileName().contains(".h")) {
            QString absPath = finfo.absoluteFilePath();
            filesList.append(absPath);
        }

    }

    return filesList;
}

/* Template creation for input class*/
void ApiTemplate::generateTemplate(const QString &headerFile)
{
    CommentRemover a(headerFile);
    QByteArray headerFileInfo;
    headerFileInfo =  a.removeComments();
    QString dummyHeaderFile = headerFile;
    QString headerFileName = dummyHeaderFile.remove(0, dummyHeaderFile.lastIndexOf('/')+1);
    headerFileName = headerFileName.left(headerFileName.lastIndexOf('.'));
    qDebug()<<"HEADERFILENAME"<<headerFileInfo<<__LINE__;


    HeaderParser b(headerFileInfo);
    QStringList enumVars = b.getEnumData();
    qDebug()<<"ENUMDATA:"<<enumVars<<__LINE__;
    QString outputFileName;
    (((outputFileName.append("/tst_")).append(headerFileName.toLower())).append(".cpp")).prepend(saveInDirectory);


    QStringList classNames = b.getClassNames();
    qDebug()<<classNames<<__LINE__;
    QMap <QString, QStringList> classBlocks = b.getClassBocks();

    qDebug()<<"classBlocks"<<classBlocks<<__LINE__;
    //creating the path string to save the generated template file

    //creation of .cpp file for given outfile name.
    QFile outputFile(outputFileName);
    qDebug()<<"OUTPUTFILENAME"<<outputFileName<<__LINE__;

    outputFile.open(QIODevice::WriteOnly | QIODevice::Text);

    //variable to store .cpp file internal text data.
    QByteArray fileData;

    //reviewer comment data
    fileData.append("\n/*******************************************************************************************");
    fileData.append("\nAuthor Name: ");
    fileData.append("\nDocument Version: ");
    fileData.append("\n===================================================================================");
    fileData.append("\nReviewer Name \tDate \t other");
    fileData.append("\n----------------------------------------------------------------------------------- ");
    fileData.append("\n ");
    fileData.append("\n*******************************************************************************************/");
    bool enumState = true;

    for(int iCount = 0; iCount < classNames.size(); iCount++)
    {
        QStringList classData = classBlocks.value(classNames.at(iCount));
        QMap<QString, QStringList> publicMethodsDataList = b.getPublicMethods(classNames.at(iCount),classData);  //getting the public members
        qDebug()<<classData<<__LINE__;
        QStringList totalPublicMethods = publicMethodsDataList.uniqueKeys();
        qDebug()<<"publicMethods"<<totalPublicMethods<<__LINE__;
        QMap<QString, QStringList> slotMethodsDataList = b.getSlotMethods(classNames.at(iCount),classData) ;
        qDebug()<<"slotMethods"<<slotMethodsDataList<<__LINE__;
        QStringList totalSlotMethods = slotMethodsDataList.uniqueKeys();
        QMap<QString, QStringList> signalMethodsDataList = b.getSignalMethods(classNames.at(iCount),classData);
        qDebug()<<"signalMethods"<<signalMethodsDataList<<__LINE__;
        QStringList totalSignalMethods = signalMethodsDataList.uniqueKeys();
        qDebug()<<"totalSignalMethods"<<totalSignalMethods<<__LINE__;
         //class declaration text
        fileData.append("\nclass tst_"+classNames.at(iCount)+": QObject");
        fileData.append("\n{");
        fileData.append("\n\tQ_OBJECT");
        QString enumVariable;
        if(enumState)
        {
          if(enumVars.size())
          {
            fileData.append("\n\n /*Enums:*/ ");
            for(int i=0; i< enumVars.count(); i++)
             {
                enumVariable= enumVars.value(i);
                fileData.append("\n\tvoid tst_" + enumVariable + "();  //MaemoAPI-");
             }

            }
          else
              enumState = false;
       }

        fileData.append("\n\npublic: ");
        fileData.append("\n\tvoid initTestcase();");

        //member function declaration text
         QString methodName,slotName,signalName;


         for(int i=0; i< totalPublicMethods.count(); i++)
          {
                methodName= totalPublicMethods.value(i);

                if(methodName.contains(QRegExp("==")))
                   methodName= methodName.replace(QRegExp("=="), "Equall");
                else if(methodName.contains(QRegExp("!=")))
                     methodName= methodName.replace(QRegExp("!="), "NotEquall");
                else if(methodName.contains(QRegExp("=")))
                     methodName= methodName.replace(QRegExp("="), "Asign");
                else if(methodName.contains(QRegExp("~")))
                     methodName= methodName.replace(QRegExp(methodName), "destructor");
                else if(methodName.contains(QRegExp(classNames.at(iCount))))  //constructor
                      methodName= methodName.replace(QRegExp(classNames.at(iCount)), "constructor");
                else if(methodName.contains(QRegExp("<<")))
                      methodName= methodName.replace(QRegExp("<<"), "LeftShift");
                else if(methodName.contains(QRegExp(">>")))
                      methodName= methodName.replace(QRegExp(">>"), "RightShift");

                fileData.append("\n\tvoid tst_" + methodName + "();  //MaemoAPI-");

          }
         if(totalSlotMethods.size())
         {
            fileData.append("\n\n/*slots: */");
            for(int i=0; i< totalSlotMethods.count(); i++)
             {
                slotName= totalSlotMethods.value(i);
                fileData.append("\n\tvoid tst_" + slotName + "();  //MaemoAPI-");
              }

         }
         if(totalSignalMethods.size())
         {
            fileData.append("\n\n /*signals:*/ ");
            for(int i=0; i< totalSignalMethods.count(); i++)
             {
                signalName= totalSignalMethods.value(i);
                fileData.append("\n\tvoid tst_" + signalName + "();  //MaemoAPI-");
              }

         }

        fileData.append("\n\tvoid cleanupTestCase();");
        fileData.append("\n};");
    }
    QString enumVariable;
    for(int i=0; i< enumVars.count(); i++)
    {
        fileData.append("\n\n/*MaemoEnums- :" + enumVars.value(i) + "() Enum test. */");
        enumVariable = enumVars.value(i);
        fileData.append("\nvoid tst_" + enumVariable + "()");
        fileData.append("\n{ \n}");

     }
        for(int iCount = 0; iCount < classNames.size(); iCount++)
        {
            QStringList classData = classBlocks.value(classNames.at(iCount));
            QMap<QString, QStringList> publicMethodsDataList = b.getPublicMethods(classNames.at(iCount),classData);  //getting the public members
            qDebug()<<publicMethodsDataList<<__LINE__;
            QStringList totalPublicMethods = publicMethodsDataList.uniqueKeys();
            QMap<QString, QStringList> slotMethodsDataList = b.getSlotMethods(classNames.at(iCount),classData) ;
            QStringList totalSlotMethods = slotMethodsDataList.uniqueKeys();
            QMap<QString, QStringList> signalMethodsDataList = b.getSignalMethods(classNames.at(iCount),classData) ;
            QStringList totalSignalMethods = signalMethodsDataList.uniqueKeys();
        //member function definition text
             QString methodName,slotName,signalName;


            for(int i=0; i< totalPublicMethods.count(); i++)
            {

                fileData.append("\n\n/*MaemoAPI- :" + totalPublicMethods.value(i) + "() API property test. */");
                methodName= totalPublicMethods.value(i);
               if(methodName.contains(QRegExp("==")))
                  methodName= methodName.replace(QRegExp("=="), "Equall");
               else if(methodName.contains(QRegExp("!=")))
                   methodName= methodName.replace(QRegExp("!="), "NotEquall");

               else if(methodName.contains(QRegExp("=")))
                   methodName= methodName.replace(QRegExp("="), "Asign");

               else if(methodName.contains(QRegExp("~")))
                   methodName= methodName.replace(QRegExp(methodName), "destructor");

               else if(methodName.contains(QRegExp(classNames.at(iCount))))  //constructor
                   methodName= methodName.replace(QRegExp(classNames.at(iCount)), "constructor");

               else if(methodName.contains(QRegExp("<<")))
                   methodName= methodName.replace(QRegExp("<<"), "LeftShift");

               else if(methodName.contains(QRegExp(">>")))
                   methodName= methodName.replace(QRegExp(">>"), "RightShift");


               fileData.append("\nvoid tst_" + classNames.at(iCount) + "::tst_" + methodName + "()");
               fileData.append("\n{ \n}");

        }
        for(int i=0; i< totalSlotMethods.count(); i++)
        {
            fileData.append("\n\n/*MaemoAPI- :" + totalSlotMethods.value(i) + "() API property test. */");
            slotName= totalSlotMethods.value(i);
            fileData.append("\nvoid tst_" + classNames.at(iCount) + "::tst_" + slotName + "()");
            fileData.append("\n{ \n}");

         }

        for(int i=0; i< totalSignalMethods.count(); i++)
        {
            fileData.append("\n\n/*MaemoAPI- :" + totalSignalMethods.value(i) + "() API property test. */");
            signalName= totalSignalMethods.value(i);
            fileData.append("\nvoid tst_" + classNames.at(iCount) + "::tst_" + signalName + "()");
            fileData.append("\n{ \n}");

         }

      // fileData.append("\n\npublic: ");
    //adding .moc and main file

  }
    fileData.append("\nQMAIN_TEST (tst_" + headerFileName + ");");
    fileData.append("\n#include \"tst_" + headerFileName + "\"");
    outputFile.write(fileData);
    outputFile.close();



}
#endif
