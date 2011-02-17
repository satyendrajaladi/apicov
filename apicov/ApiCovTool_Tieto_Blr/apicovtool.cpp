#include "apicovtool.h"
#include "ui_apicovtool.h"
#include <QDebug>
#include "apitemplate.h"
#include "mappingfilecreation.h"

enum Page{ApiCovPage,TemplatePage,GenMapPage,ValMapPage,GenTestsPage,ExampleCovPage,SettingsPage};


ApiCovTool::ApiCovTool(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::ApiCovTool)
{
    ui->setupUi(this);
    connect(ui->listContents,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(listItemClicked(QListWidgetItem*)));

    //    integrated by avinash
    //    connect(ui->ipHeadDirButton, SIGNAL(clicked(bool)), this, SLOT(browseButtonClicked()));
    //    connect(ui->ipSrcDirButton, SIGNAL(clicked(bool)), this, SLOT(browseButtonClicked()));
    //    connect(ui->resultDirButton, SIGNAL(clicked(bool)), this, SLOT(browseButtonClicked()));
    //    connect(ui->quitApiCov, SIGNAL(clicked(bool)),this,SLOT(close()));
    //    connect(ui->resultApiCov,SIGNAL(clicked(bool)), this ,SLOT(findFiles()));
}

ApiCovTool::~ApiCovTool()
{
    delete ui;
}
void ApiCovTool::listItemClicked(QListWidgetItem* item)
{
    qDebug()<<__FILE__<<__LINE__;
    if(ui->listContents->currentRow() == ApiCovPage)
    {
        ui->stackedWidget->setCurrentIndex(1);
        on_apiCov_clicked();
    }

    if(ui->listContents->currentRow() == TemplatePage)
    {

        ui->stackedWidget->setCurrentIndex(2);
        on_template_clicked();
    }

    if(ui->listContents->currentRow() == GenMapPage)
    {
        ui->stackedWidget->setCurrentIndex(3);
        on_genMap_clicked();
    }

    if(ui->listContents->currentRow() == ValMapPage)
    {
        ui->stackedWidget->setCurrentIndex(4);
        on_valMap_clicked();
    }

    if(ui->listContents->currentRow() == GenTestsPage)
    {
        ui->stackedWidget->setCurrentIndex(5);
        on_genTests_clicked();
    }

    if(ui->listContents->currentRow() == ExampleCovPage)
    {
        ui->stackedWidget->setCurrentIndex(6);
        on_example_clicked();
    }

    if(ui->listContents->currentRow() == SettingsPage)
    {
        ui->stackedWidget->setCurrentIndex(7);
        on_settings_clicked();
    }

}

void ApiCovTool::on_apiCov_clicked()
{    
    connect(ui->ipHeadDirButton, SIGNAL(clicked(bool)), this, SLOT(browseButtonClicked()));
    connect(ui->ipSrcDirButton, SIGNAL(clicked(bool)), this, SLOT(browseButtonClicked()));
    connect(ui->resultDirButton, SIGNAL(clicked(bool)), this, SLOT(browseButtonClicked()));
    connect(ui->quitApiCov, SIGNAL(clicked(bool)),this,SLOT(on_back_clicked()));
    connect(ui->resultApiCov,SIGNAL(clicked(bool)), this ,SLOT(findFiles()));
}

