#ifndef IMAGE_LABEL_H
#define IMAGE_LABEL_H

#include <QLabel>

class QDragEnterEvent;
class QDropEvent;

class ImageLabel : public QLabel {
   Q_OBJECT
   
public:
    ImageLabel(QWidget *parent = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void readImage(const QString &fileName);
};

#endif
