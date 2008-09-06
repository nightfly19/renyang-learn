#include <qpainter.h>
#include <qstyle.h>
#include <qtoolbutton.h>

#include <cmath>
using namespace std;

#include "plotter.h"

Plotter::Plotter(QWidget *parent, const char *name, WFlags flags)
    : QWidget(parent, name, flags | WNoAutoErase)
{
    setBackgroundMode(PaletteDark);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(StrongFocus);
    rubberBandIsShown = false;

    zoomInButton = new QToolButton(this);
    zoomInButton->setIconSet(QPixmap::fromMimeSource("zoomin.png"));
    zoomInButton->adjustSize();
    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));

    zoomOutButton = new QToolButton(this);
    zoomOutButton->setIconSet(
            QPixmap::fromMimeSource("zoomout.png"));
    zoomOutButton->adjustSize();
    connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));

    setPlotSettings(PlotSettings());
}

void Plotter::setPlotSettings(const PlotSettings &settings)
{
    zoomStack.resize(1);
    zoomStack[0] = settings;
    curZoom = 0;
    zoomInButton->hide();
    zoomOutButton->hide();
    refreshPixmap();
}

void Plotter::zoomOut()
{
    if (curZoom > 0) {
        --curZoom;
        zoomOutButton->setEnabled(curZoom > 0);
        zoomInButton->setEnabled(true);
        zoomInButton->show();
        refreshPixmap();
    }
}

void Plotter::zoomIn()
{
    if (curZoom < (int)zoomStack.size() - 1) {
        ++curZoom;
        zoomInButton->setEnabled(
                curZoom < (int)zoomStack.size() - 1);
        zoomOutButton->setEnabled(true);
        zoomOutButton->show();
        refreshPixmap();
    }
}

void Plotter::setCurveData(int id, const CurveData &data)
{
    curveMap[id] = data;
    refreshPixmap();
}

void Plotter::clearCurve(int id)
{
    curveMap.erase(id);
    refreshPixmap();
}

QSize Plotter::minimumSizeHint() const
{
    return QSize(4 * Margin, 4 * Margin);
}

QSize Plotter::sizeHint() const
{
    return QSize(8 * Margin, 6 * Margin);
}

void Plotter::paintEvent(QPaintEvent *event)
{
    QMemArray<QRect> rects = event->region().rects();
    for (int i = 0; i < (int)rects.size(); ++i)
        bitBlt(this, rects[i].topLeft(), &pixmap, rects[i]);

    QPainter painter(this);

    if (rubberBandIsShown) {
        painter.setPen(colorGroup().light());
        painter.drawRect(rubberBandRect.normalize());
    }
    if (hasFocus()) {
        style().drawPrimitive(QStyle::PE_FocusRect, &painter,
                              rect(), colorGroup(),
                              QStyle::Style_FocusAtBorder,
                              colorGroup().dark());
    }
}

void Plotter::resizeEvent(QResizeEvent *)
{
    int x = width() - (zoomInButton->width()
                       + zoomOutButton->width() + 10);
    zoomInButton->move(x, 5);
    zoomOutButton->move(x + zoomInButton->width() + 5, 5);
    refreshPixmap();
}

void Plotter::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == LeftButton) {
        rubberBandIsShown = true;
        rubberBandRect.setTopLeft(event->pos());
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
        setCursor(crossCursor);
    }
}

void Plotter::mouseMoveEvent(QMouseEvent *event)
{
    if (event->state() & LeftButton) {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}

void Plotter::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == LeftButton) {
        rubberBandIsShown = false;
        updateRubberBandRegion();
        unsetCursor();

        QRect rect = rubberBandRect.normalize();
        if (rect.width() < 4 || rect.height() < 4)
            return;
        rect.moveBy(-Margin, -Margin);

        PlotSettings prevSettings = zoomStack[curZoom];
        PlotSettings settings;
        double dx = prevSettings.spanX() / (width() - 2 * Margin);
        double dy = prevSettings.spanY() / (height() - 2 * Margin);
        settings.minX = prevSettings.minX + dx * rect.left();
        settings.maxX = prevSettings.minX + dx * rect.right();
        settings.minY = prevSettings.maxY - dy * rect.bottom();
        settings.maxY = prevSettings.maxY - dy * rect.top();
        settings.adjust();

        zoomStack.resize(curZoom + 1);
        zoomStack.push_back(settings);
        zoomIn();
    }
}