void ApiCovTool::on_back_clicked()
{
    if(ui->stackedWidget->currentIndex() == 1)
    {
        //clear all line edits
        ui->stackedWidget->setCurrentIndex(0);
    }
    if(ui->stackedWidget->currentIndex() == 2)
    {
        //clear all template
        ui->stackedWidget->setCurrentIndex(0);
    }
    if(ui->stackedWidget->currentIndex() == 3)
    {
        //clear all template
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void ApiCovTool::on_template_clicked()
{


   QWidget *pageTemplate = this->ui->pageTemplate;
   ApiTemplate *apiTemplate = new ApiTemplate(pageTemplate);
   QObject::connect(ui->quitTemplate,SIGNAL(clicked()),this,SLOT(on_back_clicked()));
}

void ApiCovTool::on_genMap_clicked()
{
    qDebug()<<"IN GENMAP"<<__LINE__;
    QWidget *pageTemplate = this->ui->pageTemplate;
    MappingFileCreation *genMapTemplate = new MappingFileCreation(pageTemplate);
    QObject::connect(ui->quitMapGen,SIGNAL(clicked()),this,SLOT(on_back_clicked()));
}

void ApiCovTool::on_valMap_clicked()
{
    QObject::connect(ui->quitValMap,SIGNAL(clicked()),this,SLOT(close()));
}

void ApiCovTool::on_genTests_clicked()
{
    QObject::connect(ui->quitGenTests,SIGNAL(clicked()),this,SLOT(close()));
}

void ApiCovTool::on_example_clicked()
{
    QObject::connect(ui->quitExampleCov,SIGNAL(clicked()),this,SLOT(close()));
}

void ApiCovTool::on_settings_clicked()
{
    QObject::connect(ui->quitSettings,SIGNAL(clicked()),this,SLOT(close()));
}


//Added by avinash for handling the browse buttons in apicov
void ApiCovTool::browseButtonClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), QDir::currentPath());
    if (directory.isEmpty())
         return;
    else
    {
        if(clickedButton->objectName() == "ipHeadDirButton")
        {
            ui->ipHeadDirLineEdit->setText(directory);
            headerDirectory = directory;
        }
        else if(clickedButton->objectName() == "ipSrcDirButton")
        {
            ui->ipSrcDirLineEdit->setText(directory);
            sourceDirectory = directory;

        }
        else if(clickedButton->objectName() == "resultDirButton")
        {
            ui->resultDirLineEdit->setText(directory);
            resultDirectory = directory;

        }
    }
}

/*Will look for header files and corresponding cppfile*/
void ApiCovTool::findFiles()
{
    QDir headerDir(headerDirectory);
    QDir srcDir(sourceDirectory);

    QString cppFileName = headerDirectory.remove(0,(headerDirectory.lastIndexOf('/')));

    qDebug()<<"AVINASH%%%%%%%%%%  :"<<cppFileName;
    QStringList headerFileList  = headerFilesList(headerDir);

    QStringList cppFileList = cppFilesList(srcDir);

    QStringList cppFile;
    int noHeaderFiles = headerFileList.count();
    for(int iCount =0; iCount < noHeaderFiles; iCount++)
    {
        CommentRemover app(QString(headerFileList.at(iCount)));
        QByteArray fileinfo ;
        fileinfo = app.removeComments();
        HeaderParser parseData(fileinfo);

        headerFileName = headerFileList.at(iCount); //added by raja to handle mutiple classes in one .h file
        QStringList list = headerFileName.split("/");
        headerFileName = list.takeLast();
        QString headerFile = headerFileName;
        headerFileName = headerFileName.left(headerFileName.lastIndexOf('.'));
        qDebug()<<headerFileName;
         enumVariables = parseData.getEnumData();
         enumTstVariables = parseData.getEnumTstData();
       // className = parseData.getClassName();


     //   signalTstMethods = parseData.getSignalTstMethodsData();

        QString name = className;

        //name should be haderfilename.cpp

        QString cppFileName(((headerFileName.prepend("tst_")).append(".cpp")).toLower());

        qDebug()<<"cppFileName :"<<cppFileName<<__LINE__;

        if((cppFileList.filter(cppFileName)).count() == 1)
        {
           cppFile  = cppFileList.filter(cppFileName);
           qDebug()<<"cppFile.at(0) :"<<cppFile.at(0)<<__LINE__;
           getResult(cppFile.at(0),headerFile,parseData );
        }
     }

}


