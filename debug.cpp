#include "search.hpp"
#include <string>

const string PATH = "./date/raw_html/raw.txt";

int main(void)
{
    Searcher s;

    s.InitSearcher(PATH);
    while (1)
    {
        string in;
        cout << "输入搜索的内容>>";
        getline(cin, in);

        string js;
        s.Search(in, js);
        cout << js << endl;
    }
    return 0;
}