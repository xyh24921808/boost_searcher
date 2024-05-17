#include "comm/httplib.h"
#include "search.hpp"

const string input = "./data/raw_html/raw.txt";
const string root_path = "./wwwroot";

const string pre_path="/BS";


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

int main()
{
    Searcher search;
    search.InitSearcher(input);

    httplib::Server svr;
    
    svr.Get(pre_path,Http_get_index);
    svr.Get(pre_path+"/",Http_get_index);

    svr.Get(pre_path+"/s", [&search](const httplib::Request &req, httplib::Response &rsp)
            {
            if(!req.has_param("word"))
            {
                rsp.set_content("必须要有搜索关键字!", "text/plain; charset=utf-8");
                return;
            }
            std::string word = req.get_param_value("word");
            

            std::string json_string;
            search.Search(word,json_string);
            rsp.set_content(json_string, "application/json"); });

    //svr.set_base_dir(root_path.c_str());
    svr.listen("127.0.0.1", 8080);
    return 0;
}