QStringList ApiCovTool::headerFilesList(QDir directory)
{
    QDir dir(directory);
    static QStringList filesList;

    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    foreach(QFileInfo finfo, list) {

        if(finfo.fileName().contains(".h")) {
            QString absPath = finfo.absoluteFilePath();
            filesList.append(absPath);
        }

        if (finfo.isDir()) {
            cppFilesList(QDir(finfo.absoluteFilePath()));
        }
    }

    return filesList;
}
QStringList ApiCovTool::cppFilesList(QDir directory)
{
    QDir dir(directory);
    static QStringList filesList;

    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    foreach(QFileInfo finfo, list) {

        if(finfo.fileName().contains(".cpp")) {
            QString absPath = finfo.absoluteFilePath();
            filesList.append(absPath);
        }

        if (finfo.isDir()) {
            cppFilesList(QDir(finfo.absoluteFilePath()));
        }
    }

    return filesList;
}

void ApiCovTool::getResult(const QString &cppFileName ,QString &headerFile, HeaderParser & parseData)
{
    qDebug()<<"IN GET RESULT";
    QString outputFileName, unUsedData;
    QString directory;
    if(resultDirectory.endsWith("/"))
        directory =  resultDirectory;
    else
        directory =  resultDirectory.append("/");

     qDebug()<<"headerFile:"<<headerFile<<__LINE__;
     qDebug()<<"cppFileName:"<<cppFileName<<__LINE__;
    ((outputFileName.append(headerFile.left(headerFile.indexOf('.')))).append("_Result.txt")).prepend(directory);
    qDebug()<<"outputFileName:"<<outputFileName<<__LINE__;


    QFile outputFile(outputFileName);
    QByteArray fileData;

    QStringList unUsedPublicMethods,unUsedEnumVariables,
                  unUsedEnumTstVariables,unUsedPublicTstMethods;
     QStringList unUsedSlotMethods, unUsedSignalMethods,
                  unUsedSlotTstMethods,unUsedSignalTstMethods;
     int enumCoveredPercent,publicMethodCoveredPercent,enumTstCoveredPercent,
         slotMethodCoveredPercent,slotTstMethodCoveredPercent ,signalMethodCoveredPercent,
         signalTstMethodCoveredPercent, publicTstMethodCoveredPercent;

     int enumCovered,publicMethodsCovered,enumTstCovered,publicTstMethodsCovered,slotMethodsCovered,
      slotTstMethodsCovered,signalMethodsCovered,signalTstMethodsCovered;
     QFile cppFile(cppFileName);

     if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<<__FILE__<<__LINE__;
        return;
    }

    if (!cppFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<__FILE__<<__LINE__;
        return;
    }

    QByteArray cppFileByteData = cppFile.readAll();

    classNames = parseData.getClassNames();

    QMap <QString, QStringList> classBlocks = parseData.getClassBocks();

    enumCovered = checkEnumData( enumVariables, unUsedEnumVariables, cppFileByteData);

    enumTstCovered = checkEnumTstData(enumTstVariables, unUsedEnumTstVariables,cppFileByteData);



    if(classNames.size() ==1)

        fileData.append("Header File  :"+headerFile+"\n" );
    else
    {

        fileData.append("Header File  :"+headerFile+"\n" );

        fileData.append("Total Number of Classes :"+QString("%1").arg(classNames.size())+"\n" );

        fileData.append ("LIST OF CLASS NAMES :\n");

        for(int iVar= 0;iVar<classNames.size();iVar++)
        {

            fileData.append("\t\t\t"+classNames.at(iVar)+"\n");
        }
        fileData.append("===================================================================================================\n");
    }
    //fileData.clear();
    for(int iCount = 0; iCount < classNames.size(); iCount++)
    {
        unUsedPublicMethods.clear();
        unUsedSlotMethods.clear();
        unUsedSlotMethods.clear();
        QStringList classData = classBlocks.value(classNames.at(iCount));

        publicMethods = parseData.getPublicMethods(classNames.at(iCount),classData);

        slotMethods   = parseData.getSlotMethods(classNames.at(iCount),classData);

        signalMethods = parseData.getSignalMethods(classNames.at(iCount),classData);

        publicMethodsCovered = checkPublicMethodsData(publicMethods, unUsedPublicMethods,cppFileByteData);

       // publicTstMethodsCovered = checkPublicTstMethodsData(publicTstMethods,unUsedPublicTstMethods,cppFileByteData);
        slotMethodsCovered = checkSlotMethodsData(slotMethods,unUsedSlotMethods, cppFileByteData);

        //slotTstMethodsCovered = checkSlotTstMethodsData(slotTstMethods,unUsedSlotTstMethods, cppFileByteData);
        signalMethodsCovered = checkSlotMethodsData(signalMethods,unUsedSignalMethods, cppFileByteData);

       // signalTstMethodsCovered = checkSlotTstMethodsData(signalTstMethods,unUsedSignalTstMethods, cppFileByteData);




    if(publicMethodsCovered == (publicMethods.uniqueKeys().length()))
        publicMethodCoveredPercent = 100;
    else
        publicMethodCoveredPercent = (float)((publicMethodsCovered)*100)/(float)(publicMethods.uniqueKeys().length());

    if(publicTstMethodsCovered == (publicTstMethods.length()))
        publicTstMethodCoveredPercent = 100;
    else
        publicTstMethodCoveredPercent = (float)((publicTstMethodsCovered)*100)/(float)(publicTstMethods.length());

    if(slotMethodsCovered == (slotMethods.size()))
        slotMethodCoveredPercent = 100;
    else
        slotMethodCoveredPercent = (float)((slotMethodsCovered)*100)/(float)(slotMethods.size());

    if(slotTstMethodsCovered == (slotTstMethods.length()))
        slotTstMethodCoveredPercent = 100;
    else
        slotTstMethodCoveredPercent = (float)((slotTstMethodsCovered)*100)/(float)(slotTstMethods.length());

    if(signalMethodsCovered == (signalMethods.size()))
        signalMethodCoveredPercent = 100;
    else
        signalMethodCoveredPercent = (float)((signalMethodsCovered)*100)/(float)(signalMethods.size());

    if(signalTstMethodsCovered == (signalTstMethods.length()))
        signalTstMethodCoveredPercent = 100;
    else
        signalTstMethodCoveredPercent = (float)((signalTstMethodsCovered)*100)/(float)(signalTstMethods.length());


    fileData.append("ClassName : "+classNames.at(iCount)+"\n");
    fileData.append("PublicMethodsCoverage : "+QString("%1").arg(publicMethodsCovered)+"/"+
                    QString("%1").arg(publicMethods.uniqueKeys().length())+"("+QString("%1").arg(publicMethodCoveredPercent)+"%)\n");
    fileData.append("slotsMethodsCoverage : "+QString("%1").arg(slotMethodsCovered)+"/"+
                    QString("%1").arg(slotMethods.uniqueKeys().length())+"("+QString("%1").arg(slotMethodCoveredPercent)+"%)\n");
    fileData.append("SignalMethodsCoverage : "+QString("%1").arg(signalMethodsCovered)+"/"+
                    QString("%1").arg(signalMethods.uniqueKeys().length())+"("+QString("%1").arg(signalMethodCoveredPercent)+"%)\n");


    //fileData.append("\t\t\t\tTotal Test Individuality : \n");
    //fileData.append("\nTst EnumCoverage : "+ QString("%1").arg(enumTstCovered)+"/"+ QString("%1").arg(totalEnumTstDataList.length())+"("+QString("%1").arg(enumTstCoveredPercent)+"%)\n");
    //fileData.append("PublicMethodsCoverage : "+QString("%1").arg(publicMethodsTstCovered)+"/"+ QString("%1").arg(publicMethodsTstDataList.length())+"("+QString("%1").arg(publicMethodTstCoveredPercent)+"%)\n");
    //fileData.append("PublicMethodsCoverage : "+QString("%1").arg(publicMethodsTstCovered)+"/"+ QString("%1").arg(publicMethodsTstDataList.length())+"("+QString("%1").arg(publicMethodTstCoveredPercent)+"%)\n");
    //fileData.append("PublicMethodsCoverage : "+QString("%1").arg(publicMethodsTstCovered)+"/"+ QString("%1").arg(publicMethodsTstDataList.length())+"("+QString("%1").arg(publicMethodTstCoveredPercent)+"%)\n");




    // fileData.append(unUsedData);
     fileData.append("\nNonCovered Public Methods :\n");
     unUsedData.clear();

     if(publicMethodsCovered == (publicMethods.uniqueKeys().length()))
         unUsedData.append("\t\t NONE\n");
      else
      {
              for(int i=0; i<unUsedPublicMethods.length(); i++)
              {
                  unUsedData += "\t\t";
                  unUsedData += QString("%1").arg(i+1);
                  unUsedData += ")";
                  unUsedData +=classNames.at(iCount);
                  unUsedData +="::";
                  unUsedData += unUsedPublicMethods.at(i);
                  unUsedData += "()\n";
              }

       }
      fileData.append(unUsedData);

      //For Non covered Slots

      fileData.append("\nNonCovered SLOTS Methods :\n");
       unUsedData.clear();
     if(slotMethodsCovered == (slotMethods.uniqueKeys().length()))
          unUsedData.append("\t\t NONE\n");
       else
       {
               for(int i=0; i<unUsedSlotMethods.length(); i++)
               {
                   unUsedData += "\t\t";
                   unUsedData += QString("%1").arg(i+1);
                   unUsedData += ")";
                   unUsedData +=classNames.at(iCount);
                   unUsedData +="::";
                   unUsedData += unUsedSlotMethods.at(i);
                   unUsedData += "()\n";
               }

        }
       fileData.append(unUsedData);
       //for uncovered slots
       fileData.append("\nNonCovered SIGNAL Methods :\n");
       unUsedData.clear();
      if(signalMethodsCovered == (signalMethods.uniqueKeys().length()))
           unUsedData.append("\t\t NONE\n");
        else
        {
                for(int i=0; i<unUsedSignalMethods.length(); i++)
                {
                    unUsedData += "\t\t";
                    unUsedData += QString("%1").arg(i+1);
                    unUsedData += ")";
                    unUsedData +=classNames.at(iCount);
                    unUsedData +="::";
                    unUsedData += unUsedSignalMethods.at(i);
                    unUsedData += "()\n";
                }

         }

    fileData.append(unUsedData);

#if 0
      fileData.append("Total Uncovered Test Case :\n");
      Start

      int iCount;


       fileData.append(unUsedData);
       fileData.append("\nNonCovered TST Public Methods :\n");
       unUsedData.clear();
      if(publicTstMethodsCovered == (publicTstMethods.length()))
           unUsedData.append("\t\t NONE\n");
        else
        {
                for(int i=0; i<unUsedPublicTstMethods.length(); i++)
                {
                    unUsedData += "\t\t";
                    unUsedData += QString("%1").arg(i+1);
                    unUsedData += ")";
                    unUsedData +=className;
                    unUsedData +="::";
                    unUsedData += unUsedPublicTstMethods.at(i);
                   // unUsedData += "()\n";
                }

         }
    fileData.append(unUsedData);
#endif
    fileData.append("______________________________________________________________________________________________________________\n\n");
    outputFile.write(fileData);
    fileData.clear();

}


    if(enumCovered == enumVariables.length())
        enumCoveredPercent = 100;
    else
        enumCoveredPercent = (float)((enumCovered)*100)/(float)enumVariables.length();

    if(enumTstCovered == enumTstVariables.length())
        enumTstCoveredPercent = 100;
    else
        enumTstCoveredPercent = (float)((enumTstCovered)*100)/(float)enumTstVariables.length();
    fileData.append("**************************************************************")  ;
    fileData.append("\nEnumCoverage : "+ QString("%1").arg(enumCovered)+"/"+
                           QString("%1").arg(enumVariables.length())+"("+QString("%1").arg(enumCoveredPercent)+"%)\n");
    fileData.append("\nTst EnumCoverage : "+ QString("%1").arg(enumTstCovered)+"/"+ QString("%1").arg(enumTstVariables.length())+
                            "("+QString("%1").arg(enumTstCoveredPercent)+"%)\n");



    fileData.append("NonCovered ENUMS :\n");

     unUsedData.clear();
    //int iCount;
    if(enumCovered == enumVariables.length())
        unUsedData.append("\t\t NONE\n");
     else
     {
             for(int i=0; i<unUsedEnumVariables.length(); i++)
             {
                 unUsedData += "\t\t";
                 unUsedData += QString("%1").arg(i+1);
                 unUsedData += ")";
                 unUsedData += unUsedEnumVariables.at(i);
                 unUsedData += "\n";
             }

      }
     fileData.append(unUsedData);
     unUsedData.clear();
    fileData.append("NonCovered TST ENUMS :\n");
    if(enumTstCovered == enumTstVariables.length())
        unUsedData.append("\t\t NONE\n");
     else
     {
             for(int i=0; i<unUsedEnumTstVariables.length(); i++)
             {
                 unUsedData += "\t\t";
                 unUsedData += QString("%1").arg(i+1);
                 unUsedData += ")";
                 unUsedData += unUsedEnumTstVariables.at(i);
                 unUsedData += "\n";
             }

      }
    fileData.append(unUsedData);
     fileData.append("**************************************************************")  ;
     outputFile.write(fileData);

     unUsedData.clear();
    outputFile.close();

}

