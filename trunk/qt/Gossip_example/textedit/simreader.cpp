#include "simreader.h"
#include <qfiledialog.h>
#include <qtextedit.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpushbutton.h>

SimReader::SimReader(QWidget *parent, const char *name)
         :QVBox(parent, name) {

    output = new QTextEdit(this);
    openButton = new QPushButton("Open", this);

    connect(openButton, SIGNAL(clicked()),
            this, SLOT(readTextFile()));
    resize(320, 240);
}

void SimReader::readTextFile() {
    QString s = QFileDialog::getOpenFileName(
                "/home", "Text files (*.txt);;Any files(*.*)",
                    0, "open file dialog Choose a file" );
    QFile file(s); // Read the text from a file

    if (file.open(IO_ReadOnly)) {
        setCaption(s);
        QTextStream stream(&file);
        output->setText(stream.read());
    }
}
