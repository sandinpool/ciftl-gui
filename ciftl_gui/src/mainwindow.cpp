#include <QVBoxLayout>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cryption/hash_form.h"
#include "cryption/crypter_form.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_hash_form(new HashForm(this)), m_crypter_form(new CrypterForm(this))
{
    ui->setupUi(this);

    static const char *help_info =
        "ciftl是一个密码学工具箱\n"
        "包括了\"密码工具\"、\"哈希工具\"等实用工具\n"
        "密码工具：用于对字符串进行加密，目前支持ChaCha20，AES和SM4三种加密算法\n"
        "哈希工具：用于对文件进行哈希计算，支持MD5, Sha1, Sha256, Sha512四种哈希算法\n"
        "作者：三点一洲\n"
        "Copyright (c) 三点一洲（三点一洲工作室） All copyright reserved";
    ui->stackedWidget->addWidget(m_crypter_form);
    ui->stackedWidget->addWidget(m_hash_form);

    connect(ui->actionCrypter, &QAction::triggered, this, [this]()
            {
        ui->stackedWidget->setCurrentWidget(m_crypter_form);
        ui->statusbar->showMessage("密码工具"); });
    connect(ui->actionHash, &QAction::triggered, this, [this]()
            {
        ui->stackedWidget->setCurrentWidget(m_hash_form);
        ui->statusbar->showMessage("哈希工具"); });
    connect(ui->actionAbout, &QAction::triggered,
            this, [this]
            { QMessageBox::information(this, "关于", help_info); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show()
{
    QMainWindow::show();
    // 默认显示
    ui->stackedWidget->setCurrentWidget(m_crypter_form);
    ui->statusbar->showMessage("ciftl");
}

void MainWindow::set_status_message(const QString &mes)
{
    ui->statusbar->showMessage(mes);
}