int ApiCovTool::checkEnumTstData(QStringList totalEnumsData ,QStringList & unUsedEnums ,QByteArray cppFileByteData)
{
   int noEnums = totalEnumsData.length();
   int iCount = 0;
   QByteArray a;
   QString enumData;
//   className.append("::");//scope resolution operator


   while(iCount < noEnums)
   {
      enumData = totalEnumsData.at(iCount);
      if (cppFileByteData.contains(enumData.toAscii()) == false)
          unUsedEnums.append(totalEnumsData.at(iCount));
      iCount++;
   }
   if(unUsedEnums.length()==0)
       return  noEnums;
   else
       return (noEnums-unUsedEnums.length());//((float)((noEnums-unUsedEnums.length())*100)/(float)noEnums);
}

int ApiCovTool::checkEnumData(QStringList totalEnumsData ,QStringList & unUsedEnums ,QByteArray cppFileByteData)
{
   int noEnums = totalEnumsData.length();
   int iCount = 0;
   QByteArray a;
   QString enumData;
//   className.append("::");//scope resolution operator


   while(iCount < noEnums)
   {
      enumData = totalEnumsData.at(iCount);
      if (cppFileByteData.contains(enumData.toAscii()) == false)
          unUsedEnums.append(totalEnumsData.at(iCount));
      iCount++;
   }
   if(unUsedEnums.length()==0)
       return  noEnums;
   else
       return (noEnums-unUsedEnums.length());//((float)((noEnums-unUsedEnums.length())*100)/(float)noEnums);
}


