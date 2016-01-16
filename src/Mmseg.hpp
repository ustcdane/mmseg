#ifndef _MMSEG_H_
#define _MMSEG_H_

//  mmseg 代码的另外一种格式.hpp
// http://technology.chtsai.org/mmseg/
// Data files from mmseg4j https://code.google.com/p/mmseg4j/

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

#define DEBUG_LEVEL // 输出计算过程

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

private:
  bool is_inited;
  std::unordered_map<Char, int> char_freqs_;
  Trie dict_;

  static size_t length(const StringP& w) { return std::distance(w.first, w.second); }
  // 词组 重点部分
  struct Chunk {
    std::vector<StringP> words_;
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
        if (length(w) != 1) 
			continue;
        auto it = chars.find(*w.first);
        if (it != chars.end())
          degree_ += log(float(it->second));
      }
    }

    std::string to_string() const {
      std::string s;
      for (auto& w: words_) { 
		  s += TransCode::to_utf8(String(w.first, w.second)) + " "; 
	  }
      s += "(" + std::to_string(length_) + " " + std::to_string(mean_) + " " + std::to_string(var_) +" " + std::to_string(degree_) + ")";
      return s;
    }
  };// Chunk

 // 重点部分
  std::vector<Chunk> get_chunks(StringIt _start, StringIt _end, int depth) {
    std::vector<Chunk> ret;
	// "递归"实现
    std::function<void(StringIt, StringIt, int, std::vector<StringP>)> get_chunks_it = 
		[&] (StringIt start, StringIt end, int n, std::vector<StringP> segs) {
      if (n == 0 || start == end) {
        ret.emplace_back(std::move(segs), char_freqs_);
      }
      else {
        auto m = dict_.match_all(start, end);//trie
        for (auto& w: m) {
          auto nsegs = segs;
          auto len = length(w);
          nsegs.emplace_back(std::move(w));
          get_chunks_it(start + len, end, n - 1, std::move(nsegs));
        }
        segs.emplace_back(start, start +1 );
        get_chunks_it(start + 1, end, n - 1, std::move(segs));
      }
    };

    get_chunks_it(_start, _end, depth, std::vector<StringP>{});
    return ret;
  }

public:
  std::vector<String> segment(const String& s, int depth = 3) {
    std::vector<String> ret;
    auto start = s.begin(), end = s.end();
    while (start != end) {
      auto chunks = get_chunks(start, end, depth);
      auto best = std::max_element(chunks.begin(), chunks.end(), [&](const Chunk& x, const Chunk& y) {
        return std::tie(x.length_, x.mean_, x.var_, x.degree_) < std::tie(y.length_, y.mean_, y.var_, y.degree_);
      });

      auto& word = best->words_.front();// 取出分词结果最好的第一个分词结果
      start += length(word);// 处理后续字串
      ret.emplace_back(String(word.first, word.second));
#ifdef DEBUG_LEVEL // 输出计算过程
	  static int times;
	  if(s.begin() == word.first) times = 1;//处理一个新的字串
	  std::cout<<"Step: "<<times<<std::endl;
	  for(auto &item:chunks) {
		  cout<<item.to_string()<<std::endl;
	  }
	  std::cout<<"Best one is: "<<TransCode::to_utf8(ret.back())<<std::endl;
	  ++times;
#endif
    }//while
#ifdef DEBUG_LEVEL
	std::cout<<"Result is: ";
#endif
    return ret;
  }

  int load(const std::string& dict, const std::string& char_freqs = "") {
    if (is_inited) {// 已经加载词典
		//std::cout<<"already load dict!!!\n";
		return 0;
	}
	std::ifstream ifs(dict);
    if (!ifs.is_open()) {
		fprintf(stderr, "open %s failed!!", dict.c_str());
		return -1;
	}
    while (ifs.good()) {
      std::string line;
      if (! std::getline(ifs, line)) break;
	  auto word = TransCode::from_utf8(trim(line));
      dict_.add(word);
    }
    ifs.close();

    if (char_freqs.empty()) return 0;

    std::ifstream fin(char_freqs);
    if (!fin.is_open()) {
		fprintf(stderr, "open %s failed!!", char_freqs.c_str());
		return -1;
	}
    while (fin.good()) {
      std::string line;
      if (! std::getline(fin, line)) break;
      auto s = trim(line);
      size_t pos = s.find(' ');
      if (pos != std::string::npos) {
        auto word = TransCode::from_utf8(s.substr(0, pos));
        int freq = std::stoi(s.substr(pos+1));
        char_freqs_.emplace(word[0], freq);
      }
    }
    fin.close();
	is_inited = true;
    std::cout << "Loaded Dict: " << dict_.size() << ", Freq: " << char_freqs_.size() << std::endl;
    return 0;
  }
};
}
#endif

