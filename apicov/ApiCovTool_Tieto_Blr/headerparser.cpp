#include "headerparser.h"
#include <QtGlobal>
#include "QDebug"



HeaderParser::HeaderParser(QByteArray headerinfo)
{
    this->headerFileInfo = headerinfo ;
    if(headerFileInfo.isEmpty())
    {
        //qDebug()<<"EMPTY FILE"<<__FILE__<<__LINE__;
        return;
    }

    headerFileInfo = headerFileInfo.trimmed();

    headerFileInfo = headerFileInfo.replace("const",""); // added by avinash to reove const variable from entire file

    QString fileInfo(headerFileInfo);

    fileInfoLineData = fileInfo.split('\n');
    int var = 0;
    while(var < fileInfoLineData.length())
    {
        if(fileInfoLineData.at(var).isEmpty())
        {
            fileInfoLineData.removeAt(var);
        }
        else
        var++;
    }    
}

//****Returns Class Name**********//

QString HeaderParser::getClassName()
{
    QString iterator;

    foreach(iterator, fileInfoLineData) {

        if(iterator.contains("class") && !iterator.contains(";") && !iterator.contains("}"))//need to handle classes like "class a {int a};"
        {
            if(iterator.contains(":"))
            {
                int pos = iterator.indexOf(":");
                QString str = iterator.left(pos);
                str = str.simplified();
                QStringList list = str.split(" ");
                QString className = list.takeLast();
                return className;
            }
            else
            {
                iterator = iterator.simplified();
                QStringList list = iterator.split(" ");
                QString className = list.takeLast();
                return className;
            }
        }
    }
}

QStringList HeaderParser::getClassNames()
{
    QString iterator;
    QStringList classNames;

    foreach(iterator, fileInfoLineData) {

        if(iterator.contains("class") && !iterator.contains(";") && !iterator.contains("}"))//need to handle classes like "class a {int a};"
        {
            if(iterator.contains(":"))
            {
                int pos = iterator.indexOf(":");
                QString str = iterator.left(pos);
                str = str.simplified();
                QStringList list = str.split(" ");
                QString className = list.takeLast();
                classNames.append(className);
            }
            else
            {
                iterator = iterator.simplified();
                QStringList list = iterator.split(" ");
                QString className = list.takeLast();
                classNames.append(className);
            }
        }
    }

    return classNames;
}

/*****  Returns List of enums *******/

QStringList HeaderParser::getEnumData()
{
    QStringList enumDataList,enumVariables,allEnums;

    QString enumData;

    enumDataList = this->getEnumsRawData();

    for(int i=0; i< enumDataList.length(); i++)
    {
         enumData = (enumDataList.at(i));         
         enumData = enumData.remove( 0, (enumData.indexOf("{") + 1));
         enumData  = enumData.left(enumData.lastIndexOf("}")); //chops }
         enumVariables.append((enumData.replace(" " ,"")).split(","));
         enumData.clear();
    }

    enumVariables.removeAll("");
    QString iterator;

    foreach(iterator,enumVariables)
    {
           if(iterator.contains("=")) {

             QStringList list = iterator.split("=");

             iterator = list.at(0);

             iterator = iterator.trimmed();
           }

          allEnums.append(iterator);

      }
     return allEnums;
}

/**** ENUM TESTCASES DATA LIST ********/

QStringList HeaderParser::getEnumTstData()
{
    QStringList enumMembersList = this->getEnumData();

    QStringList enumTstDataList;

    QString iterator;
    foreach(iterator,enumMembersList)
    {
        iterator = iterator.prepend("tst_");
        iterator.append("()");
        enumTstDataList.append(iterator);
    }

    return enumTstDataList;
}

