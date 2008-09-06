#include "application.h"
#include "tictactoe.h"

int main(int argc, char *argv[])
{
    Application app(argc, argv);
    TicTacToe tic(0, "tic");
    app.setTicTacToe(&tic);
    tic.show();
    return app.exec();
}
