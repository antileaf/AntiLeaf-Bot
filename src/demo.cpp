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

const string PREF = "% *";
mt19937 gen;
const int64_t self_id = 625938638;
const string at_self = "[CQ:at,qq=" + to_string(self_id) + "]";
const set<int64_t> ENABLED_GROUPS = {441254450, 228725393, 780135491, 463471261, 301538258};
const set<int64_t> NOTICE_GROUPS = {441254450, 228725393, 780135491};
const set<int64_t> ENABLED_USERS = {1094054222};
string at(int64_t user_id){return "[CQ:at,qq=" + to_string(user_id) + "]";}
#define AT ((at ? ::at(e.user_id) : "") + " ")

namespace string_tools {
	
	string original(string s) {return s;}


	string urlencode(const string &str_source){
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

namespace checker_and_handler {

	using namespace string_tools;

	class checker {
	public:
		checker() = default;

		virtual ~checker() = default;

		virtual bool check (const GroupMessageEvent&) = 0;
	};

	class multiple_checker : public checker {

	protected:
		vector<checker*> v;

    public:
		multiple_checker(vector<checker*> &v) : v(v) {}

		virtual ~multiple_checker() {
			for (auto o : v)
				delete o;
		}

		virtual bool check (const GroupMessageEvent&) = 0;
	};

	class and_checker final : public multiple_checker {

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

	class or_checker final : public multiple_checker {
		
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

	class regex_checker : public checker {

	protected:
		regex r;

	public:
		regex_checker(string t) : r(t) {}

		virtual ~regex_checker() = default;

		virtual bool check(const GroupMessageEvent&) = 0;
	};

	class complete_match final : public regex_checker {

	public:
		complete_match(string t) : regex_checker(PREF + t) {} // !!!!!!!!!!!!!!!!!

		~complete_match() = default;

		bool check(const GroupMessageEvent &e) {
			return regex_match(e.message, r);
		}
	};

	class presuf_match final : public regex_checker {
		
	public:
		presuf_match(string pre, string suf = "") :regex_checker(PREF + pre + "[\\s\\S]*" + suf) {}

		~presuf_match() = default;

		bool check(const GroupMessageEvent &e) {
			return regex_match(e.message, r);
		}
	};

	class substr_match final : public regex_checker {

	public:
		substr_match(string t) : regex_checker(t) {}

		~substr_match() = default;

		bool check(const GroupMessageEvent &e) {
			return regex_search(e.message, r);
		}
	};

	class custom_checker : public checker {

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

	class id_check final : public custom_checker {
	
		int64_t id;

	public:
		id_check(int64_t id) : id(id) {}

		~id_check() = default;

		bool check(const GroupMessageEvent &e) {
			return e.user_id == id;
		}
	};

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

	// ------------------------------------------------------

	class handler {

	protected:
		bool at = false;

	public:
		handler(bool at = false) : at(at) {}

		virtual ~handler() = default;

		virtual vector<string> reply(const GroupMessageEvent &) = 0;
	};

	class multiple_reply final : public handler { // 同时对一条消息执行多个回复，分条输出

		vector<handler*> v;

	public:
		multiple_reply(vector<handler*> &v, bool at = false) : handler(at), v(v) {}

		~multiple_reply() = default;

		vector<string> reply(const GroupMessageEvent &e) {

			vector<string> u;

			for (auto o : v)
				for (auto s : o->reply(e))
					u.push_back(s);

			if (at && !u.empty())
				u[0] = ::at(e.user_id) + u[0];

			return u;
		}
	};

	class orderly_reply final : public handler { // 对一条消息依次（递归）执行多个回复，仅限一句话
		vector<handler*> v;

	public:
		orderly_reply(vector<handler*> &v, bool at = false) : handler(at), v(v) {}

		~orderly_reply() = default;

		vector<string> reply(const GroupMessageEvent &ee) {
			auto e = ee;
			for (auto o : v)
				e.message = o->reply(e)[0];
			if (at)
				e.message = AT + e.message;
			return vector<string>({e.message});
		}
	};

	class fixed_reply final : public handler {

		string s;

	public:
		fixed_reply(string s, bool at = false) : handler(at), s(s) {}

		~fixed_reply() = default;

		vector<string> reply(const GroupMessageEvent &e) {
			return vector<string>({AT + s});
		}
	};

	class random_reply final : public handler {

		vector<string> v;

	public:
		random_reply(vector<string> &v, bool at = false) : handler(at), v(v) {}

		~random_reply() = default;

		vector<string> reply(const GroupMessageEvent &e) {
			return vector<string>({AT + v[gen()%v.size()]});
		}
	};

	class single_para_reply final : public handler { // 目前只能回复一句话
		
		string pre, suf, p, s;

	public:
		single_para_reply(string pre, string suf, string p, string s) :
			pre(pre), suf(suf), p(p), s(s) {}

		vector<string> reply(const GroupMessageEvent &e) {
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

			return vector<string>({p + t + s});
		}
	};

	class custom_reply : public handler {
		
	public:
		custom_reply(bool at = false) : handler(at) {}

		virtual ~custom_reply() = default;

		virtual vector<string> reply(const GroupMessageEvent&) = 0;
	};

	class repeater_reply final : public custom_reply {
		
	public:
		repeater_reply() : custom_reply(false) {}

		~repeater_reply() = default;

		vector<string> reply(const GroupMessageEvent &e) {
			return vector<string>({e.message});
		}
	};

	class baidu_reply final : public custom_reply {
		
	public:
		baidu_reply(bool at = false) : custom_reply(at) {}

		~baidu_reply() = default;


		vector<string> reply(const GroupMessageEvent &e) {
			string s = e.message;
			int i;
			while ((i = s.find(" ")!=string::npos))
				s.replace (i, 1, "%20");
			return vector<string>({s});
		}
	};

	class python_reply final : public custom_reply {
		
	public:
		python_reply(bool at = false) : custom_reply(at) {}

		~python_reply() = default;

		vector<string> reply(const GroupMessageEvent &e) {
			return vector<string>({"珍爱绿鸽鸽的电脑，不要再拿我开涮了！QAQ"});
			/*
			system(("python \"C:\\Users\\AntiLeaf\\Desktop\\bot\\AntiLeaf-Bot\\src\\eval.py\" " + e.message).c_str());
			ifstream f("C:\\Users\\AntiLeaf\\Desktop\\bot\\AntiLeaf-Bot\\src\\temp.txt");
			ostringstream os;
			os << f.rdbuf();
			logging::debug("os", os.str());
			return vector<string>({os.str()}); // TODO : 这里写的好丑，有空改一下
			*/
		}
	};

	class url_reply final : public custom_reply {
		
		string pre, suf;

	public:
		url_reply(string pre, string suf, bool at = false) : custom_reply(at), pre(pre), suf(suf) {}

		~url_reply() = default;

		vector<string> reply(const GroupMessageEvent &e) {
			using string_tools::urlencode;

			string s = e.message;
			int i = 0;
			while (i < (int)s.size() && s[i] == ' ')
				i++;
			if (i == (int)s.size())
				return vector<string>({"不要试图搜索空的东西哦~"});
			s = s.substr(i);

			i = 0;
			while (s[(int)s.size() - i - 1] == ' ')
				i++;
			s = s.substr(0, (int)s.size() - i);

			// assert(!at);
			return vector<string>({pre + urlencode(e.message) + suf});
		}
	};

} // namespace checker_and_handler

namespace p2p { // 私聊功能

}

void send_message(int64_t user_id, vector<string> v) {
	try {
		for (auto &s : v)
			send_message(Target::user(user_id), s);
	}
	catch (ApiError&) {}
	catch (...) {
		logging::error("错误", "未知错误");
	}
}

void send_message(int64_t user_id, string s) {
	send_message(user_id, vector<string>({s}));
}

void send_group_message(int64_t group_id, vector<string> v) {
	try {
		for (auto &s : v)
			cq::send_group_message(group_id, s);
	}
	catch (ApiError&) {}
	catch (...) {
		logging::error("错误", "未知错误");
	}
}

void send_group_message(int64_t group_id, string s) {
	send_group_message(group_id, vector<string>({s}));
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
	};

	vector<pair<vector<string>, vector<string>>> conver = { // 对话
		{{"爬", "给爷爬", "爪巴", "给爷爪巴"}, {"你怎么能让可爱的我爬呢，你坏坏QAQ"}},
		{{"AntiLeaf", "antileaf", "绿鸽鸽", "绿哥哥"}, {"绿鸽鸽好帅，最喜欢他了！QwQ"}},
		{{"呐呐", "呐呐呐", "呐 呐", "呐 呐 呐"}, {"自 动 声 呐 系 统", "巧了我也是二次元QwQ"}},
	};

	vector<pair<pair<string, string>, pair<string, string>>> single_para = { // 单参数命令
		{{"echo", ""}, {"", ""}},
		{{"输出", ""}, {"", ""}}
	};

	vector<pair<vector<string>,vector<string>>> dear = { // 给给
		{{"亲亲", "\\[CQ:face,id=109\\]"},{"[CQ:face,id=109][CQ:face,id=109][CQ:face,id=109]"}},
		{{"给给"},{"绿鸽鸽好帅，我要和绿鸽鸽给给[CQ:face,id=109][CQ:face,id=109]"}},
		{{"说几句好听的", "说两句好听的", "来两句好听的", "来几句好听的", "来句好听的", "说句好听的"}, {"绿鸽鸽最帅了[CQ:face,id=109]", "我最喜欢绿鸽鸽了[CQ:face,id=109]", "绿鸽鸽帅帅！[CQ:face,id=109]"}},
		{{"卖个萌", "来卖个萌", "卖萌"}, {"[CQ:face,id=111][CQ:face,id=111][CQ:face,id=111]", "QwQ", "别嘛~~人家要害羞了啦~~"}},
		{{"早安", "说早安", "早上好"}, {"早安~[CQ:face,id=109]", "早上好~新的一天也要精神满满哦！"}},
		{{"上午好"}, {"上午好~事不宜迟，开始忙碌的一天吧！"}},
		{{"午安", "说午安", "中午好"}, {"午安~[CQ:face,id=109]", "中午好~不要忘记睡午觉喔~"}},
		{{"下午好"}, {"下午好~吃点什么零食呢？"}},
		{{"晚上好"}, {"晚上好~晚上干活虽好，也要注意少熬夜呢~"}},
		{{"晚安", "说晚安"}, {"晚安，好好休息一下吧~[CQ:face,id=109]"}},
	};

#define add_function(a, b) functions.push_back(make_pair((new a), (new b)))
#define add_fixed(a, b, c) add_function(complete_match(a), fixed_reply(b, c))
#define and_check(a, b) and_checker(vector<checker*>({new a, new b}))
#define order_reply(a, b, c) orderly_reply(vector<handler*>({new a, new b}), c)

	void init() {

		for (auto s : {"help", "帮助"})
		add_function(complete_match(s), fixed_reply("\nBot v0.4  By AntiLeaf\n功能\t\t\t使用方法\n----------------------------\n复读\t\t\t你最好不要知道\n\
网站查询\t\t%网站+你想查询的网站\n直播查询\t\t%直播+你想查询的群友常用id\n随机查房\t\t%随机查房\n（网站列表与直播请联系绿鸽鸽提供）\n\
百度/谷歌搜索\t%百度/谷歌一下+你想搜索的东西\nPython\t\t\t%python/py+你想执行的指令\n输出（请勿刷屏）%echo/%输出+你想输出的东西\n给给\t\t\t只有绿鸽鸽才能使用\n帮助\t\t\t%help/%帮助\n\n\
最近更新：换了个头像，添加直播查询与和绿鸽鸽给给功能\n下一步计划：完善给给语录，添加百度/谷歌一下、OEIS查询、Wolfram Alpha查询功能", true));

		for (auto p : websites)
			for (auto o : p.first)
				add_function(complete_match("网站 *" + o), fixed_reply(p.first[0] + "： " + p.second, true)); // 网站查询

		string s = "支持的网站列表：";
		for (auto p : websites)
			s += "\n" + p.first[0];
		add_function(and_check(id_check(1094054222), complete_match("网站列表")), fixed_reply(s, false)); // 网站列表

		for (auto p : live)
			for (auto o : p.first)
				add_fixed("直播 *" + o, p.first[0] + "的直播间： " + p.second, true); // 直播查询

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

		add_function(presuf_match("百度一下", ""), order_reply(single_para_reply("百度一下", "", "", ""), url_reply("www.baidu.com/s?wd=", ""), true));

		// for (auto s : {"Python", "python", "py", "Py"})
		// 	add_function(presuf_match(s, ""), order_reply(single_para_reply(s, "", "", ""), python_reply(true), true)); // Python

		for (auto p : dear)
			for (auto o : p.first)
				for (auto id : ENABLED_USERS)
					add_function(and_check(id_check(id), complete_match(o)), random_reply(p.second, true)); // 给给（仅限自己使用）
		for (auto p : dear)
			for (auto o : p.first)
				add_function(complete_match(o), fixed_reply("对不起哦，人家只愿意和绿鸽鸽给给呢~[CQ:face,id=111]", true)); // 给给（让群友吃桃子）

		add_function(repeater_check, repeater_reply); // 复读机
		
		// TODO : 添加更多功能

		add_function(presuf_match(""), fixed_reply("不好意思，人家现在还听不懂哦~[CQ:face,id=111]\n如有需要可以联系绿鸽鸽添加相关功能~[CQ:face,id=111]", true));
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
		enabled_functions::init();

		for (auto group_id : NOTICE_GROUPS)
			::send_group_message(group_id, "Bot已启用，随机查房功能已上线，Python功能已下线\n注意啦！现在指令的前缀是%（百分号），不要搞错哟~~\n珍爱Bot生命，请勿刷屏~[CQ:face,id=111][CQ:face,id=111]");
	});

	on_group_message([](const GroupMessageEvent& e){
		if (ENABLED_GROUPS.count(e.group_id) == 0) return;

		for (auto o : functions) {
			if (o.first->check(e)) {
				send_group_message(e.group_id, o.second->reply(e));

				e.block();
				return;
			}
		}
	});
}
