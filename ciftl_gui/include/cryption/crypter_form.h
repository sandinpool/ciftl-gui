#ifndef CRYPTER_FORM_H
#define CRYPTER_FORM_H

#include <QWidget>
#include <QAbstractTableModel>

#include "etc/line_importer.h"
#include "etc/type.h"

class MainWindow;

namespace Ui
{
    class CrypterForm;
}

class LineImorter;

class CrypterTableDataModel : public QAbstractTableModel
{
    Q_OBJECT
#define RESULT_DATA_FIELD_COUNT 3
public:
    CrypterTableDataModel(const std::vector<CrypterTableData> &res_data, QObject *parent = nullptr)
        : QAbstractTableModel(parent), m_res_data(std::make_shared<std::vector<CrypterTableData>>(res_data)) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return (int)m_res_data->size();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return RESULT_DATA_FIELD_COUNT;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= m_res_data->size() || index.column() >= RESULT_DATA_FIELD_COUNT)
            return QVariant();

        const CrypterTableData &res_data = m_res_data->at(index.row());

        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            switch (index.column())
            {
            case 0:
                return QString::fromStdString(res_data.src_text);
            case 1:
                return QString::fromStdString(res_data.res_text);
            case 2:
                return QString::fromStdString(res_data.res_mes);
            }
        }

        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    {
        if (role == Qt::EditRole && index.isValid() && index.row() < m_res_data->size() && index.column() < RESULT_DATA_FIELD_COUNT)
        {
            CrypterTableData &res_data = m_res_data->at(index.row());

            switch (index.column())
            {
            case 0:
                res_data.src_text = value.toString().toStdString();
                break;
            case 1:
                res_data.res_text = value.toString().toStdString();
                break;
            case 2:
                res_data.res_mes = value.toString().toStdString();
                break;
            }

            emit dataChanged(index, index);
            return true;
        }
        return false;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole)
        {
            if (orientation == Qt::Horizontal)
            {
                switch (section)
                {
                case 0:
                    return QString("原始数据");
                case 1:
                    return QString("加/解密结果");
                case 2:
                    return QString("消息");
                }
            }
            else if (orientation == Qt::Vertical)
            {
                return QString::number(section + 1);
            }
        }
        return QVariant();
    }

    std::shared_ptr<std::vector<CrypterTableData>> get_raw_data()
    {
        return m_res_data;
    }

private:
    std::shared_ptr<std::vector<CrypterTableData>> m_res_data;
};

class CrypterForm : public QWidget
{
    Q_OBJECT
protected:
    void resizeEvent(QResizeEvent *event) override;

public:
    explicit CrypterForm(QWidget *parent = nullptr);
    ~CrypterForm();

public:
    enum class CryptionMode
    {
        ENCRYPTION,
        DECRYPTION
    };

private:
    void refresh_table();

    void resize_table();

private slots:
    void update_table(std::vector<CrypterTableData> data);
    void show_password();
    void clear_table();
    void add_text();
    void copy_result();
    void encrypt();
    void decrypt();

private:
    Ui::CrypterForm *ui;

private:
    MainWindow *m_parent_widget;
    LineImporter *m_line_importer;
};

#endif // CRYPTER_FORM_H
