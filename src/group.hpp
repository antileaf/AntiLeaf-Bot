<<<<<<< HEAD
#ifndef GROUP_HPP
#define GROUP_HPP

#include <string>

#include <cqcppsdk/cqcppsdk.h>

#include "checker_and_handler.hpp"
#include "string_tools.hpp"

namespace checker_and_handler {

	using namespace std;
	using namespace string_tools;

	class group_checker : public checker {

	public:
		group_checker() : checker() {}

		virtual ~group_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};
	
	class group_multiple_checker : public group_checker, public multiple_checker { // 复合检查

	protected:
		vector<group_checker*> v;

	public:
		group_multiple_checker(vector<group_checker*> &v) : group_checker(), multiple_checker(), v(v) {}

		virtual ~group_multiple_checker() = 0 {
			for (auto o : v)
				delete o;
		}

		virtual bool check (const MessageEvent&, const Target&) = 0;
	};

	class group_and_checker final : public group_multiple_checker, public and_checker { // 与

	public:
		group_and_checker(vector<group_checker*> &v) : group_multiple_checker(v), and_checker() {}

		~group_and_checker() = default;

		bool check (const MessageEvent &e, const Target &u) {
			bool ans = true;

			for (auto o : v)
				ans &= o->check(e, u);

			return ans;
		}
	};

	class group_or_checker final : public group_multiple_checker, public or_checker { // 或

	public:
		group_or_checker(vector<group_checker*> &v) : group_multiple_checker(v), or_checker() {}

		~group_or_checker() = default;

		bool check(const MessageEvent& e, const Target &u) {
			bool ans = false;

			for (auto o : v)
				ans &= o->check(e, u);

			return ans;
		}

	};

	class group_not_checker final : public group_checker, public not_checker { // 非

		group_checker* o;

	public:
		group_not_checker(group_checker* o) : group_checker(), not_checker(), o(o) {}

		~group_not_checker() {
			delete o;
		}

		bool check(const MessageEvent& e, const Target &u) {
			return !o->check(e, u);
		}
	};

	class group_regex_checker : public group_checker, public regex_checker { // 正则表达式

	protected:
		// regex r;

	public:
		group_regex_checker(string t) : group_checker(), regex_checker(t) {}

		virtual ~group_regex_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class group_complete_match final : public group_regex_checker, public complete_match { // 正则表达式（完全匹配）

	public:
		group_complete_match(string t) : group_regex_checker(PREF + t), complete_match(PREF + t){} // 注意这里已经加了指令前缀

		// complete_match(Message m) : regex_checker(PREF + (string)m) {} // 这里也加了指令前缀

		~group_complete_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_match(e.message, regex_checker::r);
		}
	};

	class group_presuf_match final : public group_regex_checker, public presuf_match { // 前后缀匹配

	protected:
		// string pre, suf;

	public:
		group_presuf_match(string pre, string suf = "") : group_regex_checker(PREF + pre + "[\\s\\S]*" + suf), presuf_match(PREF + pre, suf) {}

		// presuf_match(Message pre, Message suf = "") : regex_checker(PREF + (string)pre + "[\\s\\S]*" + (string)suf) {}

