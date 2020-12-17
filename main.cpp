#include "start.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <ctime>

// const char* const DICT_PATH = "/home/work/zhouchengjie/product_c/segment/dict/jieba.dict.utf8";


int main(int argc, char** argv) {

    Segment segment = Init();

    clock_t start,end;
    string s;
    vector<string> words;
    start = clock();

    s = "他来到了网易杭研大厦";
    // Cut(s, words);
    segment.Cut(s, words);
    cout<<words<<endl;
    return 0;
}

	// while(getline(infile,temp))
	// {
	// 	// cout<<temp<<endl;
    //     // cout<<"Hello world!"<<endl;
    //     // segment.Cut(temp, words);
    //     Cut(temp, words);

    //     // cout << words << endl;
	// }
    // end=clock();