int ApiCovTool::checkPublicMethodsData(QMap<QString , QStringList>/*QStringList*/ totalPublicMethodsData ,QStringList & unUsedPublicMethods ,QByteArray cppFileByteData)
   {
        QStringList totalPublicMethods = totalPublicMethodsData.uniqueKeys();
        int noPublicMethods = totalPublicMethodsData.count();

        int iCount = 0;
        QByteArray a;
        QString publicMethodsData;
        while(iCount < noPublicMethods)
        {
           publicMethodsData = totalPublicMethods.at(iCount);
           if (cppFileByteData.contains(publicMethodsData.toAscii()) == false)
               unUsedPublicMethods.append(totalPublicMethods.at(iCount));
           iCount++;
        }
        if(unUsedPublicMethods.length()==0)
            return noPublicMethods;
        else
            return(noPublicMethods-unUsedPublicMethods.length());
   }

int ApiCovTool::checkPublicTstMethodsData(QStringList publicMethodsTstDataList, QStringList &unUsedPublicMethodsTstList,  QByteArray cppFileByteData)
{
    int noPublicMethods = publicMethodsTstDataList.count();

    int iCount = 0;
    QByteArray a;
    QString publicMethodsTstData;
    while(iCount < noPublicMethods)
    {
       publicMethodsTstData = publicMethodsTstDataList.at(iCount);
       if (cppFileByteData.contains(publicMethodsTstData.toAscii()) == false)
           unUsedPublicMethodsTstList.append(publicMethodsTstDataList.at(iCount));
       iCount++;
    }
    if(unUsedPublicMethodsTstList.length()==0)
        return noPublicMethods;
    else
        return(noPublicMethods-unUsedPublicMethodsTstList.length());

}

