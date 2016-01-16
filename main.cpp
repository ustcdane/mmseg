/*=============================================================================
#     FileName: main.cpp
#         Desc: mmseg c++ 实现 main 测试代码
#       Author: Daniel
#        Email: daneustc@gmail.com
#     HomePage: http://ustcdane.github.io/
#      Version: 0.0.1
#   LastChange: 2016-01-09 18:43:16
#
# modify from https://github.com/jdeng/mmseg
# Data files from mmseg4j https://code.google.com/p/mmseg4j/
# util from https://github.com/aszxqw/limonp
# Compile with:
# If use src/Mmseg.hpp, just include src/Mmseg.hpp then
# g++ -Ofast -march=native -funroll-loops -o mmseg -std=c++11 main.cpp
# If use src/Mmseg.h just include src/Mmseg.h then
# g++ -Ofast -march=native -funroll-loops -o mmseg -std=c++11  main.cpp src/Mmseg.cpp
# UTF-8 or gbk input
# MIT LICENSE
=============================================================================*/

#include "src/Mmseg.h"
using namespace mmsegSpace;

int main(int argc, const char *argv[]) {
  /*
   * MMSeg mmseg;
  mmseg.load("data/words.dic", "data/chars.dic");//use load method
  */
  // MMSeg mmseg("data/words.dic", "data/chars.dic");
  MMSeg mmseg = MMSeg::Instance("data/words.dic", "data/chars.dic");
  // 测试单例
  MMSeg mmseg2 = MMSeg::Instance("data/words.dic", "data/chars.dic");

  if (argc >= 2) {
    std::ifstream ifs(argv[1]);
    if (! ifs.good()) {
      std::cerr << "Failed to open " << argv[1] << std::endl;
      return -1;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    auto s = TransCode::from_utf8(ss.str());
    time_t now = time(0);
    auto w = mmseg.segment(s);
    std::cout << "Done in " << time(0)  - now << " seconds, " << w.size() << " words from " << s.size() << " chars" << std::endl;
    return 0;
  }

  while (true) {
    std::cout << "Input String: ";
    std::string line;
    if (! std::getline(std::cin, line)) break;
    std::u16string s = TransCode::from_utf8(trim(line));
	for (auto& w: mmseg.segment(s)) 
		std::cout << TransCode::to_utf8(w) << "  "; 
	std::cout << std::endl;
  }

  return 0;
}

