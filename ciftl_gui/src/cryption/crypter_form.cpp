#include <map>

#include <QMessageBox>
#include <QClipboard>
#include <QProcess>

#include <fmt/core.h>

#include <ciftl_core/crypter/crypter.h>

#include "mainwindow.h"
#include "cryption/crypter_form.h"
#include "etc/line_importer.h"
#include "ui_crypter_form.h"

using namespace ciftl;

CrypterForm::CrypterForm(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::CrypterForm),
                                            m_parent_widget(dynamic_cast<MainWindow *>(parent)),
                                            m_line_importer(new LineImporter(parent))
{
    ui->setupUi(this);
    connect(ui->pushButtonEncrypt, &QPushButton::clicked,
            this, &CrypterForm::encrypt);
    connect(ui->pushButtonDecrypt, &QPushButton::clicked,
            this, &CrypterForm::decrypt);
    connect(ui->pushButtonShowPassword, &QPushButton::clicked,
            this, &CrypterForm::show_password);
    connect(ui->pushButtonAdd, &QPushButton::clicked,
            this, &CrypterForm::add_text);
    connect(ui->pushButtonClear, &QPushButton::clicked,
            this, &CrypterForm::clear_table);
    connect(m_line_importer, SIGNAL(table_update(std::vector<CrypterTableData>)),
            this, SLOT(update_table(std::vector<CrypterTableData>)));
    connect(ui->pushButtonCopy, &QPushButton::clicked,
            this, &CrypterForm::copy_result);

    ui->comboBoxCipherType->addItem("ChaCha20");
    ui->comboBoxCipherType->addItem("AES");
    ui->comboBoxCipherType->addItem("SM4");
    // 表格
    CrypterTableDataModel *model = new CrypterTableDataModel({}, this);
    ui->tableView->setModel(model);
    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setStretchLastSection(true);
}

CrypterForm::~CrypterForm()
{
    delete ui;
}

void CrypterForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    resize_table();
}

void CrypterForm::refresh_table()
{
    CrypterTableDataModel *crypter_table_data =
        dynamic_cast<CrypterTableDataModel *>(ui->tableView->model());
    if (!crypter_table_data)
    {
        exit(-1);
    }
    // 保留长度
    std::vector<size_t> section_widths;
    for (int i = 0; i < crypter_table_data->columnCount(); i++)
    {
        section_widths.push_back(ui->tableView->columnWidth(i));
    }
    ui->tableView->setModel(nullptr);
    ui->tableView->setModel(crypter_table_data);
    for (int i = 0; i < crypter_table_data->columnCount(); i++)
    {
        ui->tableView->setColumnWidth(i, section_widths[i]);
    }
}

void CrypterForm::resize_table()
{
    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setMaximumSectionSize(ui->tableView->width() * 0.6);
}

void CrypterForm::update_table(std::vector<CrypterTableData> data)
{
    CrypterTableDataModel *crypter_table_data = dynamic_cast<CrypterTableDataModel *>(ui->tableView->model());
    if (!crypter_table_data)
    {
        exit(-1);
    }
    auto dt = crypter_table_data->get_raw_data();
    *dt = data;
    refresh_table();
}

void CrypterForm::show_password()
{
    if (ui->lineEditPassword->echoMode() == QLineEdit::EchoMode::Password)
    {
        ui->pushButtonShowPassword->setText("隐藏密码");
        ui->lineEditPassword->setEchoMode(QLineEdit::EchoMode::Normal);
    }
    else
    {
        ui->pushButtonShowPassword->setText("显示密码");
        ui->lineEditPassword->setEchoMode(QLineEdit::EchoMode::Password);
    }
}

void CrypterForm::clear_table()
{
    update_table({});
}

void CrypterForm::add_text()
{
    m_line_importer->show();
}

void CrypterForm::copy_result()
{
    // 获取剪切板对象
    QClipboard *clipboard = QApplication::clipboard();

    CrypterTableDataModel *crypter_table_data = dynamic_cast<CrypterTableDataModel *>(ui->tableView->model());
    if (!crypter_table_data)
    {
        exit(-1);
    }
    std::string res;
    for (auto &item : *crypter_table_data->get_raw_data())
    {
        res += item.res_text + "\n";
    }
    // 设置剪切板内容为文本
    clipboard->setText(res.c_str());
}

inline std::shared_ptr<StringCrypterInterface> string_crypter_selection(const std::string &algo_name)
{
    const static std::unordered_map<std::string, ciftl::CipherAlgorithm> str2algo = {
        {"ChaCha20", CipherAlgorithm::ChaCha20},
        {"AES", CipherAlgorithm::AES},
        {"SM4", CipherAlgorithm::SM4}
    };

    auto iter = str2algo.find(algo_name);
    if (iter == str2algo.end())
    {
        return nullptr;
    }
    switch (iter->second)
    {
    case CipherAlgorithm::ChaCha20:
        return default_chacha20_string_crypter();
    case CipherAlgorithm::AES:
        return default_aes_string_crypter();
    case CipherAlgorithm::SM4:
        return default_sm4_string_crypter();
    default:
        return nullptr;
    }
}

void CrypterForm::encrypt()
{
    QString password = ui->lineEditPassword->text().trimmed();
    if (password.isEmpty())
    {
        QMessageBox::critical(this, "错误", "密码不能为空");
        return;
    }
    CrypterTableDataModel *res_data = dynamic_cast<CrypterTableDataModel *>(ui->tableView->model());
    if (!res_data)
    {
        exit(-1);
    }
    if (res_data->get_raw_data()->empty())
    {
        QMessageBox::critical(this, "错误", "待加密内容不能为空");
        return;
    }
    std::shared_ptr<StringCrypterInterface> string_crypter = string_crypter_selection(ui->comboBoxCipherType->currentText().toStdString());
    for (auto &item : *res_data->get_raw_data())
    {
        std::string plain_text = item.src_text;
        std::string password = ui->lineEditPassword->text().toStdString();
        auto res = string_crypter->encrypt(plain_text, password);
        if (res.is_ok())
        {
            item.res_text = res.get_ok_val().value();
            item.res_mes = "成功";
        }
        else
        {
            item.res_text.clear();
            item.res_mes =
                fmt::format("{}: {}", res.get_err_val().value().get_error_code(), res.get_err_val().value().get_error_message());
        }
    }
    refresh_table();
}

void CrypterForm::decrypt()
{
    QString password = ui->lineEditPassword->text().trimmed();
    if (password.isEmpty())
    {
        QMessageBox::critical(this, "错误", "密码不能为空");
        return;
    }
    CrypterTableDataModel *res_data = dynamic_cast<CrypterTableDataModel *>(ui->tableView->model());
    if (!res_data)
    {
        exit(-1);
    }
    if (res_data->get_raw_data()->empty())
    {
        QMessageBox::critical(this, "错误", "待解密内容不能为空");
        return;
    }
    std::shared_ptr<StringCrypterInterface> string_crypter = string_crypter_selection(ui->comboBoxCipherType->currentText().toStdString());
    for (auto &item : *res_data->get_raw_data())
    {
        std::string cipher_text = item.src_text;
        std::string password = ui->lineEditPassword->text().toStdString();
        auto res = string_crypter->decrypt(cipher_text, password);
        if (res.is_ok())
        {
            item.res_text = res.get_ok_val().value();
            item.res_mes = "成功";
        }
        else
        {
            item.res_text.clear();
            item.res_mes =
                fmt::format("{}: {}", res.get_err_val().value().get_error_code(), res.get_err_val().value().get_error_message());
        }
    }
    refresh_table();
}