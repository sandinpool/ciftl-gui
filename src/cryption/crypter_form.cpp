#include <map>

#include <QMessageBox>
#include <QClipboard>
#include <QProcess>

#include <fmt/core.h>

#include <ciftl/crypter/crypter.h>

#include "mainwindow.h"

#include "cryption/crypter_form.h"
#include "etc/line_importer.h"

#include "ui_crypter_form.h"

using namespace ciftl;

const std::vector<std::pair<std::string, CipherAlgorithm>> CrypterForm::__supported_cipher_algorithm__= {
    {"ChaCha20", CipherAlgorithm::ChaCha20},
    {"AES-128位-OFB", CipherAlgorithm::AES128OFB},
    {"AES-192位-OFB", CipherAlgorithm::AES192OFB},
    {"AES-256位-OFB", CipherAlgorithm::AES256OFB},
    {"SM4-OFB", CipherAlgorithm::SM4OFB}
};
const std::unordered_map<std::string, CipherAlgorithm> CrypterForm::__str_to_cipher_algorithm__(__supported_cipher_algorithm__.begin(), __supported_cipher_algorithm__.end());


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
    // 加载下拉框
    for(const auto& iter : __supported_cipher_algorithm__)
    {
        ui->comboBoxCipherType->addItem(QString::fromStdString(iter.first));
    }
    // 初始化表格
    CrypterTableDataModel *model = new CrypterTableDataModel({}, this);
    ui->tableView->setModel(model);
    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setStretchLastSection(true);
}

CrypterForm::~CrypterForm()

{
    delete ui;
}

void CrypterForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    restrict_table();
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

void CrypterForm::restrict_table()
{
    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setMinimumSectionSize(ui->tableView->width() * 0.25);
    header->setMaximumSectionSize(ui->tableView->width() * 0.75);
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

inline std::shared_ptr<IStringCrypter> string_crypter_selection(const std::string &algo_name)
{
    auto iter = CrypterForm::__str_to_cipher_algorithm__.find(algo_name);
    if (iter == CrypterForm::__str_to_cipher_algorithm__.end())
    {
        return nullptr;
    }
    switch (iter->second)
    {
    case CipherAlgorithm::ChaCha20:
        return std::make_shared<StringCrypter<ChaCha20CipherAlgorithm>>();
    case CipherAlgorithm::AES128OFB:
        return std::make_shared<StringCrypter<AES128OFBCipherAlgorithm>>();
    case CipherAlgorithm::AES192OFB:
        return std::make_shared<StringCrypter<AES192OFBCipherAlgorithm>>();
    case CipherAlgorithm::AES256OFB:
        return std::make_shared<StringCrypter<AES256OFBCipherAlgorithm>>();
    case CipherAlgorithm::SM4OFB:
        return std::make_shared<StringCrypter<SM4OFBCipherAlgorithm>>();
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
    std::shared_ptr<IStringCrypter> string_crypter = string_crypter_selection(ui->comboBoxCipherType->currentText().toStdString());
    for (auto &item : *res_data->get_raw_data())
    {
        std::string plain_text = item.src_text;
        std::string password = ui->lineEditPassword->text().toStdString();
        auto res = string_crypter->encrypt(plain_text, password);
        if (res.is_ok())
        {
            item.res_text = res.ok().value();
            item.res_mes = "成功";
        }
        else
        {
            item.res_text.clear();
            item.res_mes =
                fmt::format("{}: {}", res.error().value().error_code(), res.error().value().error_message());
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
    std::shared_ptr<IStringCrypter> string_crypter = string_crypter_selection(ui->comboBoxCipherType->currentText().toStdString());
    for (auto &item : *res_data->get_raw_data())
    {
        std::string cipher_text = item.src_text;
        std::string password = ui->lineEditPassword->text().toStdString();
        auto res = string_crypter->decrypt(cipher_text, password);
        if (res.is_ok())
        {
            item.res_text = res.ok().value();
            item.res_mes = "成功";
        }
        else
        {
            item.res_text.clear();
            item.res_mes =
                fmt::format("{}: {}", res.error().value().error_code(), res.error().value().error_message());
        }
    }
    refresh_table();
}