QStringList HeaderParser::getEnumsRawData()
{
    QString iterator;
    int var = 0;
    QString enumBlock;
    QStringList allEnumsList;
    bool inEnumLoop = false;

    while (var < fileInfoLineData.size())    
    {
        iterator = fileInfoLineData.at(var);

        if(iterator.contains("enum"))
        {
            inEnumLoop = true;           
            if(iterator.contains(";"))
            {
                if(iterator.contains("{") && iterator.contains("}"))
                {                  
                    enumBlock.append(iterator);
                    enumBlock = enumBlock.simplified();
                    allEnumsList.append(enumBlock);
                    enumBlock.clear();//Added by avinash to remove recursivive adidtion eg:a,aa,aab,aac
                    fileInfoLineData.removeAt(var);
                    inEnumLoop = false;//added by avinash to make the loop to come to an end
                }else
                {                  
                    fileInfoLineData.removeAt(var);
                }
            } else
            {               
                enumBlock.append(iterator);
                fileInfoLineData.removeAt(var);
            }
        }else if(inEnumLoop == true)
        {           
            enumBlock.append(iterator);
            if(iterator.contains(";"))
            {               
                enumBlock =  enumBlock.simplified();
                enumBlock = enumBlock.trimmed();
                allEnumsList.append(enumBlock);
                enumBlock.clear();//Added by avinash to remove recursivive adidtion eg:a,aa,aab,aac
                inEnumLoop = false;
            }
            fileInfoLineData.removeAt(var);
        }  else
               var++;
    }
    return allEnumsList;
}

/*********** Functions for returning the Qmap list of Methods*********/


/****Returns QMAP list of Public Methods *************/

QMap<QString, QStringList> HeaderParser::getPublicMethods(QString className,QStringList &classData)
{
    QStringList publicMethodsDataList;
    QMap<QString , QStringList> publicMethods;
    QStringList publicMethodsList;

    publicMethodsDataList =  this->getPublicMethodsRawData(className,classData);

    QString string;

   for (int iCount = 0; iCount<publicMethodsDataList.size() ; iCount++)
   {

      string = publicMethodsDataList.at(iCount);

       if(string.contains("(") && string.contains(")") /*&& string.contains(";")*/) {

       QStringList argList;
       QString temp = string;
       int endPos = temp.indexOf('(');
       temp =  temp.left(endPos);     //chops all data present after the pUblic method Name.
       temp.remove("&");
       temp = temp.remove("*");
       int  startPos= temp.lastIndexOf(' ');
       temp = temp.remove(0,startPos); //removes return type of publicmethods
       temp = temp.simplified();       

       QString arg = string;
       arg.remove("const");

       startPos = arg.indexOf('(');
       arg = arg.mid(startPos + 1);

       startPos = arg.indexOf(')');
       arg = arg.left(startPos);
       arg = arg.simplified();


       QStringList list = arg.split(",",QString::SkipEmptyParts);
       QString str;

       foreach(str, list) {
           str = str.simplified();
           if(str.contains("*")) {
               int pos = str.indexOf("*");
               str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);

           } else if (str.contains("&")) {
               int pos = str.indexOf("&");
                str = str.left(pos+1);
               //str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);
           }
           else{
               int pos = str.indexOf(" ");
             //  str.truncate(pos);
               str = str.left(pos);
               str = str.simplified();
               argList.append(str);
           }

       }
       if(publicMethods.contains(temp)) {
           for(int i = 2; i <=10 ; i++) {//for overloaded functions
               temp = temp.remove(QRegExp("[0123456789]"));
               temp = temp.append(QString::number(i));
               if(!publicMethods.contains(temp)) {
                   break;
               }
           }
       }

       publicMethods.insertMulti(temp,argList);
       publicMethodsList.append(temp);

     }
 }
   return publicMethods;
}


