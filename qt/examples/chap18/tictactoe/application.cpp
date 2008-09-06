#include <qmessagebox.h>
#include <qsessionmanager.h>

#include "application.h"
#include "tictactoe.h"

Application::Application(int &argc, char *argv[])
    : QApplication(argc, argv)
{
    ticTacToe = 0;
}

void Application::setTicTacToe(TicTacToe *tic)
{
    ticTacToe = tic;
    setMainWidget(tic);
}

void Application::saveState(QSessionManager &sessionManager)
{
    QString fileName = ticTacToe->saveState();

    QStringList discardCommand;
    discardCommand << "rm" << fileName;
    sessionManager.setDiscardCommand(discardCommand);
}

void Application::commitData(QSessionManager &sessionManager)
{
    if (ticTacToe->gameInProgress()
            && sessionManager.allowsInteraction()) {
        int ret = QMessageBox::warning(ticTacToe, tr("Tic-Tac-Toe"),
                     tr("The game hasn't finished.\n"
                        "Do you really want to quit?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            sessionManager.release();
        else
            sessionManager.cancel();
    }
}
