#include <QApplication>

#include "../include/game.h"
#include "../include/settingsdialog.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    SettingsDialog settingsDialog;
    if (settingsDialog.exec() == QDialog::Accepted) {
        int selectedPlayer = settingsDialog.getSelectedPlayer();
        QString selectedProtocol = settingsDialog.getSelectedProtocol();
        Game game(selectedPlayer, selectedProtocol);
        game.start();

        return app.exec();
    }

    return 0;
}
