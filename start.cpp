#ifndef CPPJIEBA_MIXSEGMENT_H
#define CPPJIEBA_MIXSEGMENT_H

// #include <pybind11/pybind11.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <ctime>
#include "MPSegment.hpp"
#include "DictTrie.hpp"
#include "Unicode.hpp"
#include "PreFilter.hpp"

using namespace std;
// namespace py = pybind11;
using namespace cppjieba;

// const char* const DICT_PATH = "./dict/jieba.dict.utf8";
const char* const DICT_PATH = "/home/work/zhouchengjie/product_c/segment/dict/jieba.dict.utf8";


class Segment {
    public:
        Segment(const string& dict_path=DICT_PATH): 
        dict_trie_(dict_path),
        mpSeg_(&dict_trie_){
        }
        ~Segment() {
        }

    void Cut(const string& sentence, vector<string>& words) const {
        vector<Word> tmp;
        PreCut(sentence, tmp);
        GetStringsFromWords(tmp, words);
    }

    void PreCut(const string& sentence, vector<Word>& words) const {
        PreFilter pre_filter(symbols_, sentence);
        PreFilter::Range range;
        vector<WordRange> wrs;
        wrs.reserve(sentence.size() / 2);
        while (pre_filter.HasNext()) {
            range = pre_filter.Next();
            MpsCut(range.begin, range.end, wrs);
        }
        words.clear();
        words.reserve(wrs.size());
        GetWordsFromWordRanges(sentence, wrs, words);
    }

    void MpsCut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res) const {
        mpSeg_.Cut(begin, end, res);
        return;
    }

    private:
        DictTrie dict_trie_;
        MPSegment mpSeg_;
    
    protected:
        unordered_set<Rune> symbols_;

}; // class Segment

Segment Init() {
  return Segment();
}


/*
void MpsCut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res, const string& dict_path) 
{
    mpSeg_.Cut(begin, end, res);
    return;
}

void PreCut(const string& sentence, vector<Word>& words, const string& dict_path)
{
    unordered_set<Rune> symbols_;
    PreFilter pre_filter(symbols_, sentence);
    PreFilter::Range range;
    vector<WordRange> wrs;
    wrs.reserve(sentence.size() / 2);
    while (pre_filter.HasNext()) {
        range = pre_filter.Next();
        MpsCut(range.begin, range.end, wrs, dict_path);
    }
    words.clear();
    words.reserve(wrs.size());
    GetWordsFromWordRanges(sentence, wrs, words);
}

void Cut(const string& sentence, vector<string>& words, const string& dict_path=DICT_PATH)
{
    vector<Word> tmp;
    PreCut(sentence, tmp, dict_path);
    GetStringsFromWords(tmp, words);
}

const MPSegment & Init(const MPSegment &mpSeg_, const string& dict_path=DICT_PATH) 
{   
    DictTrie dict_trie_(dict_path);
    MPSegment mpSeg_(&dict_trie_);
	return mpSeg_;
}

    // private:
    //     DictTrie dict_trie_;
    //     MPSegment mpSeg_;

// }; // class Segment
*/

int main(int argc, char** argv) {
    Segment segment(DICT_PATH);

    string s;
    vector<string> words;

    string filename;
    filename = "/home/work/zhouchengjie/tmp/lac/python/test_query_20w.txt";
	ifstream infile(filename.c_str());
	string temp;

    // s = "他来到了网易杭研大厦";
    // // Cut(s, words);
    // segment.Cut(s, words);
    // cout<<words<<endl;
    // return 0;

    clock_t start,end;
    start = clock();
	while(getline(infile,temp))
	{
		// cout<<temp<<endl;
        // cout<<"Hello world!"<<endl;
        segment.Cut(temp, words);
        // Cut(temp, words);

        // cout << words << endl;
	}
    end=clock();

    double endtime=(double)(end-start)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime<<endl;	

	cout<<"Hello world!"<<endl;
    return 0;

    // s = "他来到了网易杭研大厦";
    // cout << "[demo] Cut Without HMM " << endl;
    // segment.Cut(s, words);
    // cout << words << endl;
    // cout << limonp::Join(words.begin(), words.end(), "/") << endl;
}

#endif