		~group_presuf_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_match(e.message, regex_checker::r);
		}
	};

	class group_substr_match final : public group_regex_checker, public substr_match { // 子串匹配

	public:
		group_substr_match(string t) : group_regex_checker(t), substr_match(t) {}

		~group_substr_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_search(e.message, regex_checker::r);
		}
	};

	class group_custom_checker : public group_checker, public custom_checker { // 自定义检查器

	public:
		group_custom_checker() = default;

		virtual ~group_custom_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class group_repeater_check final : public group_custom_checker {

		static const int K = 3;

		std::map<int64_t, pair<string, int>> o;

	public:
		group_repeater_check() : group_custom_checker() {}

		~group_repeater_check() = default;

		bool check(const MessageEvent &e, const Target &u) {
			auto &last = o[u.group_id.value()];
			if (e.message == last.first) {
				if (++last.second == K - 1) {
					last.first = "";
					return true;
				}
				return false;
			}
			else {
				last.first = e.message;
				last.second = 0;
			}

			return false;
		}
	};

	class group_user_id_check final : public group_custom_checker, public user_id_check { // 检查是否由特定id发送

	protected:
		// int64_t user_id;

	public:
		group_user_id_check(int64_t user_id) : group_custom_checker(), user_id_check(user_id) {}

		~group_user_id_check() = default;

		bool check(const MessageEvent& e, const Target &u) {
			return u.user_id.value() == user_id;
		}
	};

	class group_id_check final : public group_custom_checker {
	
		int64_t group_id;

	public:
		group_id_check(int64_t group_id) : group_custom_checker(), group_id(group_id) {}

		~group_id_check() = default;

		bool ccheck(const MessageEvent &e, const Target &u) {
			return u.group_id.value() == group_id;
		}
	};

	/*
	class always_check final : public custom_checker {

		bool value;

	public:
		always_check(bool value) : value(value) {}

		~always_check() = default;

		bool check(const MessageEvent &e) {
			if (regex_match(e.message, regex(PREF + "[\\S\\s]*")))
				return value;
			return false;
		}

	};
	*/

	// ------------------------------------------------------
	// --------------------- handler ------------------------
	// ------------------------------------------------------

	class group_handler : public handler {

	protected:
		bool at = false;

	public:
		group_handler(bool at = false) : at(at) {}

		virtual ~group_handler() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0; // 重要！不再支持多条消息回复，可通过调用多个handler完成
	};

	class group_multiple_reply final : public group_handler, public multiple_reply { // 同时对一条消息执行多个回复，分行输出

		vector<group_handler*> v;

	public:
		group_multiple_reply(vector<group_handler*> v, bool at = false) : group_handler(at), multiple_reply(), v(v) {}

		~group_multiple_reply() = default;

		string reply(const MessageEvent& e, const Target &u) {
			string t;
			for (auto o : v) {
				if (o != v.front())
					t += "\n";
				t += o->reply(e, u);
			}

			if (at)
				t = ::at(u.user_id.value()) + t;
			return t;
		}
	};

	class group_orderly_reply final : public group_handler, public orderly_reply { // 对一条消息依次（递归）执行多个回复
		vector<group_handler*> v;

	public:
		group_orderly_reply(vector<group_handler*> &v, bool at = false) : group_handler(at), orderly_reply(), v(v) {}

		~group_orderly_reply() = default;

		string reply(const MessageEvent &ee, const Target &u) {

			auto e = ee;
			// logging::info("debug", e.message);
			for (auto o : v) {
				e.message = o->reply(e, u);
				// logging::info("debug", e.message);
			}
			if (at)
				e.message = ::at(u.user_id.value()) + " " + e.message;

			return e.message;
		}
	};

	class group_fixed_reply final : public group_handler, public fixed_reply { // 固定回复

	protected:
		// string s;

	public:
		group_fixed_reply(string s, bool at = false) : group_handler(at), fixed_reply(s) {}

		~group_fixed_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			if (at)
				return ::at(u.user_id.value()) + " " + s;
			else return s;
		}
	};

	class group_random_reply final : public group_handler, public random_reply {

	protected:
		// vector<string> v;

	public:
		group_random_reply(vector<string> &v, bool at = false) : group_handler(at), random_reply(v) {}

		~group_random_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			string t = v[gen() % v.size()];
			if (at)
				t = ::at(u.user_id.value()) + " " + t;
			return t;
		}
	};

	class group_single_para_reply final : public group_handler, public single_para_reply { // 单参数回复

	protected:
		// string pre, suf, p, s;

	public:
		group_single_para_reply(string pre, string suf, string p, string s, bool at = false) :
			group_handler(at), single_para_reply(pre, suf, p, s) {}

		~group_single_para_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
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

	class group_custom_reply : virtual public group_handler, public custom_reply { // 自定义回复

	public:
		group_custom_reply(bool at = false) : group_handler(at), custom_reply() {}

		virtual ~group_custom_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};

	class group_repeater_reply final : public group_handler, public custom_reply { // 我是一只复读机

	public:
		group_repeater_reply() : group_handler(false), custom_reply() {}

		~group_repeater_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			return e.message;
		}
	};

	class group_url_reply final : public group_custom_reply, public url_reply { // 搜索引擎回复

	protected:
		// string pre, suf;

	public:
		group_url_reply(string pre, string suf, bool at = false) : group_custom_reply(at), url_reply(pre, suf) {}

		~group_url_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
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

	class group_report_reply final : public group_custom_reply, public report_reply {

	public:
		group_report_reply(bool at = false) : group_custom_reply(at), report_reply() {}

		~group_report_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			message_tools::send_message(producer_id, "来自" + to_string(u.user_id.value()) + "在群聊" + to_string(u.group_id.value()) + "的反馈：" + e.message);
			return "已成功反馈，感谢支持！";
		}
	};
}

using namespace checker_and_handler;

