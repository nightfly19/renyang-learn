
#ifndef __INFO_VIEWER_IMP_H__
#define __INFO_VIEWER_IMP_H__

#include "InfoViewer.h"
#include <qtimer.h>

class WifiCar;
class DSRCCar;
class Car;
class InfoViewerImp : public InfoViewer
{
    Q_OBJECT

public:
    InfoViewerImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~InfoViewerImp();

    void hide();
    void show();

public slots:
	void slotCarSel(Car* c);
	void slotHide();
	void slotUpdate();

private:
	Car* sel;
	QTimer *qtimer;
};

#endif
