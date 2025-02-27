#include "../include/settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), playerGroup(new QButtonGroup(this)), protocolGroup(new QButtonGroup(this)) {
    setupUi();
}

void SettingsDialog::setupUi() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* playerLabel = new QLabel("Select Player:", this);
    layout->addWidget(playerLabel);

    QRadioButton* radioPlayer1 = new QRadioButton("Player 1", this);
    QRadioButton* radioPlayer2 = new QRadioButton("Player 2", this);
    playerGroup->addButton(radioPlayer1, 1);
    playerGroup->addButton(radioPlayer2, 2);
    layout->addWidget(radioPlayer1);
    layout->addWidget(radioPlayer2);
    radioPlayer1->setChecked(true);

    QLabel* protocolLabel = new QLabel("Select Protocol:", this);
    layout->addWidget(protocolLabel);

    QRadioButton* radioUDP = new QRadioButton("UDP", this);
    QRadioButton* radioTCP = new QRadioButton("TCP", this);
    protocolGroup->addButton(radioUDP, 1);
    protocolGroup->addButton(radioTCP, 2);
    layout->addWidget(radioUDP);
    layout->addWidget(radioTCP);
    radioUDP->setChecked(true);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
}

int SettingsDialog::getSelectedPlayer() const {
    return playerGroup->checkedId();
}

QString SettingsDialog::getSelectedProtocol() const {
    return protocolGroup->checkedId() == 1 ? "UDP" : "TCP";
}
