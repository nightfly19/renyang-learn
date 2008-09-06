#include <qapplication.h>
#include <qfile.h>

#include "plotter.h"

void readFlightCurves(Plotter *plotter, const QString &fileName)
{
    CurveData data[6];
    double factX = 0.0013;
    double factY[6] = { 0.0008, 0.1, 0.2, 0.2, 0.1, 0.8 };
    double offsetY[6] = { +500, -55, +309, +308, 0, 0 };
    int pos[6] = { 3, 6, 7, 8, 9, 10 };
    QFile file(fileName);
    double offsetX = 0.0;

    if (file.open(IO_ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList coords = QStringList::split(" ", line);
            if (coords.size() >= 6) {
                double x = factX * coords[0].toDouble();
                if (data[0].size() == 0)
                    offsetX = x;
                for (int i = 0; i < 6; ++i) {
                    double y = coords[pos[i]].toDouble();
                    data[i].push_back(x - offsetX);
                    data[i].push_back(factY[i] * (y - offsetY[i]));
                }
            }
        }
    }
    plotter->setCurveData(0, data[0]);
    plotter->setCurveData(1, data[1]);
    plotter->setCurveData(2, data[2]);
    plotter->setCurveData(3, data[3]);
    plotter->setCurveData(4, data[4]);
    plotter->setCurveData(5, data[5]);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Plotter plotter;
    plotter.setCaption(QObject::tr("Plotter"));
    readFlightCurves(&plotter, "in1.txt");
    app.setMainWidget(&plotter);
    plotter.show();
    return app.exec();
}
