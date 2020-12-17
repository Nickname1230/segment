#ifndef CPPJIEBA_MPSEGMENT_H
#define CPPJIEBA_MPSEGMENT_H

#include <algorithm>
#include <set>
#include <cassert>
#include "support/Logging.hpp"
#include "DictTrie.hpp"

namespace cppjieba {

class MPSegment {
 public:
  MPSegment(const DictTrie* dictTrie)
    : dictTrie_(dictTrie), isNeedDestroy_(false) {
    assert(dictTrie_);
  }
  ~MPSegment() {
    if (isNeedDestroy_) {
      delete dictTrie_;
    }
  }

  void Cut(RuneStrArray::const_iterator begin,
           RuneStrArray::const_iterator end,
           vector<WordRange>& words,
           size_t max_word_len = MAX_WORD_LENGTH) const {
    vector<Dag> dags;
    dictTrie_->Find(begin, 
          end, 
          dags,
          max_word_len);
    CalcDP(dags);
    CutByDag(begin, end, dags, words);
  }

  const DictTrie* GetDictTrie() const {
    return dictTrie_;
  }

 private:
  void CalcDP(vector<Dag>& dags) const {
    size_t nextPos;
    const DictUnit* p;
    double val;

    for (vector<Dag>::reverse_iterator rit = dags.rbegin(); rit != dags.rend(); rit++) {
      rit->pInfo = NULL;
      rit->weight = MIN_DOUBLE;
      assert(!rit->nexts.empty());
      for (LocalVector<pair<size_t, const DictUnit*> >::const_iterator it = rit->nexts.begin(); it != rit->nexts.end(); it++) {
        nextPos = it->first;
        p = it->second;
        val = 0.0;
        if (nextPos + 1 < dags.size()) {
          val += dags[nextPos + 1].weight;
        }

        if (p) {
          val += p->weight;
        } else {
          val += dictTrie_->GetMinWeight();
        }
        if (val > rit->weight) {
          rit->pInfo = p;
          rit->weight = val;
        }
      }
    }
  }
  void CutByDag(RuneStrArray::const_iterator begin, 
        RuneStrArray::const_iterator end, 
        const vector<Dag>& dags, 
        vector<WordRange>& words) const {
    size_t i = 0;
    while (i < dags.size()) {
      const DictUnit* p = dags[i].pInfo;
      if (p) {
        assert(p->word.size() >= 1);
        WordRange wr(begin + i, begin + i + p->word.size() - 1);
        words.push_back(wr);
        i += p->word.size();
      } else { //single chinese word
        WordRange wr(begin + i, begin + i);
        words.push_back(wr);
        i++;
      }
    }
  }

  const DictTrie* dictTrie_;
  bool isNeedDestroy_;

}; // class MPSegment

} // namespace cppjieba

#endif
