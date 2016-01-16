
//  传统的.h,.cpp分开  可以编译出动态链接库

#include "Mmseg.h"

#define DEBUG_LEVEL // 输出详细的计算过程
namespace mmsegSpace{
// 重点部分
  std::vector<MMSeg::Chunk> MMSeg::get_chunks(StringIt _start, StringIt _end, int depth) {
    std::vector<Chunk> ret;
    // "递归"实现
	std::function<void(StringIt, StringIt, int, std::vector<StringP>)> get_chunks_it = [&] (StringIt start, StringIt end, int n, std::vector<StringP> segs) {
      if (n == 0 || start == end) {
        ret.emplace_back(std::move(segs), char_freqs_);
      }
      else {
        auto m = dict_.match_all(start, end);
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

  std::vector<String> MMSeg::segment(const String& s, int depth) {
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
    }// while
#ifdef DEBUG_LEVEL
	    std::cout<<"Result is: ";
#endif
    return ret;
  }
  // 加载字典
  int mmsegSpace::MMSeg::load(const std::string& dict, const std::string& char_freqs) {
	if (is_inited) // 已经加载词典
		return 0;
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

    std::cout << "Loaded Dict: " << dict_.size() << ", Freq: " << char_freqs_.size() << std::endl;
    is_inited = true;
	return 0;
  }
}