//UPDATED SLOT  METHOD
QMap<QString, QStringList> HeaderParser::getSlotMethods(QString className,QStringList &classData)
{
    QStringList slotMethodsDataList;
    QMap<QString , QStringList> slotMethods;
    QStringList slotMethodsList;

    slotMethodsDataList =  this->getSlotMethodsRawData(className,classData);

    QString string;

   for (int iCount = 0; iCount<slotMethodsDataList.size() ; iCount++)
   {

      string = slotMethodsDataList.at(iCount);

       if(string.contains("(") && string.contains(")") /*&& string.contains(";")*/) {

       QStringList argList;
       QString temp = string;
       int endPos = temp.indexOf('(');
       temp =  temp.left(endPos);     //chops all data present after the pUblic method Name.
       temp.remove("&");
       temp = temp.remove("*");
       int  startPos= temp.lastIndexOf(' ');
       temp = temp.remove(0,startPos); //removes return type of publicmethods
       temp = temp.simplified();

       QString arg = string;
       arg.remove("const");

       startPos = arg.indexOf('(');
       arg = arg.mid(startPos + 1);

       startPos = arg.indexOf(')');
       arg = arg.left(startPos);
       arg = arg.simplified();


       QStringList list = arg.split(",",QString::SkipEmptyParts);
       QString str;

       foreach(str, list) {
           str = str.simplified();
           if(str.contains("*")) {
               int pos = str.indexOf("*");
               str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);

           } else if (str.contains("&")) {
               int pos = str.indexOf("&");
                str = str.left(pos+1);
               //str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);
           }
           else{
               int pos = str.indexOf(" ");
             //  str.truncate(pos);
               str = str.left(pos);
               str = str.simplified();
               argList.append(str);
           }

       }
       if(slotMethods.contains(temp)) {
           for(int i = 2; i <=10 ; i++) {//for overloaded functions
               temp = temp.remove(QRegExp("[0123456789]"));
               temp = temp.append(QString::number(i));
               if(!slotMethods.contains(temp)) {
                   break;
               }
           }
       }

       slotMethods.insertMulti(temp,argList);
       slotMethodsList.append(temp);

     }
 }
  // qDebug()<<"SLOTMETHODS:"<<slotMethods<<__LINE__;
   return slotMethods;
}


//Updated Signal method

QMap<QString, QStringList> HeaderParser::getSignalMethods(QString className,QStringList & classData)
{
    QStringList signalMethodsDataList;
    QMap<QString , QStringList> signalMethods;
    QStringList signalMethodsList;

    signalMethodsDataList =  this->getSignalMethodsRawData(className,classData);

    QString string;

   for (int iCount = 0; iCount<signalMethodsDataList.size() ; iCount++)
   {

      string = signalMethodsDataList.at(iCount);

       if(string.contains("(") && string.contains(")") /*&& string.contains(";")*/) {

       QStringList argList;
       QString temp = string;
       int endPos = temp.indexOf('(');
       temp =  temp.left(endPos);     //chops all data present after the pUblic method Name.
       temp.remove("&");
       temp = temp.remove("*");
       int  startPos= temp.lastIndexOf(' ');
       temp = temp.remove(0,startPos); //removes return type of publicmethods
       temp = temp.simplified();

       QString arg = string;
       arg.remove("const");

       startPos = arg.indexOf('(');
       arg = arg.mid(startPos + 1);

       startPos = arg.indexOf(')');
       arg = arg.left(startPos);
       arg = arg.simplified();


       QStringList list = arg.split(",",QString::SkipEmptyParts);
       QString str;

       foreach(str, list) {
           str = str.simplified();
           if(str.contains("*")) {
               int pos = str.indexOf("*");
               str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);

           } else if (str.contains("&")) {
               int pos = str.indexOf("&");
                str = str.left(pos+1);
               //str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);
           }
           else{
               int pos = str.indexOf(" ");
             //  str.truncate(pos);
               str = str.left(pos);
               str = str.simplified();
               argList.append(str);
           }

       }
       if(signalMethods.contains(temp)) {
           for(int i = 2; i <=10 ; i++) {//for overloaded functions
               temp = temp.remove(QRegExp("[0123456789]"));
               temp = temp.append(QString::number(i));
               if(!signalMethods.contains(temp)) {
                   break;
               }
           }
       }

       signalMethods.insertMulti(temp,argList);
       signalMethodsList.append(temp);

     }
 }
  // qDebug()<<"SIGNAL METHODS:"<<signalMethods<<__LINE__;
   return signalMethods;
}



QStringList HeaderParser::getPublicTstMethodsData()
{
    QMap <QString,QStringList> list;// = this->getPublicMethods();
    QList<QString> publicMethods = list.keys();
    QString iterator;
    QStringList publicMethodsTstData;
    foreach(iterator,publicMethods) {
        iterator = iterator.prepend("tst_");
        iterator = iterator.append("()");
        publicMethodsTstData.append(iterator);
    }

    return publicMethodsTstData;
}



QStringList HeaderParser::getSlotTstMethodsData()
{
    QMap <QString,QStringList> list ;//= this->getSlotMethods();
    QList<QString> slotMethods = list.keys();
    QString iterator;
    QStringList slotMethodsTstData;
    foreach(iterator,slotMethods) {
        iterator = iterator.prepend("tst_");
        iterator = iterator.append("()");
        slotMethodsTstData.append(iterator);
    }

    return slotMethodsTstData;
}




