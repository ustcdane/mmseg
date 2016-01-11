#ifndef _TRANSCODE_HPP_
#define _TRANSCODE_HPP_

#include "util/StringUtil.hpp"

#if defined(_LIBCPP_BEGIN_NAMESPACE_STD)
    #include <locale>
    #include <codecvt>// gcc has <codecvt>
#endif

#if defined _MSC_VER //vs need >= vs2010
    #include <locale>
    #include <codecvt>
#endif

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
  
  //  utf8转 gbk
    inline static std::string utf8ToGbk(const string& str) { // str UTF-8编码的“中文”字符串
#if defined(_LIBCPP_BEGIN_NAMESPACE_STD)
        std::wstring_convert<std::codecvt_utf8<wchar_t>>cvt_utf8;//UTF-8<->Unicode转换器
        std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>
            cvt_ansi(new std::codecvt<wchar_t, char, std::mbstate_t>("CHS"));//GBK<->Unicode转换器
        std::wstring ws = cvt_utf8.from_bytes(str);//UTF-8转换为Unicode
        std::string myGBK = cvt_ansi.to_bytes(ws);//Unicode转换为GBK
        return myGBK;
#endif
        return "NULL";
    }
};

#endif
