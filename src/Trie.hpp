#ifndef _TRIE_HPP_
#define _TRIE_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>


namespace mmsegSpace {

typedef char16_t Char;// UTF-16
typedef std::u16string String;
typedef String::const_iterator StringIt;
typedef std::pair<StringIt, StringIt> StringP;// 词(分词后的一个词的开始结束位置)

class Trie {
	public:
		struct Node {// 字对应的子节点集合
			std::unordered_map<Char, Node*> trans_;
			bool val_ = false;// 标示是否存在某个词条
			
			~Node() {
				for (auto& it: trans_)
					delete it.second;
				trans_.clear();
			}
		};
		// 添加节点 字串中每个字对应的Node
		void add(const String& word) {
			Node *current = &root_;
			for (auto ch: word) {
				auto it = current->trans_.find(ch);
				if (it == current->trans_.end()) {
					auto ret = current->trans_.emplace(ch, new Node{});
					current = ret.first->second;
				}
				else {
					current = it->second;
				}
			}
			current->val_ = true;// 词条存在
		}
		// trie字串查找,区间[s,e]在trie中存的词存储在vector
		std::vector<StringP> match_all(StringIt start, StringIt end) {
			std::vector<StringP> ret;
			Node *current = &root_;
			StringIt _start = start;
			for (; start != end; ++start) {
				auto ch = *start;
				auto it = current->trans_.find(ch);
				if (it == current->trans_.end())
					break;
				current = it->second;
				if (current->val_) ret.emplace_back(_start, start + 1);
			}
			return ret;
		}
		
		const size_t size() const { 
			return root_.trans_.size(); 
		}
	private:
		Node root_;
  };
}
#endif