QStringList HeaderParser::getSignalTstMethodsData()
{
    QMap <QString,QStringList> list;// = this->getSignalMethods();
    QList<QString> signalMethods = list.keys();
    QString iterator;
    QStringList signalMethodsTstData;
    foreach(iterator,signalMethods) {
        iterator = iterator.prepend("tst_");
        iterator = iterator.append("()");
        signalMethodsTstData.append(iterator);
    }

    return signalMethodsTstData;
}

/*********** Functions for raw data *****************/

/******** Returns the QstringList of Valid Public method  Data ***********/


#if 1
QStringList HeaderParser::getPublicMethodsRawData(QString className,QStringList &classBlock)
{
    QString iterator;
    int var = 0;
    QString publicMethodsBlock;
    QStringList allPublicMethodsList;
    bool inPublic = false;
    QStringList fileInfoLineData = classBlock;

    while (var < fileInfoLineData.length())
    {
        iterator = fileInfoLineData.at(var);        
        iterator = iterator.simplified();

        if(iterator.contains("public") && iterator.contains(":") && (iterator.length() <= 8)) //pure public methods
        {     
            inPublic = true;
            fileInfoLineData.removeAt(var);
        } else
        {            
            if(inPublic == true)
            {         
               // publicMethodsBlock.append(iterator);commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                if(iterator.contains(":") && !iterator.contains("::") || iterator.contains("};"))
                {        
                    inPublic = false;
                   // fileInfoLineData.removeAt(var);// commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    break;
                }
                else
                {
                    publicMethodsBlock.append(iterator);//commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    fileInfoLineData.removeAt(var);
                }
            }else
                var ++;
        }
    }

    publicMethodsBlock = publicMethodsBlock.simplified();

    this->removeDefinitions(publicMethodsBlock);
    allPublicMethodsList = removeProperties(className,publicMethodsBlock);
    qDebug()<<"allPublicMethodsList"<<allPublicMethodsList<<__LINE__;

    return allPublicMethodsList;
}
#endif

QStringList HeaderParser::getSlotMethodsRawData(QString className,QStringList &classBlock)
{
    QString iterator;
    int var = 0;
    QString slotMethodsBlock;
    QStringList allSlotMethodsList;
    bool inSlot = false;
    QStringList fileInfoLineData = classBlock;

    while (var < fileInfoLineData.length())
    {
        iterator = fileInfoLineData.at(var);
        iterator = iterator.simplified();

        if(((iterator.contains("slots") || (iterator.contains("Q_SLOTS"))) && (iterator.contains(":")) && (!iterator.contains("private")) && (!iterator.contains("protected")))) //pure Slot methods
        {
            inSlot = true;
            fileInfoLineData.removeAt(var);
        } else
        {
            if(inSlot == true)
            {
               // publicMethodsBlock.append(iterator);commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                if(iterator.contains(":") && !iterator.contains("::") || iterator.contains("};"))
                {
                    inSlot = false;
                   // fileInfoLineData.removeAt(var);// commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    break;
                }
                else
                {
                    slotMethodsBlock.append(iterator);//commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    fileInfoLineData.removeAt(var);
                }
            }else
                var ++;
        }
    }

    slotMethodsBlock = slotMethodsBlock.simplified();

    this->removeDefinitions(slotMethodsBlock);
    allSlotMethodsList = removeProperties(className,slotMethodsBlock);
  //  qDebug()<<"SLOT_METHODS :"<<allSlotMethodsList;
    return allSlotMethodsList;
}

//Updated Signal method


