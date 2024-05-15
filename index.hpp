// 此文件是正排索引 倒排索引数据结构
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <thread>
#include <mutex>
#include "comm/util.hpp"
#include "comm/cppjieba/Jieba.hpp"
using namespace std;

struct Docinfo_ix
{
    string title;    // 文档的标题
    string content;  // 文档的内容
    string url;      // 文档的url
    uint64_t doc_id; // 文档的id
};

// 倒排节点数据结构
struct Elem
{
    uint64_t doc_id;
    int weight;
    string word;
};

// 单例模式
class Index
{
private:
    Index()
    {
    }
    Index(const Index &x) = delete;
    Index &operator=(const Index &x) = delete;

public:
    static Index *_indexptr;
    static mutex *_mu;
    static Index *Getindex_ptr()
    {
        if (_indexptr == nullptr)
        {
            _mu->lock();
            if (_indexptr == nullptr)
            {
                _indexptr = new Index();
            }
            _mu->unlock();
        }

        return _indexptr;
    }

    ~Index()
    {
    }

    // 获取正排索引id的结构
    Docinfo_ix Getfront_word(uint64_t doc_id)
    {
        if (doc_id >= _front_index.size())
        {
            return Docinfo_ix();
        }
        return _front_index[doc_id];
    }

    // 根据关键字获取倒排拉链
    vector<Elem> *Getback_list(const string &s)
    {
        if (_back_index.find(s) != _back_index.end())
        {
            return &_back_index[s];
        }
        return nullptr;
    }



    // 根据去标签的文档来构建正排 倒排索引
    bool BuilIndex(const string &src)
    {
        std::ifstream in(src, ios::binary | ios::in);
        if (!in.is_open())
        {
            cout << "打开去标签文件失败" << endl;
            return false;
        }
        int count = 0;
        int sumcount = 0;
        string linefile;


        while (getline(in, linefile))
        {
            // 按照行读取doc结构
            Docinfo_ix *ptr = Builfrontindex(linefile);
            if (ptr == nullptr)
            {
                continue;
            }
            shared_ptr<Docinfo_ix> info(ptr);

            // 进行倒排拉链插入 info分词 统计词频 再插入结构
            Builbackindex(*info);

            // 可视化索引建立过程
            if (++count >= 100)
            {
                sumcount += count;
                cout << "Buil Index:" << sumcount << endl;
                count = 0;
            }
        }


        cout << "文档建立完毕!" << endl;
        return true;
    }

private:
    // 局内构造接口
    Docinfo_ix *Builfrontindex(const string &line)
    {
        // 字符串按分隔符切分
        vector<string> vc;
        string sep = "\3";
        String_util::Cutstring(line, vc, sep);

        if (vc.size() != 3)
        {
            return nullptr;
        }

        // 将数据推入到正排中
        Docinfo_ix *ret = new Docinfo_ix;
        ret->title = vc[0];
        ret->content = vc[1];
        ret->url = vc[2];
        ret->doc_id = _front_index.size();
        _front_index.push_back(*ret);
        return ret;
    }

    void Builbackindex(const Docinfo_ix &doc)
    {
        // 标题分词
        vector<string> vc_tile;
        Jieba_util::Cutstring(doc.title, vc_tile);

        // 正文分词
        vector<string> vc_conten;
        Jieba_util::Cutstring(doc.content, vc_conten);

        // 词频统计 用pair<int,int> first代表标题 second代表正文
        unordered_map<string, pair<int, int>> word_cnt;

        for (auto x : vc_tile)
        {
            // 小写转换
            boost::to_lower(x);
            word_cnt[x].first++;
        }

        for (auto x : vc_conten)
        {
            boost::to_lower(x);
            word_cnt[x].second++;
        }

#define X 100
#define Y 1
        // 遍历词频结构来创建倒排数据
        for (auto &x : word_cnt)
        {
            Elem val;
            val.doc_id = doc.doc_id;
            val.weight = x.second.first * X + x.second.second * Y; // 对文档的标题和正文进行权重赋值
            val.word = x.first;

            // 插入倒排拉链
            _back_index[x.first].push_back(val);
        }
    }


private:
    // 正排索引
    vector<Docinfo_ix> _front_index;
    // 倒排索引
    unordered_map<string, vector<Elem>> _back_index;
};
Index *Index::_indexptr = nullptr;
mutex *Index::_mu = new mutex;