=======
#ifndef GROUP_HPP
#define GROUP_HPP

#include <string>

#include <cqcppsdk/cqcppsdk.h>

#include "checker_and_handler.hpp"
#include "string_tools.hpp"

namespace checker_and_handler {

	using namespace std;
	using namespace string_tools;

	class group_checker : public checker {

	public:
		group_checker() : checker() {}

		virtual ~group_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};
	
	class group_multiple_checker : public group_checker, public multiple_checker { // 复合检查

	protected:
		vector<group_checker*> v;

	public:
		group_multiple_checker(vector<group_checker*> &v) : group_checker(), multiple_checker(), v(v) {}

		virtual ~group_multiple_checker() = 0 {
			for (auto o : v)
				delete o;
		}

		virtual bool check (const MessageEvent&, const Target&) = 0;
	};

	class group_and_checker final : public group_multiple_checker, public and_checker { // 与

	public:
		group_and_checker(vector<group_checker*> &v) : group_multiple_checker(v), and_checker() {}

		~group_and_checker() = default;

		bool check (const MessageEvent &e, const Target &u) {
			bool ans = true;

			for (auto o : v)
				ans &= o->check(e, u);

			return ans;
		}
	};

	class group_or_checker final : public group_multiple_checker, public or_checker { // 或

	public:
		group_or_checker(vector<group_checker*> &v) : group_multiple_checker(v), or_checker() {}

		~group_or_checker() = default;

		bool check(const MessageEvent& e, const Target &u) {
			bool ans = false;

			for (auto o : v)
				ans &= o->check(e, u);

			return ans;
		}

	};

	class group_not_checker final : public group_checker, public not_checker { // 非

		group_checker* o;

	public:
		group_not_checker(group_checker* o) : group_checker(), not_checker(), o(o) {}

		~group_not_checker() {
			delete o;
		}

		bool check(const MessageEvent& e, const Target &u) {
			return !o->check(e, u);
		}
	};

	class group_regex_checker : public group_checker, public regex_checker { // 正则表达式

	protected:
		// regex r;

	public:
		group_regex_checker(string t) : group_checker(), regex_checker(t) {}

		virtual ~group_regex_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class group_complete_match final : public group_regex_checker, public complete_match { // 正则表达式（完全匹配）

	public:
		group_complete_match(string t) : group_regex_checker(PREF + t), complete_match(PREF + t){} // 注意这里已经加了指令前缀

		// complete_match(Message m) : regex_checker(PREF + (string)m) {} // 这里也加了指令前缀

		~group_complete_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_match(e.message, regex_checker::r);
		}
	};

	class group_presuf_match final : public group_regex_checker, public presuf_match { // 前后缀匹配

	protected:
		// string pre, suf;

	public:
		group_presuf_match(string pre, string suf = "") : group_regex_checker(PREF + pre + "[\\s\\S]*" + suf), presuf_match(PREF + pre, suf) {}

		// presuf_match(Message pre, Message suf = "") : regex_checker(PREF + (string)pre + "[\\s\\S]*" + (string)suf) {}

