#ifndef CPPJIEBA_DICT_TRIE_HPP
#define CPPJIEBA_DICT_TRIE_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <cmath>
#include <limits>
#include "support/StringUtil.hpp"
#include "support/Logging.hpp"
#include "Unicode.hpp"
#include "Trie.hpp"

namespace cppjieba {

using namespace limonp;

const double MIN_DOUBLE = -3.14e+100;
const double MAX_DOUBLE = 3.14e+100;
const size_t DICT_COLUMN_NUM = 3;
const char* const UNKNOWN_TAG = "";

class DictTrie {
 public:
  enum UserWordWeightOption {
    WordWeightMin,
    WordWeightMedian,
    WordWeightMax,
  }; // enum UserWordWeightOption

  DictTrie(const string& dict_path, const string& user_dict_paths = "", UserWordWeightOption user_word_weight_opt = WordWeightMedian) {
    Init(dict_path, user_dict_paths, user_word_weight_opt);
  }

  ~DictTrie() {
    delete trie_;
  }

  const DictUnit* Find(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end) const {
    return trie_->Find(begin, end);
  }

  void Find(RuneStrArray::const_iterator begin, 
        RuneStrArray::const_iterator end, 
        vector<struct Dag>&res,
        size_t max_word_len = MAX_WORD_LENGTH) const {
    trie_->Find(begin, end, res, max_word_len);
  }

  bool Find(const string& word)
  {
    const DictUnit *tmp = NULL;
    RuneStrArray runes;
    if (!DecodeRunesInString(word, runes))
    {
      XLOG(ERROR) << "Decode failed.";
    }
    tmp = Find(runes.begin(), runes.end());
    if (tmp == NULL)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  double GetMinWeight() const {
    return min_weight_;
  }

 private:
  void Init(const string& dict_path, const string& user_dict_paths, UserWordWeightOption user_word_weight_opt) {
    LoadDict(dict_path);
    freq_sum_ = CalcFreqSum(static_node_infos_);
    CalculateWeight(static_node_infos_, freq_sum_);
    // SetStaticWordWeights(user_word_weight_opt);

    Shrink(static_node_infos_);
    CreateTrie(static_node_infos_);
  }
  
  void CreateTrie(const vector<DictUnit>& dictUnits) {
    assert(dictUnits.size());
    vector<Unicode> words;
    vector<const DictUnit*> valuePointers;
    for (size_t i = 0 ; i < dictUnits.size(); i ++) {
      words.push_back(dictUnits[i].word);
      valuePointers.push_back(&dictUnits[i]);
    }

    trie_ = new Trie(words, valuePointers);
  }
  
  bool MakeNodeInfo(DictUnit& node_info,
        const string& word, 
        double weight, 
        const string& tag) {
    if (!DecodeRunesInString(word, node_info.word)) {
      XLOG(ERROR) << "Decode " << word << " failed.";
      return false;
    }
    node_info.weight = weight;
    node_info.tag = tag;
    return true;
  }

  void LoadDict(const string& filePath) {
    ifstream ifs(filePath.c_str());
    XCHECK(ifs.is_open()) << "open " << filePath << " failed.";
    string line;
    vector<string> buf;

    DictUnit node_info;
    for (size_t lineno = 0; getline(ifs, line); lineno++) {
      Split(line, buf, " ");
      XCHECK(buf.size() == DICT_COLUMN_NUM) << "split result illegal, line:" << line;
      MakeNodeInfo(node_info, 
            buf[0], 
            atof(buf[1].c_str()), 
            buf[2]);
      static_node_infos_.push_back(node_info);
    }
  }

  static bool WeightCompare(const DictUnit& lhs, const DictUnit& rhs) {
    return lhs.weight < rhs.weight;
  }

  double CalcFreqSum(const vector<DictUnit>& node_infos) const {
    double sum = 0.0;
    for (size_t i = 0; i < node_infos.size(); i++) {
      sum += node_infos[i].weight;
    }
    return sum;
  }

  void CalculateWeight(vector<DictUnit>& node_infos, double sum) const {
    assert(sum > 0.0);
    for (size_t i = 0; i < node_infos.size(); i++) {
      DictUnit& node_info = node_infos[i];
      assert(node_info.weight > 0.0);
      node_info.weight = log(double(node_info.weight)/sum);
    }
  }

  void Shrink(vector<DictUnit>& units) const {
    vector<DictUnit>(units.begin(), units.end()).swap(units);
  }

  vector<DictUnit> static_node_infos_;
  deque<DictUnit> active_node_infos_; // must not be vector
  Trie * trie_;

  double freq_sum_;
  double min_weight_;
  double max_weight_;
  double median_weight_;
  double user_word_default_weight_;
};
}

#endif
