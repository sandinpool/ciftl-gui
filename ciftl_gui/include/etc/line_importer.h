#pragma once
#include <QWidget>
#include <QDialog>

#include "etc/type.h"

namespace Ui
{
    class LineImporter;
}

class LineImporter : public QDialog
{
    Q_OBJECT

public:
    explicit LineImporter(QWidget *parent = nullptr);
    ~LineImporter();

private slots:
    void confirm();
    void cancel();

private:
    Ui::LineImporter *ui;

signals:
    void table_update(std::vector<CrypterTableData> data);
};