void Plotter::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Key_Plus:
        zoomIn();
        break;
    case Key_Minus:
        zoomOut();
        break;
    case Key_Left:
        zoomStack[curZoom].scroll(-1, 0);
        refreshPixmap();
        break;
    case Key_Right:
        zoomStack[curZoom].scroll(+1, 0);
        refreshPixmap();
        break;
    case Key_Down:
        zoomStack[curZoom].scroll(0, -1);
        refreshPixmap();
        break;
    case Key_Up:
        zoomStack[curZoom].scroll(0, +1);
        refreshPixmap();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void Plotter::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numTicks = numDegrees / 15;

    if (event->orientation() == Horizontal)
        zoomStack[curZoom].scroll(numTicks, 0);
    else
        zoomStack[curZoom].scroll(0, numTicks);
    refreshPixmap();
}

void Plotter::updateRubberBandRegion()
{
    QRect rect = rubberBandRect.normalize();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

void Plotter::refreshPixmap()
{
    pixmap.resize(size());
    pixmap.fill(this, 0, 0);
    QPainter painter(&pixmap, this);
    drawGrid(&painter);
    drawCurves(&painter);
    update();
}

void Plotter::drawGrid(QPainter *painter)
{
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);
    PlotSettings settings = zoomStack[curZoom];
    QPen quiteDark = colorGroup().dark().light();
    QPen light = colorGroup().light();

    for (int i = 0; i <= settings.numXTicks; ++i) {
        int x = rect.left() + (i * (rect.width() - 1)
                                 / settings.numXTicks);
        double label = settings.minX + (i * settings.spanX()
                                          / settings.numXTicks);
        painter->setPen(quiteDark);
        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->setPen(light);
        painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
        painter->drawText(x - 50, rect.bottom() + 5, 100, 15,
                          AlignHCenter | AlignTop,
                          QString::number(label));
    }
    for (int j = 0; j <= settings.numYTicks; ++j) {
        int y = rect.bottom() - (j * (rect.height() - 1)
                                   / settings.numYTicks);
        double label = settings.minY + (j * settings.spanY()
                                          / settings.numYTicks);
        painter->setPen(quiteDark);
        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->setPen(light);
        painter->drawLine(rect.left() - 5, y, rect.left(), y);
        painter->drawText(rect.left() - Margin, y - 10,
                          Margin - 5, 20,
                          AlignRight | AlignVCenter,
                          QString::number(label));
    }
    painter->drawRect(rect);
}

void Plotter::drawCurves(QPainter *painter)
{
    static const QColor colorForIds[6] = {
        red, green, blue, cyan, magenta, yellow
    };
    PlotSettings settings = zoomStack[curZoom];
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);

    painter->setClipRect(rect.x() + 1, rect.y() + 1,
                         rect.width() - 2, rect.height() - 2);

    map<int, CurveData>::const_iterator it = curveMap.begin();
    while (it != curveMap.end()) {
        int id = (*it).first;
        const CurveData &data = (*it).second;
        int numPoints = 0;
        int maxPoints = data.size() / 2;
        QPointArray points(maxPoints);

        for (int i = 0; i < maxPoints; ++i) {
            double dx = data[2 * i] - settings.minX;
            double dy = data[2 * i + 1] - settings.minY;
            double x = rect.left() + (dx * (rect.width() - 1)
                                         / settings.spanX());
            double y = rect.bottom() - (dy * (rect.height() - 1)
                                           / settings.spanY());
            if (fabs(x) < 32768 && fabs(y) < 32768) {
                points[numPoints] = QPoint((int)x, (int)y);
                ++numPoints;
            }
        }
        points.truncate(numPoints);
        painter->setPen(colorForIds[(uint)id % 6]);
        painter->drawPolyline(points);
        ++it;
    }
}

PlotSettings::PlotSettings()
{
    minX = 0.0;
    maxX = 10.0;
    numXTicks = 5;

    minY = 0.0;
    maxY = 10.0;
    numYTicks = 5;
}

void PlotSettings::scroll(int dx, int dy)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;

    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;
}

void PlotSettings::adjust()
{
    adjustAxis(minX, maxX, numXTicks);
    adjustAxis(minY, maxY, numYTicks);
}

void PlotSettings::adjustAxis(double &min, double &max,
                              int &numTicks)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = pow(10, floor(log10(grossStep)));

    if (5 * step < grossStep)
        step *= 5;
    else if (2 * step < grossStep)
        step *= 2;

    numTicks = (int)(ceil(max / step) - floor(min / step));
    min = floor(min / step) * step;
    max = ceil(max / step) * step;
}
