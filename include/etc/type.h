#ifndef TYPE_H
#define TYPE_H
#include <string>

// 加密器的表格数据
struct CrypterTableData
{
    std::string src_text;
    std::string res_text;
    std::string res_mes;

    CrypterTableData(std::string const &src_text,
                     std::string const &res_text = "",
                     std::string const &res_mes = "")
        : src_text(src_text),
          res_text(res_text),
          res_mes(res_mes)
    {
    }
};

#endif // TYPE_H
