#include "etc/line_importer.h"
#include "etc/type.h"

#include "ui_line_importer.h"

LineImporter::LineImporter(QWidget *parent)
    : QDialog(parent), ui(new Ui::LineImporter)
{
    ui->setupUi(this);
    connect(ui->pushButtonCancel, &QPushButton::clicked,
            this, &LineImporter::cancel);
    connect(ui->pushButtonConfirm, &QPushButton::clicked,
            this, &LineImporter::confirm);
}

LineImporter::~LineImporter()
{
    delete ui;
}

void LineImporter::confirm()
{
    QString text = ui->plainTextEdit->toPlainText();
    QStringList lines = text.split("\n");
    for (; lines.size() >= 1 && lines.back().trimmed().isEmpty();)
    {
        lines.removeLast();
    }
    std::vector<CrypterTableData> crypter_table_data;
    for (const QString &line : lines)
    {
        crypter_table_data.emplace_back(line.toStdString(), "", "");
    }
    emit table_update(crypter_table_data);
    this->hide();
}

void LineImporter::cancel()
{
    this->hide();
}