QStringList HeaderParser::getSignalMethodsRawData(QString className,QStringList &classBlock)
{
    QString iterator;
    int var = 0;
    QString signalMethodsBlock;
    QStringList allSignalMethodsList;
    bool inSignal = false;
    QStringList fileInfoLineData = classBlock;

    while (var < fileInfoLineData.length())
    {
        iterator = fileInfoLineData.at(var);
        iterator = iterator.simplified();

        if(((iterator.contains("signals") || (iterator.contains("Q_SIGNALS"))) && (iterator.contains(":")) && (!iterator.contains("private")) && (!iterator.contains("protected")))) //pure Slot methods
        {
            inSignal = true;
            fileInfoLineData.removeAt(var);
        } else
        {
            if(inSignal == true)
            {
               // publicMethodsBlock.append(iterator);commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                if(iterator.contains(":") && !iterator.contains("::") || iterator.contains("};"))
                {
                    inSignal = false;
                   // fileInfoLineData.removeAt(var);// commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    break;
                }
                else
                {
                    signalMethodsBlock.append(iterator);//commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    fileInfoLineData.removeAt(var);
                }
            }else
                var ++;
        }
    }

    signalMethodsBlock = signalMethodsBlock.simplified();

    this->removeDefinitions(signalMethodsBlock);
    allSignalMethodsList = removeProperties(className,signalMethodsBlock);
   // qDebug()<<"SLOT_METHODS :"<<allSignalMethodsList;
    return allSignalMethodsList;
}


QMap <QString,QStringList> HeaderParser::getClassBocks()
{
    QString iterator;
    int var = 0;
    QStringList classBlock;
    QStringList classNames = this->getClassNames();

    QMap <QString,QStringList> classBlocks;

    bool inClass = false;

    for (int i = 0; i < classNames.size() ; i ++)
    {
       // qDebug()<<__FILE__<<__LINE__;
        int count = 0;

        while (var < fileInfoLineData.length())
        {            
            iterator = fileInfoLineData.at(var);
            iterator = iterator.simplified();

            if(iterator.contains("class") && iterator.contains(classNames.at(i)) && !iterator.contains(";") && !iterator.contains("}")) //pure public methods
            {
              //  qDebug()<<__FILE__<<__LINE__;
                count = count + iterator.count('{');
                inClass = true;
                classBlock.append(iterator);
                var++;
            } else
            {
                if(inClass == true)
                {
                    qDebug()<<"iterator"<<iterator<<__FILE__<<__LINE__;
                    count = count + iterator.count('{');
                    qDebug()<<"count :"<<count<<__FILE__<<__LINE__;

                    count = count - iterator.count('}');
                    qDebug()<<"count :"<<count<<__FILE__<<__LINE__;
                    if(count == 0)
                    {
                        qDebug()<<__FILE__<<__LINE__;
                        inClass = false;
                        classBlock.append(iterator);
                        var++;
                        break;
                    }else
                    {
                        classBlock.append(iterator);
                        var++;
                        qDebug()<<__FILE__<<__LINE__;

                    }
                }else {
                    qDebug()<<__FILE__<<__LINE__;
                    var++;
                }

            }
        }

        classBlocks.insert(classNames.at(i),classBlock);
        classBlock.clear();
        qDebug()<<__FILE__<<__LINE__;
    }
    return classBlocks;
}



/****************Generic Functions**********************/

/***** Removes definations of a method and replaces it by ; ***********/

void HeaderParser::removeDefinitions(QString &data)
{
    int loopVar = 0;
    int i = 0;
    int startingBrace = 0;
    int closingBrace = 0;
    bool insideBrace = false;

    while(loopVar < data.length())
    {
        if( data.at(loopVar) == '{')
        {
           insideBrace = true;         
            if(i == 0)
                startingBrace =  loopVar;
            i++;
            loopVar ++;
        }else if( data.at(loopVar) == '}')
        {        
            closingBrace =  loopVar;
            i--;
            loopVar ++;
        }else if( insideBrace == true && i == 0)
        {        
            insideBrace = false;

            data = data.remove(startingBrace,(closingBrace - startingBrace)+1);
            data.insert(startingBrace,";");
            loopVar = 0;
        } else
        {            
            loopVar ++;
        }
    }

}
/************************** Removes Properties from the methods and returns the list of the Public methods********/

