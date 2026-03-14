#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include "src/database/dbmanager.h"

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

signals:
    void registerFinished();

private slots:
    void on_submitRegisterButton_clicked();

private:
    Ui::RegisterWindow *ui;
    DBManager* m_dbManager;
};

#endif // REGISTERWINDOW_H
