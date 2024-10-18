#ifndef HASH_FORM_H
#define HASH_FORM_H
#include <thread>

#include <QWidget>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QPlainTextEdit>

#include <ciftl/hash/hash.h>
#include <ciftl/etc/etc.h>

namespace Ui
{
    class HashForm;
}

class HashForm : public QWidget
{
    Q_OBJECT

public:
    explicit HashForm(QWidget *parent = nullptr);
    ~HashForm();

private:
    std::vector<std::pair<std::string, std::shared_ptr<ciftl::Hasher>>> generate_hasher_vec();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override
    {
        if (event->mimeData()->hasUrls())
        {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override
    {
        QList<QUrl> urls = event->mimeData()->urls();
        QStringList file_paths;
        for (const QUrl &url : urls)
        {
            file_paths.append(url.toLocalFile());
        }
        if (file_paths.isEmpty())
        {
            return;
        }
        do_hash(file_paths);
    }

signals:
    void operation_start();
    void operation_end();
    void file_progress_update(size_t val);
    void total_progress_update(size_t val);
    void main_text_update(QString val);

private slots:
    void start_operation();
    void end_operation();
    void update_file_progress(size_t val);
    void update_total_progress(size_t val);
    void update_main_text(QString val);
    // 界面槽函数
    void clear_text();
    void copy_result();
    void save_as();
    void choose_files();
    void do_hash(QStringList file_paths);

private:
    Ui::HashForm *ui;
    std::unique_ptr<std::thread> m_thread;
};

#endif // HASH_FORM_H
