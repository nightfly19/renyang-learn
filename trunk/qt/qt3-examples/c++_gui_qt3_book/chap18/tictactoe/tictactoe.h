#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <qwidget.h>

class TicTacToe : public QWidget
{
    Q_OBJECT
public:
    TicTacToe(QWidget *parent = 0, const char *name = 0);

    QSize sizeHint() const;
    bool gameInProgress() const;
    QString saveState() const;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    enum { Empty = '-', Cross = 'X', Nought = 'O' };

    void clearBoard();
    void restoreState();
    QString sessionFileName() const;
    QRect cellRect(int row, int col) const;
    int cellWidth() const { return width() / 3; }
    int cellHeight() const { return height() / 3; }

    char board[3][3];
    int turnNumber;
};

#endif