QStringList HeaderParser::removeProperties(QString className,QString &data)
{  
    int loopVar = 0;
    int i = 0;
    int startingBrace = 0;
    int closingBrace = 0;
    bool insideBrace = false;

    while(loopVar < data.length())
    {
        if( data.at(loopVar) == '(')
        {
           insideBrace = true;       
            if(i == 0)
                startingBrace =  loopVar;
            i++;
            loopVar ++;
        }else if( data.at(loopVar) == ')')
        {          
            closingBrace =  loopVar;
            i--;
            loopVar ++;
        }else if( insideBrace == true && i == 0)
        {        
            insideBrace = false;

            //data = data.remove(startingBrace,(closingBrace - startingBrace)+1);
            data.insert(closingBrace + 1,";");
            loopVar ++;
        } else
        {            
            loopVar ++;
        }
    }

    data = data.simplified();
    data = data.trimmed();
    data.replace("; ;",";");
    data.replace(";;",";");
//    data.replace("const ;","");
    QStringList list = data.split(";");    
    QString iterator;
    int iCount =0;
   // int size = list.size();    
    while(iCount < list.size())        //Learning:never increment icount in the loop where we are removing the list content
    {
        iterator = list.at(iCount);
        iterator = iterator.left(iterator.indexOf("("));

        if( iterator.contains(" ") == false )
        {
            if(iterator.contains(className) == false )
             {
              list.removeAt(iCount);
             }
            else
                iCount++;
        }
         else
             iCount++;
    }    

    return list;
}









/******** Returns the QstringList of Valid Slot Methods Data ***********/
#if 0
QStringList HeaderParser::getSlotMethodsRawData()
{
    QString iterator;
    int var = 0;
    QString slotMethodsBlock;
    QStringList allSlotMethodsList;
    bool inSlot = false;

    while (var < fileInfoLineData.length())
    {
        iterator = fileInfoLineData.at(var);
        iterator = iterator.simplified();

        if(((iterator.contains("slots") && (iterator.length() <= 7)) ||
                (iterator.contains("Q_SLOTS") && (iterator.length() <= 9))) &&
                     (iterator.contains(":") )) //pure Slot methods
        {
            inSlot = true;
            fileInfoLineData.removeAt(var);
        } else
        {
            if(inSlot == true)
            {
               // publicMethodsBlock.append(iterator);commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                if(iterator.contains(":") || iterator.contains("};"))
                {
                    inSlot = false;
                   // fileInfoLineData.removeAt(var);// commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    break;
                }
                else
                {
                    slotMethodsBlock.append(iterator);//commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    fileInfoLineData.removeAt(var);
                }
            }else
                var ++;
        }
    }

    slotMethodsBlock = slotMethodsBlock.simplified();

    removeDefinitions(slotMethodsBlock);
    allSlotMethodsList = removeProperties(QString(),slotMethodsBlock);
    return allSlotMethodsList;
}



/******** Returns the QstringList of Valid Signal Data ***********/

QStringList HeaderParser::getSignalMethodsRawData()
{
    QString iterator;
    int var = 0;
    QString signalMethodsBlock;
    QStringList allSignalMethodsList;
    bool inSignal = false;

    while (var < fileInfoLineData.length())
    {
        iterator = fileInfoLineData.at(var);
        iterator = iterator.simplified();

        if(((iterator.contains("signals") && (iterator.length() <= 9)) ||
                (iterator.contains("Q_SIGNALS") && (iterator.length() <= 11))) &&
                     (iterator.contains(":") )) //pure Slot methods
        {
            inSignal = true;
            fileInfoLineData.removeAt(var);
        } else
        {
            if(inSignal == true)
            {
               // publicMethodsBlock.append(iterator);commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                if(iterator.contains(":") || iterator.contains("};"))
                {
                    inSignal = false;
                   // fileInfoLineData.removeAt(var);// commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    break;
                }
                else
                {
                    signalMethodsBlock.append(iterator);//commented by avinash from preventing the removal of macros from fileinfodata like protected: ,signal: etc ;
                    fileInfoLineData.removeAt(var);
                }
            }else
                var ++;
        }
    }

    signalMethodsBlock = signalMethodsBlock.simplified();

    removeDefinitions(signalMethodsBlock);

    allSignalMethodsList = removeProperties(QString(),signalMethodsBlock);
    return allSignalMethodsList;
}
#endif
/****Returns QMAP list of Slot Methods *************/
#if 0

