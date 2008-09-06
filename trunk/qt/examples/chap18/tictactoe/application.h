#ifndef APPLICATION_H
#define APPLICATION_H

#include <qapplication.h>

class TicTacToe;

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char *argv[]);

    void setTicTacToe(TicTacToe *tic);
    void commitData(QSessionManager &sessionManager);
    void saveState(QSessionManager &sessionManager);

private:
    TicTacToe *ticTacToe;
};

#endif
