// 本程序是搜索功能的实现
#pragma once
#include "index.hpp"
#include "util.hpp"
#include <set>
#include <algorithm>
#include <json/json.h>
#include <unordered_map>
#include <string>
#include <vector>

class Searcher
{
private:
    struct Elem_words
    {

        uint64_t doc_id;
        int weight;
        vector<string> word_list;

        Elem_words()
        {
            doc_id = 0;
            weight = 0;
        }
    };

public:
    Searcher()
    {
    }
    ~Searcher()
    {
    }

    // 初始化索引结构
    void InitSearcher(const string &src)
    {
        _index = Index::Getindex_ptr();
        _index->BuilIndex(src);
    }

    // 搜索主模块 将搜索内容进行分词 对应index 权重排序 再来构建jsoncpp网页
    void Search(const string &input, string &json_string)
    {
        cout << "用户搜索了" << input << endl;

        // 分词
        vector<string> vc_input;
        Jieba_util::Cutstring(input, vc_input);

        // 获取倒排拉链 分词获取
        unordered_map<uint64_t, Elem_words> hash_table;
        vector<Elem_words> vc_Elem_words;
        for (string word : vc_input)
        {
            boost::to_lower(word);

            vector<Elem> *El_list = _index->Getback_list(word);
            if (El_list == nullptr)
            {
                continue;
            }

            // 进行文档去重 相同id叠加权重处理
            for (auto &x : *El_list)
            {
                auto &item = hash_table[x.doc_id];

                item.doc_id = x.doc_id;
                item.weight += x.weight;
                item.word_list.push_back(move(word));
            }
        }

        for (auto &x : hash_table)
        {
            vc_Elem_words.push_back(x.second);
        }

        // 按权重降序排序
        sort(vc_Elem_words.begin(), vc_Elem_words.end(), [](const Elem_words &a, const Elem_words &b) -> bool
             { return a.weight > b.weight; });

        // 创建jsoncpp
        Json::Value root;
        for (auto &x : vc_Elem_words)
        {
            Docinfo_ix doc = move(_index->Getfront_word(x.doc_id));

            Json::Value item;
            item["title"] = move(doc.title);
            item["desc"] = move(Getdesc(doc.content, x.word_list[0]));
            item["url"] = move(doc.url);

            root.append(move(item));
        }

        // json写入到string里
        Json::StyledWriter wr;
        json_string = wr.write(root);
    }

private:
    // 局内  正文转摘要
    string Getdesc(const string &conten, const string &word)
    {
        // 从首次出现单词位置的前后开始制作摘要
        int post_pre = 100;
        int post_next = 100;

        // 查找转小写
        auto it = search(conten.begin(), conten.end(), word.begin(), word.end(), [](char a, char b) -> bool
                         { return tolower(a) == tolower(b); });

        if (it == conten.end())
        {
            return "None1";
        }
        int post = distance(conten.begin(), it);

        int start = 0;
        int end = conten.size() - 1;

        if (post - post_pre > start)
        {
            start = post - post_pre;
        }
        if (post + post_next <= end)
        {
            end = post + post_next;
        }
        if (start >= end)
        {
            return "None2";
        }
        return move(conten.substr(start, end - start) + "...");
    }

private:
    // 索引创建 ptr
    Index *_index;
};