QMap<QString, QStringList> HeaderParser::getSlotMethods()
{
    QStringList slotMethodsDataList;
    QMap<QString , QStringList> slotMethods;
    QStringList slotMethodsList;

    slotMethodsDataList =  this->getSlotMethodsRawData();

    QString string;

   for (int iCount = 0; iCount<slotMethodsDataList.size() ; iCount++)
   {

      string = slotMethodsDataList.at(iCount);

       if(string.contains("(") && string.contains(")") /*&& string.contains(";")*/) {

       QStringList argList;
       QString temp = string;
       int endPos = temp.indexOf('(');
       temp =  temp.left(endPos);     //chops all data present after the pUblic method Name.
       temp.remove("&");
       int  startPos= temp.lastIndexOf(' ');
       temp = temp.remove(0,startPos); //removes return type of publicmethods
       temp = temp.simplified();

       QString arg = string;
       arg.remove("const");

       startPos = arg.indexOf('(');
       arg = arg.mid(startPos + 1);

       startPos = arg.indexOf(')');
       arg = arg.left(startPos);
       arg = arg.simplified();

       QStringList list = arg.split(",",QString::SkipEmptyParts);
       QString str;

       foreach(str, list) {
           str = str.simplified();
           if(str.contains("*")) {
               int pos = str.indexOf("*");
               str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);

           } else if (str.contains("&")) {
               int pos = str.indexOf("&");
                str = str.left(pos+1);
               //str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);
           }
           else{
               int pos = str.indexOf(" ");
             //  str.truncate(pos);
               str = str.left(pos);
               str = str.simplified();
               argList.append(str);
           }

       }

       if(slotMethods.contains(temp)) {
           for(int i = 2; i <=10 ; i++) {//for overloaded functions
               temp = temp.remove(QRegExp("[0123456789]"));
               temp = temp.append(QString::number(i));
               if(!slotMethods.contains(temp)) {
                   break;
               }
           }
       }

       slotMethods.insertMulti(temp,argList);
       slotMethodsList.append(temp);
     }
 }
   return slotMethods;
}

#endif
/****Returns QMAP list of signals Methods *************/
#if 0
QMap<QString, QStringList> HeaderParser::getSignalMethods()
{
    QStringList signalMethodsDataList;
    QMap<QString , QStringList> signalMethods;
    QStringList signalMethodsList;

    signalMethodsDataList =  this->getSignalMethodsRawData();

    QString string;
      //Extracting the Arguments

   for (int iCount = 0; iCount<signalMethodsDataList.size() ; iCount++)
   {

      string = signalMethodsDataList.at(iCount);

       if(string.contains("(") && string.contains(")") /*&& string.contains(";")*/) {

       QStringList argList;
       QString temp = string;
       int endPos = temp.indexOf('(');
       temp =  temp.left(endPos);     //chops all data present after the pUblic method Name.
       //temp.remove("&");
       int  startPos= temp.lastIndexOf(' ');
       temp = temp.remove(0,startPos); //removes return type of publicmethods
       temp = temp.simplified();

       QString arg = string;
       arg.remove("const");

       startPos = arg.indexOf('(');
       arg = arg.mid(startPos + 1);

       startPos = arg.indexOf(')');
       arg = arg.left(startPos);
       arg = arg.simplified();

       QStringList list = arg.split(",",QString::SkipEmptyParts);
       QString str;

       foreach(str, list) {
           str = str.simplified();
           if(str.contains("*")) {
               int pos = str.indexOf("*");
               str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);

           } else if (str.contains("&")) {
               int pos = str.indexOf("&");
                str = str.left(pos+1);
               //str = str.left(pos+1);
               str = str.simplified();
               argList.append(str);
           }
           else{
               int pos = str.indexOf(" ");
             //  str.truncate(pos);
               str = str.left(pos);
               str = str.simplified();
               argList.append(str);
           }

       }
       if(signalMethods.contains(temp)) {
           for(int i = 2; i <=10 ; i++) {//for overloaded functions
               temp = temp.remove(QRegExp("[0123456789]"));
               temp = temp.append(QString::number(i));
               if(!signalMethods.contains(temp)) {
                   break;
               }
           }
       }

       signalMethods.insertMulti(temp,argList);
       signalMethodsList.append(temp);

     }
 }

   return signalMethods;
}

#if 0 //in getting classblocks
                    if(iterator.contains("};"))
                    {
                       // qDebug()<<__FILE__<<__LINE__;
                        classBlock.append(iterator);
                        inClass = false;
                        var++;
                        break;
                    }
                    else
                    {
                        var++;
                        classBlock.append(iterator);
                    }
                }else
                    var ++;
#endif

#endif