		~group_presuf_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_match(e.message, regex_checker::r);
		}
	};

	class group_substr_match final : public group_regex_checker, public substr_match { // 子串匹配

	public:
		group_substr_match(string t) : group_regex_checker(t), substr_match(t) {}

		~group_substr_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_search(e.message, regex_checker::r);
		}
	};

	class group_custom_checker : public group_checker, public custom_checker { // 自定义检查器

	public:
		group_custom_checker() = default;

		virtual ~group_custom_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class group_repeater_check final : public group_custom_checker {

		static const int K = 3;

		std::map<int64_t, pair<string, int>> o;

	public:
		group_repeater_check() : group_custom_checker() {}

		~group_repeater_check() = default;

		bool check(const MessageEvent &e, const Target &u) {
			auto &last = o[u.group_id.value()];
			if (e.message == last.first) {
				if (++last.second == K - 1) {
					last.first = "";
					return true;
				}
				return false;
			}
			else {
				last.first = e.message;
				last.second = 0;
			}

			return false;
		}
	};

	class group_user_id_check final : public group_custom_checker, public user_id_check { // 检查是否由特定id发送

	protected:
		// int64_t user_id;

	public:
		group_user_id_check(int64_t user_id) : group_custom_checker(), user_id_check(user_id) {}

		~group_user_id_check() = default;

		bool check(const MessageEvent& e, const Target &u) {
			return u.user_id.value() == user_id;
		}
	};

	class group_id_check final : public group_custom_checker {
	
		int64_t group_id;

	public:
		group_id_check(int64_t group_id) : group_custom_checker(), group_id(group_id) {}

		~group_id_check() = default;

		bool ccheck(const MessageEvent &e, const Target &u) {
			return u.group_id.value() == group_id;
		}
	};

	/*
	class always_check final : public custom_checker {

		bool value;

	public:
		always_check(bool value) : value(value) {}

		~always_check() = default;

		bool check(const MessageEvent &e) {
			if (regex_match(e.message, regex(PREF + "[\\S\\s]*")))
				return value;
			return false;
		}

	};
	*/

	// ------------------------------------------------------
	// --------------------- handler ------------------------
	// ------------------------------------------------------

	class group_handler : public handler {

	protected:
		bool at = false;

	public:
		group_handler(bool at = false) : at(at) {}

		virtual ~group_handler() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0; // 重要！不再支持多条消息回复，可通过调用多个handler完成
	};

	class group_multiple_reply final : public group_handler, public multiple_reply { // 同时对一条消息执行多个回复，分行输出

		vector<group_handler*> v;

	public:
		group_multiple_reply(vector<group_handler*> v, bool at = false) : group_handler(at), multiple_reply(), v(v) {}

		~group_multiple_reply() = default;

		string reply(const MessageEvent& e, const Target &u) {
			string t;
			for (auto o : v) {
				if (o != v.front())
					t += "\n";
				t += o->reply(e, u);
			}

			if (at)
				t = ::at(u.user_id.value()) + t;
			return t;
		}
	};

	class group_orderly_reply final : public group_handler, public orderly_reply { // 对一条消息依次（递归）执行多个回复
		vector<group_handler*> v;

	public:
		group_orderly_reply(vector<group_handler*> &v, bool at = false) : group_handler(at), orderly_reply(), v(v) {}

		~group_orderly_reply() = default;

		string reply(const MessageEvent &ee, const Target &u) {

			auto e = ee;
			// logging::info("debug", e.message);
			for (auto o : v) {
				e.message = o->reply(e, u);
				// logging::info("debug", e.message);
			}
			if (at)
				e.message = ::at(u.user_id.value()) + " " + e.message;

			return e.message;
		}
	};

	class group_fixed_reply final : public group_handler, public fixed_reply { // 固定回复

	protected:
		// string s;

	public:
		group_fixed_reply(string s, bool at = false) : group_handler(at), fixed_reply(s) {}

		~group_fixed_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			if (at)
				return ::at(u.user_id.value()) + " " + s;
			else return s;
		}
	};

	class group_random_reply final : public group_handler, public random_reply {

	protected:
		// vector<string> v;

	public:
		group_random_reply(vector<string> &v, bool at = false) : group_handler(at), random_reply(v) {}

		~group_random_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			string t = v[gen() % v.size()];
			if (at)
				t = ::at(u.user_id.value()) + " " + t;
			return t;
		}
	};

	class group_single_para_reply final : public group_handler, public single_para_reply { // 单参数回复

	protected:
		// string pre, suf, p, s;

	public:
		group_single_para_reply(string pre, string suf, string p, string s, bool at = false) :
			group_handler(at), single_para_reply(pre, suf, p, s) {}

		~group_single_para_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
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

	class group_custom_reply : virtual public group_handler, public custom_reply { // 自定义回复

	public:
		group_custom_reply(bool at = false) : group_handler(at), custom_reply() {}

		virtual ~group_custom_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};

	class group_repeater_reply final : public group_handler, public custom_reply { // 我是一只复读机

	public:
		group_repeater_reply() : group_handler(false), custom_reply() {}

		~group_repeater_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			return e.message;
		}
	};

	class group_url_reply final : public group_custom_reply, public url_reply { // 搜索引擎回复

	protected:
		// string pre, suf;

	public:
		group_url_reply(string pre, string suf, bool at = false) : group_custom_reply(at), url_reply(pre, suf) {}

		~group_url_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
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

	class group_report_reply final : public group_custom_reply, public report_reply {

	public:
		group_report_reply(bool at = false) : group_custom_reply(at), report_reply() {}

		~group_report_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			message_tools::send_message(producer_id, "来自" + to_string(u.user_id.value()) + "在群聊" + to_string(u.group_id.value()) + "的反馈：" + e.message);
			return "已成功反馈，感谢支持！";
		}
	};
}

using namespace checker_and_handler;

>>>>>>> e1fa7543481f021ac86a6771f02ffdd12868b3a0
#endif