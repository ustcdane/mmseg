#ifndef _MMSEG_H_
#define _MMSEG_H_

// mmseg 代码的一种格式.h .cpp
//  http://technology.chtsai.org/mmseg/
// Data files from mmseg4j https://code.google.com/p/mmseg4j/
// UTF-8 or gbk input

#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>

#include "Trie.hpp"
#include "TransCode.hpp"
#include "util/StringUtil.hpp"

namespace mmsegSpace {
using namespace utilSpace;

class MMSeg {
public:
	MMSeg():is_inited(false){}
	MMSeg(const std::string& dict, const std::string& char_freqs = ""):is_inited(false)
	{
		if(load(dict,char_freqs)==-1) {
			fprintf(stderr, "MMseg construct failed , dict load error!");
		}
	}
	// 单例
	static MMSeg& Instance(const std::string& dict, const std::string& char_freqs) {
		static MMSeg instance;
		instance.load(dict, char_freqs);
		return instance;
	}
public:
  std::vector<String> segment(const String& s, int depth = 3);
  int load(const std::string& dict, const std::string& char_freqs = "");

private:
  bool is_inited;
  std::unordered_map<Char, int> char_freqs_;
  Trie dict_;

  static size_t length(const StringP& w) { return std::distance(w.first, w.second); }
// 词组 重点部分
  struct Chunk {
    std::vector<StringP> words_;// 存储词组中各个词
    size_t length_ = 0;// 备选词组合的长度
    float mean_ = 0, var_ = 0, degree_ = 0;// 平均词长,词长变化,单字频率

    Chunk(std::vector<StringP> words, const std::unordered_map<Char, int>& chars) : words_(std::move(words)) {
		length_ = std::accumulate(words_.begin(), words_.end(), size_t(0), 
				[&](size_t n, const StringP& w) { return n + length(w); });
      mean_ = float(length_) / words_.size();
      var_ = - std::accumulate(words_.begin(), words_.end(), float(0), 
			  [&](size_t n, const StringP& w) { return  n + (length(w) - mean_) * (length(w) - mean_); }) / words_.size();
	// sum of degree of morphemic freedom of one-character words
      for (auto& w: words_) {
        if (length(w) != 1) continue;
        auto it = chars.find(*w.first);
        if (it != chars.end())
          degree_ += log(float(it->second));
      }
    }

    std::string to_string() const {
      std::string s;
      for (auto& w: words_) { s += TransCode::to_utf8(String(w.first, w.second)) + " "; }
      s += "(" + std::to_string(length_) + " " + std::to_string(mean_) + " " + std::to_string(var_) + " " + std::to_string(degree_) + ")";
      return s;
    }
  };

  std::vector<Chunk> get_chunks(StringIt _start, StringIt _end, int depth);

};
}

#endif
