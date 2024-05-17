#include "comm/httplib.h"
#include "search.hpp"

const string input = "./data/raw_html/raw.txt";
const string root_path = "./wwwroot";

const string pre_path="/BS";

int port=8081;


Searcher*sc_ptr=nullptr;

void Http_get_index(const httplib::Request &req, httplib::Response &rsp)
{
    string html;
    if(File_util::Read(root_path,html))
    {
        rsp.set_content(html, "text/html;charset=utf-8");
    }
    else 
    {
        rsp.status = 404;
        rsp.set_content("404 Not Found", "text/plain");
    }
}

void Http_search_post(const httplib::Request &req, httplib::Response &rsp)
{
    if(!req.has_param("word"))
    {
        rsp.set_content("必须要有搜索关键字!", "text/plain; charset=utf-8");
        return;
    }
    std::string word = req.get_param_value("word");
    
    std::string json_string;
    sc_ptr->Search(word,json_string);
    rsp.set_content(json_string, "application/json");
}

int main(int argnum, char *argvs[])
{
    if(argnum==2)
    {
        port=atoi(argvs[1]);
    }

    Searcher search;
    search.InitSearcher(input);

    sc_ptr=&search;
    httplib::Server svr;
    
    svr.Get(pre_path,Http_get_index);
    svr.Get(pre_path+"/",Http_get_index);

    svr.Get(pre_path+"/s",Http_search_post);

    //svr.set_base_dir(root_path.c_str());
    svr.listen("127.0.0.1",port);
    return 0;
}