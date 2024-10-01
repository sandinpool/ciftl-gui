#include <fstream>
#include <thread>
#include <filesystem>

#include <fmt/core.h>

#include <QFileDialog>
#include <QClipboard>

#include <ciftl_core/hash/hash.h>
#include <ciftl_core/etc/etc.h>

#include "cryption/hash_form.h"
#include "ui_hash_form.h"

HashForm::HashForm(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::HashForm)
{
    ui->setupUi(this);
    connect(this, SIGNAL(operation_start()), this, SLOT(start_operation()));
    connect(this, SIGNAL(operation_end()), this, SLOT(end_operation()));
    connect(this, SIGNAL(file_progress_update(size_t)), this, SLOT(update_file_progress(size_t)));
    connect(this, SIGNAL(total_progress_update(size_t)), this, SLOT(update_total_progress(size_t)));
    connect(this, SIGNAL(main_text_update(QString)), this, SLOT(update_main_text(QString)));
    connect(ui->pushButtonOpen, SIGNAL(clicked()), this, SLOT(choose_files()));
    connect(ui->pushButtonCopy, SIGNAL(clicked()), this, SLOT(copy_result()));
    connect(ui->pushButtonSaveAs, SIGNAL(clicked()), this, SLOT(save_as()));
    connect(ui->pushButtonClear, SIGNAL(clicked()), this, SLOT(clear_text()));
}

HashForm::~HashForm()
{
    delete ui;
}

std::vector<std::pair<std::string, std::shared_ptr<ciftl::Hasher>>> HashForm::generate_hasher_vec()
{
    std::vector<std::pair<std::string, std::shared_ptr<ciftl::Hasher>>> hasher_vec;
    if (ui->checkBoxMD5->isChecked())
    {
        hasher_vec.push_back({"MD5", std::make_shared<ciftl::MD5Hasher>()});
    }
    if (ui->checkBoxSha1->isChecked())
    {
        hasher_vec.push_back({"Sha1", std::make_shared<ciftl::Sha1Hasher>()});
    }
    if (ui->checkBoxSha256->isChecked())
    {
        hasher_vec.push_back({"Sha256", std::make_shared<ciftl::Sha256Hasher>()});
    }
    if (ui->checkBoxSha512->isChecked())
    {
        hasher_vec.push_back({"Sha512", std::make_shared<ciftl::Sha512Hasher>()});
    }
    return hasher_vec;
}

void HashForm::start_operation()
{
    this->setEnabled(false);
}

void HashForm::end_operation()
{
    this->setEnabled(true);
    if (m_thread)
    {
        m_thread->join();
        m_thread = nullptr;
    }
}

void HashForm::update_file_progress(size_t val)
{
    ui->progressBarFile->setValue(val);
}

void HashForm::update_total_progress(size_t val)
{
    ui->progressBarTotal->setValue(val);
}

void HashForm::update_main_text(QString val)
{
    ui->plainTextEdit->appendHtml(val);
}

void HashForm::clear_text()
{
    ui->plainTextEdit->clear();
    ui->progressBarFile->setValue(0);
    ui->progressBarTotal->setValue(0);
}

void HashForm::copy_result()
{
    // 获取剪切板对象
    QClipboard *clipboard = QApplication::clipboard();
    // 设置剪切板内容为文本
    clipboard->setText(ui->plainTextEdit->toPlainText());
}

inline std::string to_local_path(const QString &str)
{
    auto local_8bit = str.toLocal8Bit();
    auto local_8bit_str = std::string(local_8bit.constData(), local_8bit.size());
    return local_8bit_str;
};

void HashForm::save_as()
{
    QString q_file_path = QFileDialog::getSaveFileName(nullptr, "保存文件", QDir::homePath(), "文本文件 (*.txt)");
    auto file_path = to_local_path(q_file_path);
    std::ofstream ofs(file_path, std::ios::out);
    ofs << ui->plainTextEdit->toPlainText().toStdString();
}

void HashForm::choose_files()
{
    // 选取文件
    QStringList q_file_paths = QFileDialog::getOpenFileNames(nullptr, "选择文件", QDir::homePath(), "所有文件 (*.*)");
    do_hash(q_file_paths);
}

void HashForm::do_hash(QStringList file_paths)
{
    std::function<void()> func = [this, file_paths]()
    {
        emit operation_start();
        emit total_progress_update(0L);
        // 遍历文件执行
        for (size_t i = 0; i < (size_t)file_paths.size(); i++)
        {
            auto q_file_path = file_paths[i];
            auto file_path = to_local_path(q_file_path);
            // 文件名不存在则跳过
            if (std::filesystem::exists(file_path))
            {
                // 标题和文件大小
                QString title = QString("<b>文件名: ") + q_file_path + "</b>";
                size_t file_size = std::filesystem::file_size(file_path);
                QString file_size_banner =
                    QString::fromStdString(fmt::format(std::locale("zh_CN.UTF-8"), "<b>文件大小:</b> {:L} Bytes", file_size));
                emit main_text_update(title);
                emit main_text_update(file_size_banner);
                // 哈希算法
                auto hasher_vec = generate_hasher_vec();
                // 打开文件
                std::ifstream ifs(file_path, std::ios::in | std::ios::binary);
                if (ifs)
                {
                    size_t sum = 0;
                    // 以32MB为一个块进行计算来减少IO
                    constexpr size_t block_size = 1024 * 1024 * 32;
                    ciftl::ByteVector tmp(block_size);
                    emit file_progress_update(0L);
                    // 边读取边计算hash
                    for (;;)
                    {
                        ifs.read((char *)tmp.data(), block_size);
                        if (auto cnt = ifs.gcount(); cnt)
                        {
                            for (auto iter : hasher_vec)
                            {
                                iter.second->update(tmp.data(), cnt);
                            }
                            sum += cnt;
                            emit file_progress_update((size_t)(100.0 * sum / file_size));
                        }
                        else
                        {
                            break;
                        }
                    }
                    // 以十六进制格式输出
                    ciftl::HexEncoding hex;
                    for (auto iter : hasher_vec)
                    {
                        auto res = iter.second->finalize();
                        QString message = QString::fromStdString(fmt::format("<b>{}:</b> {}", iter.first, hex.encode(res)));
                        emit main_text_update(message);
                    }
                }
                else
                {
                    // 处理文件打开失败的情况
                    emit main_text_update("无法打开文件：" + q_file_path);
                }
            }
            emit main_text_update("");
            emit total_progress_update((size_t)(100.0 * (i + 1) / file_paths.size()));
        }
        emit operation_end();
    };
    if (!m_thread)
    {
        m_thread = std::make_unique<std::thread>(func);
    }
}
