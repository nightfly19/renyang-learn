
#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qintdict.h>
#include <qcanvas.h>
#include <qstring.h>
#include <qfile.h>

#include "canvas.h"
#include "cmainwindow.h"
#include "FigureEditor.h"

class Coordinator;
class Road;
class Car;
class QScrollBar;
class QPushButton;
class Road;
class NetCoordinator;
class WifiCar;
class InfoViewerImp;

//class Main : public QMainWindow {
class Main : public cMainWindow {
    Q_OBJECT

public:
	enum UI_STATE{
		UI_USER_SEL,
		UI_CENTER_SEL,
	};
    Main(QCanvas&, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~Main();


signals:
	void CarSel(Car*);

public slots:
    void helpAbout();

protected:
    void keyPressEvent ( QKeyEvent * e );

private slots:
    void slotNewView();
    void showMsg(QString&);
    void slotClear();
    void init();

    void slotAddSprite();
    void slotAddCar();
    void slotRandAddWifiCars();
    void slotSetTimeRatio();
    void slotScrollPosition(int pos);
    void slotActPosViewer( bool );
    void slotActSimSet();
    void slotRandAddDSRCCar();
    void slotSimDone();
    virtual void slotLogFileSetting();

    // mouse events
    void slotLeftSelect(QCanvasItem* );
    void slotRightSelect(QCanvasItem* );
    void slotMouseRelease();
    void slotUpdateStatusBar();

    virtual void slotFileOpen();
    virtual void slotStop();
    virtual void slotPlay();
    virtual void slotPause();
    void slotSetFixedCenter(bool b);

    virtual void slotActCenter();
    virtual void slotActUsrSel();
/*    void addCircle();
    void addHexagon();
    void addPolygon();
    void addSpline();
    void addText();
    void addLine();
    void addRectangle();
    void addMesh();
    void addLogo();
    void addButterfly();*/

    void slotEnlarge();
    void slotShrink();
    void slotZoomIn();
    void slotZoomOut();
    void rotateClockwise();
    void rotateCounterClockwise();
    void mirror();
    void moveL();
    void moveR();
    void moveU();
    void moveD();

    void print();

    void toggleDoubleBuffer(bool);

private:
    void tryOpenLogFile();
    void tryWriteLogFile();
    void tryCloseLogFile();

protected slots:
    virtual void languageChange();


private:
	void randCarPos(Car*);
	void ParseConfigureFile(QFile&);
	void resetScrollBar();
private:
    QCanvas& canvas;
    FigureEditor *editor;
    Coordinator *coordinator;
    Road *road;
    void roadResize();

    QPopupMenu* options;
    QPrinter* printer;
    Car* _sel_car;

    QScrollBar* sc_position;
    QPushButton* pb_center;
    InfoViewerImp *pos_viewer ;
    UI_STATE ui_state;

    QString _fp_log_file;
    QFile _log_file;
//    int dbf_id;

	// networking
private:
    NetCoordinator *net_coor;
};

#endif
