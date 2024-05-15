#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <boost/algorithm/string.hpp>
#include "cppjieba/Jieba.hpp"
using namespace std;

class File_util
{
public:
    static bool Read(const string &file_path, string &out)
    {
        std::ifstream in(file_path, ios::in);
        if (!in.is_open())
        {
            return false;
        }

        string line;
        while (getline(in, line))
        {
            out += line;
        }
        in.close();
        return true;
    }
};

class String_util
{
public:
    static void Cutstring(const string &src, vector<string> &vc, const string &sep)
    {
        boost::split(vc, src, boost::is_any_of(sep));
    }
};

const char *const DICT_PATH = "./comm/cppjieba/dict/jieba.dict.utf8";
const char *const HMM_PATH = "./comm/cppjieba/dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "./comm/cppjieba/dict/user.dict.utf8";
const char *const IDF_PATH = "./comm/cppjieba/dict/idf.utf8";
const char *const STOP_WORD_PATH = "./comm/cppjieba/dict/stop_words.utf8";

class Jieba_util
{
private:
    static cppjieba::Jieba jieba;
    static unordered_map<string, int> stop_words;

public:
    Jieba_util()
    {
        ifstream in(STOP_WORD_PATH);
        if (!in.is_open())
        {
            cout << "open SOTP_WORD error" << endl;
        }

        string line;
        while (getline(in, line))
        {
            stop_words[line]++;
        }

        in.close();
    }

    static void Remostop_wrod(const vector<string> &src, vector<string> &res)
    {
        for (int i = 0; i < src.size(); i++)
        {
            if (stop_words[src[i]] == 0)
            {
                res.push_back(src[i]);
            }
        }
    }
    static void Cutstring(const string &src, vector<string> &vc)
    {
        if (stop_words.size() == 0)
        {
            Jieba_util();
        }

        vector<string> res;
        jieba.Cut(src, vc);

        // 是否去掉暂停词
        Remostop_wrod(vc,res);
        vc = move(res);
    }
};

cppjieba::Jieba Jieba_util::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
unordered_map<string, int> Jieba_util::stop_words;
