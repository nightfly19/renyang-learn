#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qpainter.h>
#include <qtextstream.h>

#include "tictactoe.h"

TicTacToe::TicTacToe(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    setCaption(tr("Tic-Tac-Toe"));
    clearBoard();
    if (qApp->isSessionRestored())
        restoreState();
}

QSize TicTacToe::sizeHint() const
{
    return QSize(200, 200);
}

void TicTacToe::clearBoard()
{
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            board[row][col] = Empty;
        }
    }
    turnNumber = 0;
}

void TicTacToe::restoreState()
{
    QFile file(sessionFileName());
    if (file.open(IO_ReadOnly)) {
        QTextStream in(&file);
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                in >> board[row][col];
                if (board[row][col] != Empty)
                    ++turnNumber;
            }
        }
    }
    repaint();
}

QString TicTacToe::sessionFileName() const
{
    return QDir::homeDirPath() + "/.tictactoe_"
           + qApp->sessionId() + "_" + qApp->sessionKey();
}

QString TicTacToe::saveState() const
{
    QFile file(sessionFileName());
    if (file.open(IO_WriteOnly)) {
        QTextStream out(&file);
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                out << board[row][col];
            }
        }
    }
    return file.name();
}

bool TicTacToe::gameInProgress() const
{
    return turnNumber > 0 && turnNumber < 9;
}

void TicTacToe::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(QPen(red, 1));
    painter.drawLine(cellWidth(), 0, cellWidth(), height());
    painter.drawLine(2 * cellWidth(), 0, 2 * cellWidth(), height());
    painter.drawLine(0, cellHeight(), width(), cellHeight());
    painter.drawLine(0, 2 * cellHeight(), width(), 2 * cellHeight());

    painter.setPen(QPen(blue, 2));
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            QRect r = cellRect(row, col);
            if (board[row][col] == Cross) {
                painter.drawLine(r.topLeft(), r.bottomRight());
                painter.drawLine(r.topRight(), r.bottomLeft());
            } else if (board[row][col] == Nought) {
                painter.drawEllipse(r);
            }
        }
    }

    painter.setPen(QPen(yellow, 3));
    for (int row = 0; row < 3; ++row) {
        if (board[row][0] != Empty
                && board[row][1] == board[row][0]
                && board[row][2] == board[row][0]) {
            int y = cellRect(row, 0).center().y();
            painter.drawLine(0, y, width(), y);
            turnNumber = 9;
        }
    }
    for (int col = 0; col < 3; ++col) {
        if (board[0][col] != Empty
                && board[1][col] == board[0][col]
                && board[2][col] == board[0][col]) {
            int x = cellRect(0, col).center().x();
            painter.drawLine(x, 0, x, height());
            turnNumber = 9;
        }
    }
    if (board[0][0] != Empty && board[1][1] == board[0][0]
            && board[2][2] == board[0][0]) {
        painter.drawLine(0, 0, width(), height());
        turnNumber = 9;
    }
    if (board[0][2] != Empty && board[1][1] == board[0][2]
            && board[2][0] == board[0][2]) {
        painter.drawLine(0, height(), width(), 0);
        turnNumber = 9;
    }
}

void TicTacToe::mousePressEvent(QMouseEvent *event)
{
    if (turnNumber == 9) {
        clearBoard();
        repaint();
    } else {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                QRect r = cellRect(row, col);
                if (r.contains(event->pos())) {
                    if (board[row][col] == Empty) {
                        if (turnNumber % 2 == 0)
                            board[row][col] = Cross;
                        else
                            board[row][col] = Nought;
                        ++turnNumber;
                        repaint();
                    }
                    break;
                }
            }
        }
    }
}

QRect TicTacToe::cellRect(int row, int col) const
{
    const int HMargin = width() / 30;
    const int VMargin = height() / 30;
    return QRect(col * cellWidth() + HMargin,
                 row * cellHeight() + VMargin,
                 cellWidth() - 2 * HMargin,
                 cellHeight() - 2 * VMargin);
}
