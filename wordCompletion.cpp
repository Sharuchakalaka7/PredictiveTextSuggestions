#include <iostream>
#include <string>
#include <sstream>
#include <vector>

const int SIZE = 26;	// |Σ| = |{a-z}| = 26
const std::string EMPTY = "";

/**
 * Using a compressed trie with several flags (possibly implicit)
 */
class WordTrie {
	
	struct Node {
		int freq;						// freq > 0 is the implicit flag
		Node *maxFreq;					// descendant flagged (possibly self) with the maximum frequency
		std::vector<Node*> children;	// static array for each possible child in the array
		std::string substr;				// char sequence of the edge sequence that was compressed in compressed trie
		std::string word;				// by default empty, but if accessed, then stores the full query
		Node *parent;

		Node(Node *parent, std::string substr = "", int dist = 1)
			: freq(0), maxFreq(this), children(SIZE), substr(substr), word(""), parent(parent) {}
		~Node() {
			for (Node *child : children)
				if (child != nullptr) delete child;
		}
	};

	/** Find the node corresponding to the given query. Upon fail, find the last node
	 *  (knowing root always exists) that corresponds to a successful prefix. Implements
	 *  this node change to the given node reference so that the prefix iterator end pos
	 *  (prefix.length(), which is out of bounds in prefix string) can be returned
	 */
	int safeSearch(std::string &query, Node* &cur) {
		int i = 0;
		std::string prefix = "";
		Node *child;

		while (i < query.length() && (child = cur->children[query[i]-'a']) != nullptr) {
			prefix += query[i] + child->substr;
			if (prefix != query.substr(0, prefix.length())) break;
			cur = child;
			i += 1 + cur->substr.length();
		}
		return i;
	}

	/** Returns the position of the first mismatched character between the two given strings, assuming possible **/
	int strDiff(std::string &suffix, std::string &substr) {
		int j = 0;
		int minLen = 1+std::min(suffix.length(), substr.length());
		while (j < minLen && suffix[j] == substr[j]) ++j;
		return j;
	}

	/** Update the max frequence node ptr after accessing new node **/
	void setMaxFreq(std::string &query, Node *accessed) {
		int af = accessed->freq;

		for (Node *cur = accessed; cur != root; cur = cur->parent) {
			int maxf = cur->maxFreq->freq;
			if (af > maxf || (af == maxf && accessed->word <= cur->maxFreq->word))
				cur->maxFreq = accessed;
		}
	}

	Node *root;
  public:
	WordTrie() : root(new Node(nullptr)) {}
	~WordTrie() { delete root; }

	void print() {
		std::vector<Node*> stack;
		stack.push_back(root);

		while (!stack.empty()) {
			Node *cur = stack.back();
			stack.pop_back();

			if (cur->freq > 0)
				std::cout << cur->word << ", " << cur->freq << std::endl;

			for (int i = SIZE-1; i >= 0; --i)
				if (cur->children[i] != nullptr)
					stack.push_back(cur->children[i]);
		}
	}

	void access(std::string &query) {
		// search and find last valid node, giving three cases for finding the access node
		// implementation note: performing safeSearch also modifies cur to the advertised node
		//
		// case 1: child found - trivial
		// case 2: empty child - create new compressed node
		// case 3: existing child has prefix mismatch - decompress and branch new node
		Node *cur = root;
		int i = safeSearch(query, cur);

		// case 1 (implicit)
		if (i != query.length()) {
			char c = query[i]-'a';
			std::string suffix = (i+1 < query.length()) ? query.substr(i+1) : "";
			Node *tmp = cur->children[c];

			// case 2
			if (tmp == nullptr) {
				cur->children[c] = new Node(cur, suffix);
				cur = cur->children[c];

			// case 3
			} else {
				// if here, this we know there is mismatch in the child node substr
				// j := position of first character diff, after matching prefix
				int j = strDiff(suffix, tmp->substr);
				
				// create junction node
				cur->children[c] = new Node(cur, suffix.substr(0, j));
				cur = cur->children[c];
				cur->maxFreq = tmp->maxFreq;

				// replace tmp (previous child) under junction
				cur->children[tmp->substr[j]-'a'] = tmp;
				tmp->parent = cur;
				tmp->substr = (j+1 < tmp->substr.length()) ? tmp->substr.substr(j+1) : "";

				// make new node under junction for the new access
				if (j < suffix.length()) {
					cur->children[suffix[j]-'a'] = new Node(cur, (j+1 < suffix.length()) ? suffix.substr(j+1) : "");
					cur = cur->children[suffix[j]-'a'];
				}
			}
		}
		++(cur->freq);
		cur->word = query;
		setMaxFreq(query, cur);
	}

	void getCompletions(std::string &query) {
		int j = 0;
		int len = 0;
		std::string prefix = "";
		Node *cur = root;

		for (int i = 0; i < query.length(); ++i) {
			prefix += query[i];
			cur = (cur == nullptr) ? nullptr
				: (len == 0) ? cur->children[query[i]-'a']
				: (query[i] == cur->substr[j]) ? cur
				: nullptr;

			// No possible extensions for the remaining prefixes if ever nullptr encountered
			if (cur == nullptr) {
				std::cout << "No extension of " << prefix << std::endl;

			} else {
				std::cout << "Best extension of " << prefix << " is " << cur->maxFreq->word << std::endl;
				if (len == 0) {
					j = 0;
					len = cur->substr.length();
				} else if (++j == len) {
					j = 0;
					len = 0;
				}
			}
		}
	}
};

int main() {
	char c;
	std::string word;
	WordTrie W;

	while (std::cin >> c && c != 'x') {
		switch (c) {
			case 'p':
				W.print();
				break;
			case 'a':
				std::cin >> word;
				W.access(word);
				break;
			case 'g':
				std::cin >> word;
				W.getCompletions(word);
				break;
			default:
				break;
		}
	}
}
