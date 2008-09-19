/****************************************************************************
** $Id: qt/mainwindow.h   3.3.7   edited Aug 31 2005 $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qmainwindow.h>

class HeaderToolTip;
class TableToolTip;
class QAssistantClient;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

public slots:
    void assistantSlot();

private:
    HeaderToolTip *horizontalTip;
    HeaderToolTip *verticalTip;    
    TableToolTip *cellTip;
    QAssistantClient *assistant;
};
