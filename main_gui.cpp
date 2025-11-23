#include <QApplication>
#include "chess_gui.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    ChessGUI window;
    window.show();
    
    return app.exec();
}
