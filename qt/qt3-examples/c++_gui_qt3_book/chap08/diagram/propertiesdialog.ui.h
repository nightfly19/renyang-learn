#include <qcolordialog.h>

#include "diagramview.h"

void PropertiesDialog::chooseLineColor()
{
    QColor oldColor = lineColorSample->paletteBackgroundColor();
    QColor newColor = QColorDialog::getColor(oldColor, this);
    if (newColor.isValid())
        lineColorSample->setPaletteBackgroundColor(newColor);
}

void PropertiesDialog::chooseFillColor()
{
    QColor oldColor = fillColorSample->paletteBackgroundColor();
    QColor newColor = QColorDialog::getColor(oldColor, this);
    if (newColor.isValid())
        fillColorSample->setPaletteBackgroundColor(newColor);
}

void PropertiesDialog::exec(QCanvasItem *item)
{
    if (item->rtti() == DiagramBox::RTTI) {
        DiagramBox *box = (DiagramBox *)item;
        QRect rect = box->rect();
        setCaption(tr("Properties for Box"));

        xSpinBox->setValue(rect.x());
        ySpinBox->setValue(rect.y());
        widthSpinBox->setValue(rect.width());
        heightSpinBox->setValue(rect.height());
        lineGeometryGroupBox->hide();
        textLineEdit->setText(box->text());
        lineColorSample->setPaletteBackgroundColor(
                box->pen().color());
        fillColorSample->setPaletteBackgroundColor(
                box->brush().color());
    } else if (item->rtti() == DiagramLine::RTTI) {
        DiagramLine *line = (DiagramLine *)item;
        QPoint start = line->startPoint() + line->offset();
        QPoint end = line->endPoint() + line->offset();
        setCaption(tr("Properties for Line"));

        rectGeometryGroupBox->hide();
        x1SpinBox->setValue(start.x());
        y1SpinBox->setValue(start.y());
        x2SpinBox->setValue(end.x());
        y2SpinBox->setValue(end.y());
        textLabel->hide();
        textLineEdit->hide();
        lineColorSample->setPaletteBackgroundColor(
                line->pen().color());
        fillColorLabel->hide();
        fillColorSample->hide();
        fillColorButton->hide();
    }

    if (QDialog::exec()) {
        if (item->rtti() == DiagramBox::RTTI) {
            DiagramBox *box = (DiagramBox *)item;
            box->move(xSpinBox->value(), ySpinBox->value());
            box->setSize(widthSpinBox->value(),
                         heightSpinBox->value());
            box->setText(textLineEdit->text());
            box->setPen(lineColorSample->paletteBackgroundColor());
            box->setBrush(fillColorSample->paletteBackgroundColor());
        } else if (item->rtti() == DiagramLine::RTTI) {
            DiagramLine *line = (DiagramLine *)item;
            line->move(0, 0);
            line->setPoints(x1SpinBox->value(), y1SpinBox->value(),
                            x2SpinBox->value(), y2SpinBox->value());
            line->setPen(lineColorSample->paletteBackgroundColor());
            line->setBrush(fillColorSample->paletteBackgroundColor());
        }
        item->canvas()->update();
    }
}