int ApiCovTool::checkSlotMethodsData(QMap<QString , QStringList>/*QStringList*/ totalPublicMethodsData ,QStringList & unUsedPublicMethods ,QByteArray cppFileByteData)
   {
        QStringList totalPublicMethods = totalPublicMethodsData.uniqueKeys();
        int noPublicMethods = totalPublicMethodsData.count();

        int iCount = 0;
        QByteArray a;
        QString publicMethodsData;
        while(iCount < noPublicMethods)
        {
           publicMethodsData = totalPublicMethods.at(iCount);
           if (cppFileByteData.contains(publicMethodsData.toAscii()) == false)
               unUsedPublicMethods.append(totalPublicMethods.at(iCount));
           iCount++;
        }
        if(unUsedPublicMethods.length()==0)
            return noPublicMethods;
        else
            return(noPublicMethods-unUsedPublicMethods.length());
   }

int ApiCovTool::checkSlotTstMethodsData(QStringList publicMethodsTstDataList, QStringList &unUsedPublicMethodsTstList,  QByteArray cppFileByteData)
{
    int noPublicMethods = publicMethodsTstDataList.count();

    int iCount = 0;
    QByteArray a;
    QString publicMethodsTstData;
    while(iCount < noPublicMethods)
    {
       publicMethodsTstData = publicMethodsTstDataList.at(iCount);
       if (cppFileByteData.contains(publicMethodsTstData.toAscii()) == false)
           unUsedPublicMethodsTstList.append(publicMethodsTstDataList.at(iCount));
       iCount++;
    }
    if(unUsedPublicMethodsTstList.length()==0)
        return noPublicMethods;
    else
        return(noPublicMethods-unUsedPublicMethodsTstList.length());

}

