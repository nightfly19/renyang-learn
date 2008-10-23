
#ifndef DDLABEL_H
#define DDLABEL_H

#include <qlabel.h>
#include <qdragobject.h>


class DDLabel : public QLabel{
public:
	enum labelType {Sor, Des};

	DDLabel(QWidget *parent, int type);

protected:
	void dragEnterEvent(QDragEnterEvent *e);

	void dragLeaveEvent(QDragLeaveEvent *);

	void dropEvent(QDropEvent *e);

	void mousePressEvent(QMouseEvent *e);

private:
	int labeltype;
};

#endif // DDLABEL_H

