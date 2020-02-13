#include <cqcppsdk/cqcppsdk.h>

#include <functional>
#include <iostream>
#include <cstdio>
#include <regex>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <ctime>
#include <cstdlib>

// #include "python.h"

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;
auto at = MessageSegment::at; // 这个auto感觉好蠢

const string cq_path = ""; // "C:\\Users\\AntiLeaf\\Desktop\\酷Q Pro\\";
const string PREF = "% *";
mt19937 gen;
const int64_t self_id = 625938638, producer_id = 1094054222;
const set<int64_t> ENABLED_GROUPS = {441254450, 228725393, 780135491, 463471261, 301538258, 1047201693};
const set<int64_t> NOTICE_GROUPS = {441254450, 228725393, 780135491, 1047201693};
const set<int64_t> ENABLED_USERS = {1094054222, 780768723}; // AntiLeaf, KsKun

namespace string_tools {
	
	string original(string s) {return s;}

	void erase_pre(string &s, char c = ' ') { // 去掉前缀的某个字符
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

	void erase_suf(string &s, char c = ' ') { // 去掉前缀的某个字符
		int i = (int)s.size() - 1;
		while (i >= 0 && s[i] == c)
			i--;
		if (i < 0) // 不知道要不要特判
			s = "";
		else s = s.substr(0, i + 1);
	}

	void erase_suf(string &s, const string &t) { // 不带错误检查
		s = s.substr(0, s.size() - t.size());
	}

	bool start_with(const string &s, const string &t) {
		return s.find(t) == 0;
	}

	bool end_with(const string &s, const string &t) {
		return s.size() >= t.size() && s.rfind(t) == s.size() - t.size();
	}

	pair<string, string> split(const string &s, const string &t = " ") { // 从第一个t处把s分成两半，不保留t
		int i = s.find(t);
		if (i == string::npos)
			return make_pair(s, ""); // 找不到则返回s和空串
		return make_pair(s.substr(0, i), s.substr(i + (int)t.size()));
	}

	vector<string> splitv(string s, const string &t = " ") { // 把s按t分割成几个字符串
		vector<string> v;
		string a, b;
		do {
			tie(a, b) = split(s, t);
			v.push_back(a);
			s = move(b);
		} while (s != "");

		return v;
	}

	string urlencode(string str_source){
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

namespace message_tools { // 发消息工具

	const int MAX_TRIES = 5;
	// const int DELAY = 500;

	void send_message(int64_t user_id, string s) {
		for (int k = 0; k < MAX_TRIES; k++) {
			try {
				send_message(Target::user(user_id), s);
				break;
			}
			catch (ApiError&) {}
			catch (...) {
				logging::error("错误", "未知错误");
				break;
			}
		}
	}

	void send_group_message(int64_t group_id, string s) {
		for (int k = 0; k < MAX_TRIES; k++) {
			try {
				cq::send_group_message(group_id, s);
				break;
			}
			catch (ApiError&) {}
			catch (...) {
				logging::error("错误", "未知错误");
				break;
			}
		}
	}
}

auto send_message = message_tools::send_message;
auto send_group_message = message_tools::send_group_message;

using namespace message_tools;

namespace collections {
	class collection {

	private:
		string file_name;

		vector<string> v;

	public:
		collection(string file_name) : file_name(file_name) {}

		~collection() = default;

		int size() const {
			return (int)v.size();
		}

		string get() const {
			if (v.empty())
				return "现在还是空空的哦…";
			return v[gen()%(int)v.size()];
		}

		string add(const string &s) {
			for (auto &t : v)
				if (s == t)
					return "已经添加过了哦~";
			v.push_back(s);
			return "添加成功"; // 添加成功
		}

		string load() {
			try {
				v.clear();
				ifstream f(cq_path + file_name + ".txt");
				string s;
				while (getline(f, s) && s != "")
					v.push_back(s);
				f.close();
				return "加载完成";
			}
			catch (...) {
				return "加载失败，检查一下出了什么问题吧…";
			}
		}

		string save() const { // 返回是否执行成功
			try {
				string s;
				for (auto t : v)
					s += t + "\n";
				ofstream f(cq_path + file_name + ".txt");
				f << s;
				f.close();
				return "存档成功";
			}
			catch(...) {
				return "存档失败，检查一下出了什么问题吧…";
			}
		}
	} *poems;

	void init() {
		poems = new collection("poems");
		poems->load();
	}
}

namespace checker_and_handler {

	using namespace string_tools;

	class checker {

	protected:
		bool is_finally = true; // 是否在识别为该类型指令后截断，默认为真，目前尚未启用

	public:
		checker() = default;

		virtual ~checker() = default;

		virtual bool check (const GroupMessageEvent&) = 0;

		bool is_final() const {
			return is_finally;
		}
	};

	class multiple_checker : public checker { // 复合检查

	protected:
		vector<checker*> v;

	public:
		multiple_checker(vector<checker*> &v) : checker(), v(v) {}

		virtual ~multiple_checker() {
			for (auto o : v)
				delete o;
		}

		virtual bool check (const GroupMessageEvent&) = 0;
	};

	class and_checker final : public multiple_checker { // 与

	public:
		and_checker(vector<checker*> &&v) : multiple_checker(v) {}

		~and_checker() = default;

		bool check (const GroupMessageEvent &e) {
			bool ans = true;

			for (auto o : v)
				ans &= o->check (e);

			return ans;
		}
	};

	class or_checker final : public multiple_checker { // 或
		
	public:
		or_checker(vector<checker*> &&v) : multiple_checker(v) {}

		~or_checker() = default;

		bool check(const GroupMessageEvent &e) {
			bool ans = false;

			for (auto o : v)
				ans &= o->check(e);

			return ans;
		}
		
	};

	class not_checker final : public checker { // 非
		
		checker *o;

	public:
		not_checker(checker *o) : checker(), o(o) {}

		~not_checker() {
			delete o;
		}

		bool check(const GroupMessageEvent &e) {
			return !o->check(e);
		}
	};

	class regex_checker : public checker { // 正则表达式

	protected:
		regex r;

	public:
		regex_checker(string t) : r(t) {}

		virtual ~regex_checker() = default;

		virtual bool check(const GroupMessageEvent&) = 0;
	};

	class complete_match final : public regex_checker { // 正则表达式（完全匹配）

	public:
		complete_match(string t) : regex_checker(PREF + t) {} // 注意这里已经加了指令前缀

		// complete_match(Message m) : regex_checker(PREF + (string)m) {} // 这里也加了指令前缀

		~complete_match() = default;

		bool check(const GroupMessageEvent &e) {
			return regex_match(e.message, r);
		}
	};

	class presuf_match final : public regex_checker { // 前后缀匹配
		
	public:
		presuf_match(string pre, string suf = "") : regex_checker(PREF + pre + "[\\s\\S]*" + suf) {}

		// presuf_match(Message pre, Message suf = "") : regex_checker(PREF + (string)pre + "[\\s\\S]*" + (string)suf) {}

		~presuf_match() = default;

		bool check(const GroupMessageEvent &e) {
			return regex_match(e.message, r);
		}
	};

	class substr_match final : public regex_checker { // 子串匹配

	public:
		substr_match(string t) : regex_checker(t) {}

		// substr_match(Message m) : regex_checker((string)m) {}

		~substr_match() = default;

		bool check(const GroupMessageEvent &e) {
			return regex_search(e.message, r);
		}
	};

	class custom_checker : public checker { // 自定义检查器

	public:
		custom_checker() = default;

		virtual ~custom_checker() = default;

		virtual bool check(const GroupMessageEvent&) = 0;
	};

	class repeater_check final : public custom_checker {

		std::map<int64_t, string> o;

	public:
		repeater_check() = default;

		~repeater_check() = default;

		bool check(const GroupMessageEvent &e) {
			auto &last = o[e.group_id];
			if (e.message == last) {
				last = "";
				return true;
			} else
				last = e.message;

			return false;
		}
	};

	class id_check final : public custom_checker { // 检查是否由特定id发送
	
		int64_t id;

	public:
		id_check(int64_t id) : id(id) {}

		~id_check() = default;

		bool check(const GroupMessageEvent &e) {
			return e.user_id == id;
		}
	};

	/*
	class always_check final : public custom_checker {
		
		bool value;

	public:
		always_check(bool value) : value(value) {}

		~always_check() = default;

		bool check(const GroupMessageEvent &e) {
			if (regex_match(e.message, regex(PREF + "[\\S\\s]*")))
				return value;
			return false;
		}
		
	};
	*/

	// ------------------------------------------------------
	// --------------------- handler ------------------------
	// ------------------------------------------------------

	class handler {

	protected:
		bool at = false;

	public:
		handler(bool at = false) : at(at) {}

		virtual ~handler() = default;

		virtual string reply(const GroupMessageEvent &) = 0; // 重要！不再支持多条消息回复，可通过调用多个handler完成
	};

	class multiple_reply final : public handler { // 同时对一条消息执行多个回复，分行输出

		vector<handler*> v;

	public:
		multiple_reply(vector<handler*> v, bool at = false) : handler(at), v(v) {}

		~multiple_reply() = default;

		string reply(const GroupMessageEvent &e) {
			string u;
			for (auto o : v) {
				if (o != v.front())
					u += "\n";
				u += o->reply(e);
			}

			if (at)
				u = ::at(e.user_id) + u;
			return u;
		}
	};

	class orderly_reply final : public handler { // 对一条消息依次（递归）执行多个回复
		vector<handler*> v;

	public:
		orderly_reply(vector<handler*> &v, bool at = false) : handler(at), v(v) {}

		~orderly_reply() = default;

		string reply(const GroupMessageEvent &ee) {
			
			auto e = ee;
			logging::info("debug", e.message);
			for (auto o : v) {
				e.message = o->reply(e);
				logging::info("debug", e.message);
			}
			if (at)
				e.message = ::at(e.user_id) + " " + e.message;
			
			return e.message;
		}
	};

	class fixed_reply final : public handler { // 固定回复

		string s;

	public:
		fixed_reply(string s, bool at = false) : handler(at), s(s) {}

		~fixed_reply() = default;

		string reply(const GroupMessageEvent &e) {
			if (at)
				return ::at(e.user_id)+ " " + s;
			else return s;
		}
	};

	class random_reply final : public handler {

		vector<string> v;

	public:
		random_reply(vector<string> &v, bool at = false) : handler(at), v(v) {}

		~random_reply() = default;

		string reply(const GroupMessageEvent &e) {
			string t = v[gen()%v.size()];
			if (at)
				t = ::at(e.user_id) + " " + t;
			return t;
		}
	};

	class single_para_reply final : public handler { // 单参数回复
		
		string pre, suf, p, s;

	public:
		single_para_reply(string pre, string suf, string p, string s) :
			pre(pre), suf(suf), p(p), s(s) {}

		string reply(const GroupMessageEvent &e) {
			string t = e.message/*.substr(1)*/; // assert(t[0] == "#");
			int i = 0;
			while (i < (int)t.size() && (t[i] == ' ' || t[i] == '\t'))
				i++;
			t = t.substr(i); // 去除前缀空格

			i = t.find(pre); // assert(i != string::npos);
			t = t.substr(i + (int)pre.size());

			if (!t.empty()) {
				i = t.rfind(suf);
				t = t.substr(0, i);
			}

			return p + t + s;
		}
	};

	class custom_reply : public handler { // 自定义回复
		
	public:
		custom_reply(bool at = false) : handler(at) {}

		virtual ~custom_reply() = default;

		virtual string reply(const GroupMessageEvent&) = 0;
	};

	class repeater_reply final : public custom_reply { // 我是一只复读机
		
	public:
		repeater_reply() : custom_reply(false) {}

		~repeater_reply() = default;

		string reply(const GroupMessageEvent &e) {
			return e.message;
		}
	};

	class python_reply final : public custom_reply { // Python功能
		
	public:
		python_reply(bool at = false) : custom_reply(at) {}

		~python_reply() = default;

		string reply(const GroupMessageEvent &e) {
			bool auth = false;
			for (auto o : ENABLED_USERS)
				auth |= (o == e.user_id);
			if (!auth)
				return "珍爱绿鸽鸽的电脑，不要再拿我开涮了！QAQ";
			
			ofstream g("temp.txt");
			g << e.message;
			g.close();
			system("python eval.pyw < temp.txt > output.txt");
			ifstream f("output.txt");
			ostringstream os;
			os << f.rdbuf();
			f.close();
			return os.str(); // TODO : 这里写的好丑，有空改一下
		}
	};

	class url_reply final : public custom_reply { // 搜索引擎回复
		
		string pre, suf;

	public:
		url_reply(string pre, string suf, bool at = false) : custom_reply(at), pre(pre), suf(suf) {}

		~url_reply() = default;

		string reply(const GroupMessageEvent &e) {
			using string_tools::urlencode;

			string s = e.message;
			erase_pre(s);

			if (s == "")
				return "不要试图搜索空的东西哦~";

			erase_suf(s);

			// assert(!at); // 只应作为中间步骤使用，不应当有at
			return pre + urlencode(e.message) + suf;
		}
	};

	using collections::collection;

	class collection_reply final : public custom_reply { // 收集回复，集成添加、加载、存档、查询功能

		collection *o;

	public:
		collection_reply(collection *o, bool at = false) : custom_reply(at), o(o) {}

		~collection_reply() {}

		string reply(const GroupMessageEvent &e) {
			// logging::info("??", "232323");
			// return "有毒吧啊啊啊啊啊啊";
			string s = e.message;
			erase_pre(s);
			bool auth = false;
			for (auto o : ENABLED_USERS)
				auth |= (o == e.user_id);
			if (start_with(s, "添加")) {
				if (!auth)
					return "抱歉，只有绿鸽鸽和ks才有这个权限哦~";
				erase_pre(s, "添加");
				erase_pre(s);
				erase_suf(s);
				return o->add(s);
			}
			else if (start_with(s, "加载")) {
				if (!auth)
					return "抱歉，只有绿鸽鸽和ks才有这个权限哦~";
				erase_pre(s, "加载");
				return o->load();
			}
			else if (start_with(s, "存档")) {
				if (!auth)
					return "抱歉，只有绿鸽鸽和ks才有这个权限哦~";
				erase_pre(s, "存档");
				return o->save();
			}
			else if (s == "") // 查询
				return o->get();
			return "指令有误，重新试一次吧~";
		}
	};

	class oeis_reply final : public custom_reply { // OEIS查询，后面再接一个url最好
		
	private:
		const string program_name = "tem.pyw";

	public:
		oeis_reply(bool at = false) : custom_reply(at) {}

		~oeis_reply() = default;

		string reply(const GroupMessageEvent &e) {
			string p = urlencode(e.message);
			logging::info("debug", p);
			ofstream f("oeis.txt");
			f << p;
			f.close();
			system(("python " + program_name + " < oeis.txt").c_str());

			string u, s, t;
			ifstream g("oeis.txt");
			int k = 5;
			u = "以下是前" + to_string(k) + "个搜索结果：";
			while (g >> s >> t && k--) {
				u += "\n" + s + " " + t;
			}
			g.close();
			return u;
		}
	};

} // namespace checker_and_handler

namespace private_functions { // 私聊功能

	class evaluator {
		
	public:
		evaluator() = default;

		string reply(string s) {
			return s; // 我是一只复读机
		}
	} *eval = new evaluator;
}

namespace backend_functions { // 给自己用的后台功能

	class evaluator {
		
		enum class backend_status {DISABLED, PRIVATE, GROUP} status;
		int64_t id;
		map<int64_t, string> cards, nicks;

	public:
		evaluator() : status(backend_status::DISABLED), id(0) {}

		~evaluator() = default;

		string reply(string s) {
			if (regex_match (s, regex(" *%[\\s\\S]*"))) {
				id = 0;
					for (auto c : s)
						if (isdigit(c))
							id = id * 10 + c - '0';
				
				if (regex_match(s, regex(" *\\% *群聊 *[0-9]*"))) { // 群聊转发
					cards.clear();
					for (auto o : get_group_member_list(id)) {
						cards[o.user_id] = o.card; // 初始化群名片和昵称
						nicks[o.user_id] = o.nickname;
					}
					cards[0] = "（匿名用户）";
					nicks[0] = "匿名用户";

					status = backend_status::GROUP;
					return "已切换至群聊" + to_string(id);
				}

				else if (regex_match(s, regex(" *% *私聊 *[0-9]*"))) { // 私聊转发
					status = backend_status::PRIVATE;
					return "已切换至私聊" + to_string(id);
				}

				else if (regex_match(s, regex(" *% *关闭"))) { // 关闭
					status = backend_status::DISABLED;
					return "转发功能已关闭";
				}
				
				return "非法输入，请重试";
			}

			else {
				if (status == backend_status::DISABLED)
					return "功能未开启";
				else if (status == backend_status::GROUP)
					message_tools::send_group_message(id, s);
				else if (status == backend_status::PRIVATE)
					message_tools::send_message(id, s);
				return "";
			}
		}

		bool check_group(int64_t id) {
			return status == backend_status::GROUP && this->id == id;
		}

		bool check_private(int64_t id) {
			return status == backend_status::PRIVATE && this->id == id;
		}

		string get_name(int64_t id) {
			return cards[id] + " （" + nicks[id] + "）";
		}
	} *eval = new evaluator();
}

namespace enabled_functions {

	using namespace checker_and_handler;

	vector<pair<checker*, handler*>> functions;

	vector<pair<vector<string>, string>> websites = { // 常用网站
		{{"百度", "百度搜索", "baidu"}, "www.baidu.com"},
		{{"谷歌", "google", "Google", "谷歌搜索"}, "www.google.com"},
		{{"谷歌翻译", "翻译"}, "translate.google.cn"},
		{{"淘宝", "淘宝网", "桃饱网"}, "www.taobao.com"},
		{{"京东", "jd", "JD", "二手东"}, "www.jd.com"},
		{{"b站", "哔哩哔哩", "bilibili"}, "www.bilibili.com"},
		{{"oeis", "OEIS"}, "oeis.org"},
		{{"op.gg", "OP.GG"}, "op.gg"},
		{{"cf", "CF", "codeforce", "codeforces"}, "codeforces.com"},
		{{"UOJ", "uoj", "Universal OJ", "universal oj", "universal OJ"}, "uoj.ac"},
		{{"LOJ", "loj", "Loj", "LibreOJ", "Libreoj", "libreoj"}, "loj.ac"},
		{{"atcoder", "Atcoder", "AtCoder"}, "atcoder.jp"},
		{{"Pixiv", "P站", "p站", "pixiv", "PIXIV"}, "www.pixiv.net （不要看人家的本子嘛~[CQ:face,id=111]）"},
		{{"atcoder", "Atcoder", "AtCoder"}, "atcoder.jp"},
		{{"Pornhub", "pornhub", "PornHub", "P\\*\\**Hub", "P\\*\\**hub", "p\\*\\**hub", "\\*\\**hub", "\\*\\**Hub", "\\*\\**HUB"}, "\n[CQ:face,id=26]咿，你要看什么奇怪的网站，是不是想看我的本子！太可怕了QAQ"},
		{{"Vijos", "vijos", "VIJOS"}, "vijos.org"},
		{{"bzoj", "Bzoj", "BZOJ", "LYDSY", "lydsy", "八中oj", "八中OJ", "板子OJ", "板子oj"}, "www.lydsy.com"},
		{{"GitHub", "Github", "github", "gayhub", "GayHub", "Gayhub"}, "github.com"},
		{{"POJ", "Poj", "poj"}, "poj.org"},
		{{"OneDrive", "onedrive", "Onedrive"}, "onedrive.live.com"},
		{{"微云", "Weiyun", "weiyun"}, "weiyun.com"},
		{{"百度网盘", "度盘"}, "pan.baidu.com"},
		{{"洛谷", "Luogu", "luogu", "锣鼓"}, "luogu.org"},
		{{"OierDb", "oierdb", "oier", "OIerdb", "OIerDb", "OierDB", "OIerDB"}, "bytew.net/OIer"},
		{{"知乎", "zhihu", "逼乎", "某乎"}, "zhihu.com"},
		{{"维基百科", "维基", "wiki", "wikipedia", "Wikipedia"}, "zh.wikipedia.org"},
		{{"yande.re", "yande", "Yande"}, "yande.re"},
		{{"Steam", "steam", "STEAM"}, "store.steampowered.com"},
		{{"Wolfram Alpha", "wolfram *alpha", "Wolfram *Alpha"}, "https://www.wolframalpha.com/"},
	};

	vector<pair<vector<string>, string>> live = { // 直播速达
		{{"AntiLeaf", "绿鸽鸽", "绿哥哥", "antileaf", "反叶子", "反对叶子", "绿叶", "绿"}, "live.bilibili.com/21493332"},
		{{"KS", "ks", "KsKun", "Kskun", "kskun"}, "https://ksmeow.moe/live"},
		{{"M晓", "m晓", "咕咕晓"}, "live.bilibili.com/30014"},
		{{"Megumin", "megumin", "蘑菇民", "蘑菇米"}, "live.bilibili.com/14866481"},
		{{"北啻", "北帝口"}, "live.bilibili.com/152745"},
		{{"Ringoer", "ringoer"}, "live.bilibili.com/706938"},
		{{"Kamigen", "kamigen"}, "live.bilibili.com/9508073"},
		{{"Pantw", "pantw", "PTW", "Ptw", "ptw", "群主"}, "live.bilibili.com/4299357"},
		{{"MegaOwler", "Megaowler", "megaowler", "百万猫头鹰"}, "live.bilibili.com/917033"},
		{{"Slanterns", "SLanterns", "slanterns", "slantern", "Slantern"}, "live.bilibili.com/627355"},
		{{"Democracy", "democracy"}, "live.bilibili.com/4620643"},
		{{"icy", "Icy", "绿泡泡", "ICY"}, "live.bilibili.com/8815853"},
	};

	vector<pair<vector<string>, vector<string>>> conver = { // 对话
		{{"爬", "给爷爬", "爪巴", "给爷爪巴"}, {"你怎么能让可爱的我爬呢，你坏坏QAQ"}},
		{{"AntiLeaf", "antileaf", "绿鸽鸽", "绿哥哥"}, {"绿鸽鸽好帅，最喜欢他了！QwQ"}},
		{{"呐呐", "呐呐呐", "呐 呐", "呐 呐 呐"}, {"自 动 声 呐 系 统", "巧了我也是二次元[CQ:face,id=109]"}},
	};

	vector<pair<pair<string, string>, pair<string, string>>> single_para = { // 单参数命令
		{{"echo", ""}, {"", ""}},
		{{"输出", ""}, {"", ""}}
	};

	vector<pair<vector<string>, vector<string>>> dear = { // 贴贴
		{{"亲亲", "\\[CQ:face,id=109\\]"},{"[CQ:face,id=109][CQ:face,id=109][CQ:face,id=109]"}},
		{{"贴贴"}, {"绿鸽鸽好帅，我和绿鸽鸽贴贴[CQ:face,id=109][CQ:face,id=109]"}},
		{{"说几句好听的", "说两句好听的", "来两句好听的", "来几句好听的", "来句好听的", "说句好听的"}, {"绿鸽鸽最帅了[CQ:face,id=109]", "我最喜欢绿鸽鸽了[CQ:face,id=109]", "绿鸽鸽帅帅！[CQ:face,id=109]"}},
		{{"卖个萌", "来卖个萌", "卖萌", "qwq", "qwqwq"}, {"[CQ:face,id=111][CQ:face,id=111][CQ:face,id=111]", "QwQ", "别嘛~~人家要害羞了啦~~"}},
	};

	// 诗词句子收集写在专用的命名空间中

	vector<pair<vector<string>, vector<string>>> greetings = { // 问好
		{{"早安", "说早安", "早上好", "早"}, {"早安~", "早上好~新的一天也要元气满满哦！"}},
		{{"上午好"}, {"上午好~事不宜迟，开始忙碌的一天吧！" }},
		{{"午安", "说午安", "中午好"}, {"午安~", "中午好~不要忘记睡午觉喔~"}},
		{{"下午好"}, {"下午好~吃点什么零食呢？"}},
		{{"晚上好"}, {"晚上好~晚上干活虽好，也要注意少熬夜呢~"}},
		{{"晚安", "说晚安"}, {"晚安，好好休息一下吧~"}},
	};

#define add_function(a, b) functions.push_back(make_pair((new a), (new b)))
#define add_fixed(a, b, c) add_function(complete_match(a), fixed_reply(b, c))
#define and_check(a, b) and_checker(vector<checker*>({new a, new b}))
#define multi_reply(a, b, c) multiple_reply(vector<handler*>({new a, new b}), c)
#define order_reply(a, b, c) orderly_reply(vector<handler*>({new a, new b}), c)

	void init() {

		for (auto s : {"help", "帮助"})
		add_function(complete_match(s), fixed_reply("\nBot v0.5  By AntiLeaf\n功能\t\t\t使用方法\n----------------------------\n复读\t\t\t你最好不要知道\n\
网站查询\t\t%网站+你想查询的网站\n直播查询\t\t%直播+你想查询的群友常用id\n随机查房\t\t%随机查房\n（网站列表与直播请联系绿鸽鸽提供）\n\
百度/谷歌搜索\t%百度/谷歌一下+你想搜索的东西\nPython\t\t\t%python/py+你想执行的指令\nOEIS\t\t\toeis/OEIS+你想搜索的东西\n输出（请勿刷屏）%echo/%输出+你想输出的东西\n贴贴\t\t\t只有绿鸽鸽钦定的人才能使用\n帮助\t\t\t%help/%帮助\n诗词收集\t\t%诗词+内容\
\n（添加+诗词：添加句子 存档：保存已收集诗词\n加载：读取已存档的诗词 无内容：随机返回已收集诗词）\n\n\
最近更新：添加OEIS功能\n下一步计划：添加Wolfram Alpha查询等爬虫功能\nGitHub仓库：https://github.com/AntiLeaf/AntiLeaf-Bot", true));

		for (auto p : websites)
			for (auto o : p.first)
				add_function(complete_match("网站 *" + o), fixed_reply(p.first[0] + "： " + p.second, true)); // 网站查询

		string s = "支持的网站列表：";
		for (auto p : websites)
			s += "\n" + p.first[0];
		add_function(and_check(id_check(1094054222), complete_match("网站列表")), fixed_reply(s, false)); // 网站列表

		for (auto p : live)
			for (auto o : p.first)
				for (string s : {"直播", "查房"})
					add_fixed(s + " *" + o, p.first[0] + "的直播间： " + p.second, true); // 直播查询

		vector<string> v;
		for (auto p : live)
			v.push_back(p.first[0] + "的直播间： " + p.second);
		add_function(complete_match("随机查房 *"), random_reply(v, true)); // 随机查房

		s = "直播列表：";
		for (auto p : live)
			s += "\n" + p.first[0] + " : " + p.second;
		add_function(and_check(id_check(1094054222), complete_match("直播列表")), fixed_reply(s, false)); // 网站列表

		for (auto p : conver)
			for (auto o : p.first)
				add_function(complete_match(o), random_reply(p.second, true)); // 对话

		for (auto p : single_para)
			add_function(presuf_match(p.first.first, p.first.second), single_para_reply(p.first.first, p.first.second, p.second.first, p.second.second)); // 单参数命令

		for (auto s : {"百度一下", "baidu一下", "Baidu一下", "百度", "baidu", "Baidu"})
			add_function(presuf_match(s, ""), order_reply(single_para_reply(s, "", "", ""), url_reply("www.baidu.com/s?wd=", ""), true)); // 百度搜索

		for (auto s : {"谷歌一下", "Google一下", "google一下", "谷歌", "google", "Google"})
			add_function(presuf_match(s, ""), order_reply(single_para_reply(s, "", "", ""), url_reply("www.google.com/search?q=", ""), true)); // 谷歌搜索

		for (auto s : {"Python", "python", "py", "Py"})
			add_function(presuf_match(s, ""), order_reply(single_para_reply(s, "", "", ""), python_reply(false), true)); // Python

		for (auto p : dear)
			for (auto o : p.first)
				for (auto id : ENABLED_USERS)
					add_function(and_check(id_check(id), complete_match(o)), random_reply(p.second, true)); // 贴贴（仅限自己使用）
		for (auto p : dear)
			for (auto o : p.first)
				add_function(complete_match(o), fixed_reply("请你吃桃子", true)); // 贴贴（让群友吃桃子）

		for (auto p : greetings)
			for (auto o : p.first)
				add_function(complete_match(o), random_reply(p.second, true)); // 问好

		add_function(presuf_match("诗词", ""), order_reply(single_para_reply("诗词", "", "", ""), collection_reply(collections::poems), true)); // 诗词收集

		for (auto s : {"OEIS", "oeis", "Oeis"})
			add_function(presuf_match(s, ""), order_reply(single_para_reply(s, "", "", ""), multi_reply(oeis_reply(), url_reply("更多结果： http://oeis.org/search?q=", ""), false), true));

		add_function(repeater_check, repeater_reply); // 复读机
		
		// TODO : 添加更多功能

		add_function(presuf_match(""), fixed_reply("不好意思，人家现在还听不懂哦~[CQ:face,id=111]\n如有需要可以联系绿鸽鸽添加相关功能~[CQ:face,id=111]", true)); // 没听懂
	}

}

namespace disabled_functions { // 行 为 艺 术

}

using namespace checker_and_handler;
using namespace enabled_functions;

CQ_INIT {
	on_enable([]{
		logging::info("启用", "Bot已启用");

		gen = mt19937(time(0));
		collections::init();
		enabled_functions::init();

		for (auto group_id : NOTICE_GROUPS)
			message_tools::send_group_message(group_id, "Bot已启用，OEIS功能已上线\n珍爱Bot生命，请勿刷屏~[CQ:face,id=111][CQ:face,id=111]");
	});

	on_group_message([](const GroupMessageEvent &e){
		if (ENABLED_GROUPS.count(e.group_id) == 0) return;

		if (backend_functions::eval->check_group(e.group_id)) // 如果开启了群聊转发功能则转发消息
			::send_message(producer_id, backend_functions::eval->get_name(e.user_id) + "：" + e.message);

		for (auto o : functions) {
			if (o.first->check(e)) {
				auto s = o.second->reply(e);
				::send_group_message(e.group_id, s);

				if (backend_functions::eval->check_group(e.group_id)) // 如果开启了群聊转发功能则转发消息
					::send_message(producer_id, backend_functions::eval->get_name(self_id) + "：" + s);
				
				if (o.first->is_final()) {
					e.block();
					return;
				}
			}
		}
	});

	on_private_message([](const PrivateMessageEvent &e) {
		if (e.user_id == producer_id) {
			message_tools::send_message(producer_id, backend_functions::eval->reply(e.message));
		}
		else {
			if (backend_functions::eval->check_private(e.user_id))
				message_tools::send_message(producer_id, e.message); // 如果启用了私聊转发则转发消息
			else message_tools::send_message(e.user_id, private_functions::eval->reply(e.message));
		}
		e.block();
	});
}