int ApiCovTool::checkSignalMethodsData(QMap<QString , QStringList>/*QStringList*/ totalPublicMethodsData ,QStringList & unUsedPublicMethods ,QByteArray cppFileByteData)
   {
        QStringList totalPublicMethods = totalPublicMethodsData.uniqueKeys();
        int noPublicMethods = totalPublicMethodsData.count();

        int iCount = 0;
        QByteArray a;
        QString publicMethodsData;
        while(iCount < noPublicMethods)
        {
           publicMethodsData = totalPublicMethods.at(iCount);
           if (cppFileByteData.contains(publicMethodsData.toAscii()) == false)
               unUsedPublicMethods.append(totalPublicMethods.at(iCount));
           iCount++;
        }
        if(unUsedPublicMethods.length()==0)
            return noPublicMethods;
        else
            return(noPublicMethods-unUsedPublicMethods.length());
   }


int ApiCovTool::checkSignalTstMethodsData(QStringList publicMethodsTstDataList, QStringList &unUsedPublicMethodsTstList,  QByteArray cppFileByteData)
{
    int noPublicMethods = publicMethodsTstDataList.count();

    int iCount = 0;
    QByteArray a;
    QString publicMethodsTstData;
    while(iCount < noPublicMethods)
    {
       publicMethodsTstData = publicMethodsTstDataList.at(iCount);
       if (cppFileByteData.contains(publicMethodsTstData.toAscii()) == false)
           unUsedPublicMethodsTstList.append(publicMethodsTstDataList.at(iCount));
       iCount++;
    }
    if(unUsedPublicMethodsTstList.length()==0)
        return noPublicMethods;
    else
        return(noPublicMethods-unUsedPublicMethodsTstList.length());

}

