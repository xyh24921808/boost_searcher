// 此程序是去掉库文档的标签工作
//    C++/20
#include "comm/util.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <fstream>

using namespace std;
using namespace boost::filesystem;

vector<string>src_url_path={"https://www.boost.org/doc/libs/1_83_0/doc/html",
                            "https://www.boost.org/doc/libs/1_83_0/libs"};


// html原始文件的目录
vector<string>src_path_vc={"date/input/doc","date/input/libs"};

// 去标签之后html存放的目录
const string output = "date/raw_html/raw.txt";

struct Docinfo
{
    string title;   // 文档的标题
    string content; // 文档的内容
    string url;     // 文档的url
};

// 获取所有html文件名
bool Enum_File(const string &src, vector<string> &vc);
// 解析文件 去标签
bool Parser_html(vector<string> &file_list, vector<Docinfo> &vc,size_t url_post);
// 写入到output里
bool Save_html(vector<Docinfo> &vc, const string &output);

int main(void)
{
    for(int i=0;i<src_path_vc.size();i++)
    {
        vector<string> file_list;
        // 获取html所有文件名
        if (!Enum_File(src_path_vc[i], file_list))
        {
            cout << "读取原始html失败" << endl;
            return -1;
        }

        // 对所有html文件进行解析
        vector<Docinfo> result;
        if (!Parser_html(file_list, result,i))
        {
            cout << "解析html失败" << endl;
            return -2;
        }

        // 把解析完成的各个内容写入到raw.txt里  按照\3作为分隔符
        if (!Save_html(result, output))
        {
            cout << "写入raw.txt失败" << endl;
            return -3;
        }
    }

    return 0;
}

// 获取所有html文件名
bool Enum_File(const string &src, vector<string> &vc)
{
    path root_path(src);

    // 判断文件夹是否存在
    if (!exists(root_path))
    {
        return false;
    }

    // 定义一个空迭代器 判断结束
    recursive_directory_iterator end;
    for (recursive_directory_iterator x(root_path); x != end; x++)
    {
        // 判断是否是文件
        if (!is_regular_file(*x))
        {
            continue;
        }
        // 判断是否为html
        if (x->path().extension() != ".html")
        {
            continue;
        }

        vc.push_back(x->path().string());
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// 获取title标题 局内函数
static bool parser_head(const string &res, string &docs)
{
    size_t begin = res.find("<title>");
    size_t end = res.find("</title>");
    if (begin == string::npos || end == string::npos)
    {
        return false;
    }

    begin += string("<title>").size();
    if (begin > end)
    {
        return false;
    }
    docs = move(res.substr(begin, end - begin));
    return true;
}

// 获取正文 局内函数
static bool parser_conten(const string &res, string &docs)
{
    bool T = false;
    string con;
    for (char c : res)
    {
        
        if (c == '>')
        {
            T = true;
            continue;
        }
        else if (c == '<')
        {
            T = false;
        }
        if (T)
        {
            if (c == '\n')
                c = ' ';
            con += c;
        }
    }
    docs = move(con);
    return true;
}


// 获取url 局内函数
static bool parser_url(const string &file_name,size_t url_head_post,string &docs)
{
    const string url_head=src_url_path[url_head_post];
    string url_tail = file_name.substr(src_path_vc[url_head_post].size());
    docs = move(url_head + url_tail);
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////

// 解析文件
bool Parser_html(vector<string> &file_list, vector<Docinfo> &vc,size_t url_post)
{
    for (const string files : file_list)
    {
        // 读取文件到res里
        string res;
        if (!File_util::Read(files, res))
        {
            continue;
        }

        Docinfo dc;
        // 提取html的标题
        if (!parser_head(res, dc.title))
        {
            continue;
        }

        // 提取html的正文内容
        if (!parser_conten(res, dc.content))
        {
            continue;
        }

        // 提取html的目标网址url
        if (!parser_url(files,url_post,dc.url))
        {
            continue;
        }
        vc.push_back(dc);
    }
    return true;
}

// 写入到output里
bool Save_html(vector<Docinfo> &vc, const string &output)
{
// 分隔符
#define SEP '\3'

    std::ofstream out(output, ios::app | ios::binary);
    if (!out.is_open())
    {
        cout << "open output error" << endl;
        return false;
    }

    // 遍历Doc数组 写入output
    for (auto &item : vc)
    {
        // 采用分隔符进行分隔 \n结尾
        string outstring;
        outstring += item.title;
        outstring += SEP;
        outstring += item.content;
        outstring += SEP;
        outstring += item.url;
        outstring += '\n';

        out.write(outstring.c_str(),outstring.size());
    }

    out.close();
    return true;
}