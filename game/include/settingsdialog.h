#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QDialogButtonBox>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    int getSelectedPlayer() const;
    QString getSelectedProtocol() const;

private:
    void setupUi();

    QButtonGroup *playerGroup;
    QButtonGroup *protocolGroup;
};

#endif // SETTINGSDIALOG_H
