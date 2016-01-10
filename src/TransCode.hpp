#ifndef _TRANSCODE_HPP_
#define _TRANSCODE_HPP_

#include "util/StringUtil.hpp"

using namespace utilSpace;

class TransCode {
public:
  // from utf16 tans to utf8(or gbk)
  inline static std::string to_utf8(const std::u16string& in) {
    std::string out;
#ifdef MMSEG_GBK
	gbkTrans(in.begin(), in.end(), out);
#else
	unicodeToUtf8(in.begin(), in.end(), out);
#endif
	return out;
  }
  // from utf8(or gbk) trans to utf16
  inline static std::u16string from_utf8(const std::string& in) {
	std::u16string out;
#ifdef MMSEG_GBK
	gbkTrans(in, out);
#else
	utf8ToUnicode(in, out);
#endif
	return out;
  }
};

#endif
