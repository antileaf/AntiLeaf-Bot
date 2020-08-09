<<<<<<< HEAD
#ifndef STRING_TOOLS_HPP
#define STRING_TOOLS_HPP

#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

namespace string_tools {
	string original(string s) { return s; }

	void erase_pre(string& s, char c = ' ') { // 去掉前缀的某个字符
		int i = 0;
		while (i < (int)s.size() && s[i] == c)
			i++;
		if (i == (int)s.size()) // 不知道要不要做特判
			s = "";
		else s = s.substr(i);
	}

	void erase_pre(string &s, const string &t) { // 不带错误检查
		s = s.substr(t.size());
	}

	void erase_suf(string& s, char c = ' ') { // 去掉前缀的某个字符
		int i = (int)s.size() - 1;
		while (i >= 0 && s[i] == c)
			i--;
		if (i < 0) // 不知道要不要特判
			s = "";
		else s = s.substr(0, i + 1);
	}

	void erase_suf(string& s, const string& t) { // 不带错误检查
		s = s.substr(0, s.size() - t.size());
	}

	bool start_with(const string& s, const string& t) {
		return s.find(t) == 0;
	}

	bool end_with(const string& s, const string& t) {
		return s.size() >= t.size() && s.rfind(t) == s.size() - t.size();
	}

	void length_check(string& s) {
		return;
		if ((int)s.size() > 200)
			s = s.substr(0, 200) + "\n……\n（过长部分已截断，共" + to_string(s.size()) + "字节）";
	}

	pair<string, string> split(const string &s, const string &t = " ") { // 从第一个t处把s分成两半，不保留t
		int i = s.find(t);
		if (i == string::npos)
			return make_pair(s, ""); // 找不到则返回s和空串
		return make_pair(s.substr(0, i), s.substr(i + (int)t.size()));
	}

	vector<string> splitv(string s, const string& t = " ") { // 把s按t分割成几个字符串
		vector<string> v;
		string a, b;
		do {
			tie(a, b) = split(s, t);
			v.push_back(a);
			s = move(b);
		} while (s != "");

		return v;
	}

	string urlencode(string str_source) {
		erase_pre(str_source);
		erase_suf(str_source);

		char const* in_str = str_source.c_str();
		int in_str_len = strlen(in_str);
		if (in_str_len == 0)
			return "出错了，麻烦联系绿鸽鸽看看咋回事";
		int out_str_len = 0;
		string out_str;
		register unsigned char c;
		unsigned char* to, * start;
		unsigned char const* from, * end;
		unsigned char hexchars[] = "0123456789ABCDEF";

		from = (unsigned char*)in_str;
		end = (unsigned char*)in_str + in_str_len;
		start = to = (unsigned char*)malloc(3 * in_str_len + 1);

		while (from < end) {
			c = *from++;

			if (c == ' ') {
				*to++ = '+';
			}
			else if ((c < '0' && c != '-' && c != '.') ||
				(c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') ||
				(c > 'z')) {
				to[0] = '%';
				to[1] = hexchars[c >> 4];
				to[2] = hexchars[c & 15];
				to += 3;
			}
			else {
				*to++ = c;
			}
		}
		*to = 0;

		out_str_len = to - start;
		out_str = (char*)start;
		free(start);
		return out_str;
	}
}

using namespace string_tools;

=======
#ifndef STRING_TOOLS_HPP
#define STRING_TOOLS_HPP

#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

namespace string_tools {
	string original(string s) { return s; }

	void erase_pre(string& s, char c = ' ') { // 去掉前缀的某个字符
		int i = 0;
		while (i < (int)s.size() && s[i] == c)
			i++;
		if (i == (int)s.size()) // 不知道要不要做特判
			s = "";
		else s = s.substr(i);
	}

	void erase_pre(string &s, const string &t) { // 不带错误检查
		s = s.substr(t.size());
	}

	void erase_suf(string& s, char c = ' ') { // 去掉前缀的某个字符
		int i = (int)s.size() - 1;
		while (i >= 0 && s[i] == c)
			i--;
		if (i < 0) // 不知道要不要特判
			s = "";
		else s = s.substr(0, i + 1);
	}

	void erase_suf(string& s, const string& t) { // 不带错误检查
		s = s.substr(0, s.size() - t.size());
	}

	bool start_with(const string& s, const string& t) {
		return s.find(t) == 0;
	}

	bool end_with(const string& s, const string& t) {
		return s.size() >= t.size() && s.rfind(t) == s.size() - t.size();
	}

	void length_check(string& s) {
		return;
		if ((int)s.size() > 200)
			s = s.substr(0, 200) + "\n……\n（过长部分已截断，共" + to_string(s.size()) + "字节）";
	}

	pair<string, string> split(const string &s, const string &t = " ") { // 从第一个t处把s分成两半，不保留t
		int i = s.find(t);
		if (i == string::npos)
			return make_pair(s, ""); // 找不到则返回s和空串
		return make_pair(s.substr(0, i), s.substr(i + (int)t.size()));
	}

	vector<string> splitv(string s, const string& t = " ") { // 把s按t分割成几个字符串
		vector<string> v;
		string a, b;
		do {
			tie(a, b) = split(s, t);
			v.push_back(a);
			s = move(b);
		} while (s != "");

		return v;
	}

	string urlencode(string str_source) {
		erase_pre(str_source);
		erase_suf(str_source);

		char const* in_str = str_source.c_str();
		int in_str_len = strlen(in_str);
		if (in_str_len == 0)
			return "出错了，麻烦联系绿鸽鸽看看咋回事";
		int out_str_len = 0;
		string out_str;
		register unsigned char c;
		unsigned char* to, * start;
		unsigned char const* from, * end;
		unsigned char hexchars[] = "0123456789ABCDEF";

		from = (unsigned char*)in_str;
		end = (unsigned char*)in_str + in_str_len;
		start = to = (unsigned char*)malloc(3 * in_str_len + 1);

		while (from < end) {
			c = *from++;

			if (c == ' ') {
				*to++ = '+';
			}
			else if ((c < '0' && c != '-' && c != '.') ||
				(c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') ||
				(c > 'z')) {
				to[0] = '%';
				to[1] = hexchars[c >> 4];
				to[2] = hexchars[c & 15];
				to += 3;
			}
			else {
				*to++ = c;
			}
		}
		*to = 0;

		out_str_len = to - start;
		out_str = (char*)start;
		free(start);
		return out_str;
	}
}

using namespace string_tools;

>>>>>>> e1fa7543481f021ac86a6771f02ffdd12868b3a0
#endif