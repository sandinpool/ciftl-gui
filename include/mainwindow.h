#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MPasswordToolForm;
class HashForm;
class CrypterForm;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

protected:
    CrypterForm *m_crypter_form;
    HashForm *m_hash_form;

public:
    void show();

public:
    void set_status_message(const QString &mes);
};
#endif // MAINWINDOW_H
