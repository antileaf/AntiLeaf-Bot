#ifndef PRIVATE_HPP
#define PRIVATE_HPP

#include <string>

#include <cqcppsdk/cqcppsdk.h>

#include "checker_and_handler.hpp"
#include "string_tools.hpp"

using namespace message_tools;
using namespace cq;

namespace checker_and_handler {

	using namespace std;
	using namespace string_tools;

	class private_checker : virtual public checker {

	public:
		private_checker() : checker() {}

		virtual ~private_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class private_multiple_checker : virtual public private_checker, public multiple_checker { // 复合检查

	protected:
		vector<private_checker*> v;

	public:
		private_multiple_checker(vector<private_checker*> &v) : private_checker(), multiple_checker(), v(v) {}

		virtual ~private_multiple_checker() = 0 {
			for (auto o : v)
				delete o;
		}

		virtual bool check (const MessageEvent&, const Target &u) = 0;
	};

	class private_and_checker final : public private_multiple_checker, public and_checker { // 与

	public:
		private_and_checker(vector<private_checker*> &v) : private_multiple_checker(v), and_checker() {}

		~private_and_checker() = default;

		bool check(const MessageEvent& e, const Target &u) {
			bool ans = true;

			for (auto o : v)
				ans &= o->check(e, u);

			return ans;
		}
	};

	class private_or_checker final : public private_multiple_checker, public or_checker { // 或

	public:
		private_or_checker(vector<private_checker*>& v) : private_multiple_checker(v), or_checker() {}

		~private_or_checker() = default;

		bool check(const MessageEvent& e, const Target& u) {
			bool ans = false;

			for (auto o : v)
				ans &= o->check(e, u);

			return ans;
		}

	};

	class private_not_checker final : public private_checker, public not_checker { // 非

		private_checker* o;

	public:
		private_not_checker(private_checker* o) : private_checker(), not_checker(), o(o) {}

		~private_not_checker() {
			delete o;
		}

		bool check(const MessageEvent &e, const Target &u) {
			return !o->check(e, u);
		}
	};

	class private_regex_checker : virtual public private_checker, public regex_checker { // 正则表达式

	protected:
		// regex r;

	public:
		private_regex_checker(string t) : private_checker(), regex_checker(t) {}

		virtual ~private_regex_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class private_complete_match final : public private_regex_checker, public complete_match { // 正则表达式（完全匹配）

	public:
		private_complete_match(string t) : private_regex_checker(t), complete_match(t) {} // 注意这里已经加了指令前缀

		// complete_match(Message m) : regex_checker((string)m) {} // 这里也加了指令前缀

		~private_complete_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_match(e.message, regex_checker::r);
		}
	};

	class private_presuf_match final : public private_regex_checker, public presuf_match { // 前后缀匹配

	protected:
		// string pre, suf;

	public:
		private_presuf_match(string pre, string suf = "") : private_regex_checker(pre + "[\\s\\S]*" + suf), presuf_match(pre, suf) {}

		// presuf_match(Message pre, Message suf = "") : regex_checker((string)pre + "[\\s\\S]*" + (string)suf) {}

		~private_presuf_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_match(e.message, regex_checker::r);
		}
	};

	class private_substr_match final : public private_regex_checker, public substr_match { // 子串匹配

	public:
		private_substr_match(string t) : private_regex_checker(t), substr_match(t) {}

		~private_substr_match() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return regex_search(e.message, regex_checker::r);
		}
	};

	class private_custom_checker : virtual public private_checker, public custom_checker { // 自定义检查器

	public:
		private_custom_checker() = default;

		virtual ~private_custom_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class private_user_id_check final : public private_custom_checker, public user_id_check { // 检查是否由特定id发送

	protected:
		// int64_t user_id;

	public:
		private_user_id_check(int64_t user_id) : private_custom_checker(), user_id_check(user_id) {}

		~private_user_id_check() = default;

		bool check(const MessageEvent &e, const Target &u) {
			return u.user_id == user_id;
		}
	};

	/*
	class always_check final : public custom_checker {

		bool value;

	public:
		always_check(bool value) : value(value) {}

		~always_check() = default;

		bool check(const MessageEvent &e) {
			if (regex_match(e.message, regex("[\\S\\s]*")))
				return value;
			return false;
		}

	};
	*/

	// ------------------------------------------------------
	// --------------------- handler ------------------------
	// ------------------------------------------------------

	class private_handler : virtual public handler {

	public:
		private_handler() = default;

		virtual ~private_handler() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0; // 重要！不再支持多条消息回复，可通过调用多个handler完成
	};

	class private_multiple_reply final : public private_handler, public multiple_reply { // 同时对一条消息执行多个回复，分行输出

		vector<private_handler*> v;

	public:
		private_multiple_reply(vector<private_handler*> v) : private_handler(), v(v) {}

		~private_multiple_reply() = default;

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

	class private_orderly_reply final : public private_handler, public orderly_reply { // 对一条消息依次（递归）执行多个回复
		vector<private_handler*> v;

	public:
		private_orderly_reply(vector<private_handler*>& v) : private_handler(), v(v) {}

		~private_orderly_reply() = default;

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

	class private_fixed_reply final : public private_handler, public fixed_reply { // 固定回复

	protected:
		// string s;

	public:
		private_fixed_reply(string s) : private_handler(), fixed_reply(s) {}

		~private_fixed_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			if (at)
				return ::at(u.user_id.value()) + " " + s;
			else return s;
		}
	};

	class private_random_reply final : public private_handler, public random_reply {

	protected:
		// vector<string> v;

	public:
		private_random_reply(vector<string>& v) : private_handler(), random_reply(v) {}

		~private_random_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			string t = v[gen() % v.size()];
			if (at)
				t = ::at(u.user_id.value()) + " " + t;
			return t;
		}
	};

	class private_single_para_reply final : public private_handler, public single_para_reply { // 单参数回复

	protected:
		// string pre, suf, p, s;

	public:
		private_single_para_reply(string pre, string suf, string p, string s) :
			private_handler(), single_para_reply(pre, suf, p, s) {}

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

	class private_custom_reply : virtual public private_handler, public custom_reply { // 自定义回复

	public:
		private_custom_reply() : private_handler(), custom_reply() {}

		virtual ~private_custom_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};

	class private_url_reply final : public private_custom_reply, public url_reply { // 搜索引擎回复

	protected:
		// string pre, suf;

	public:
		private_url_reply(string pre, string suf) : private_custom_reply(), url_reply(pre, suf) {}

		~private_url_reply() = default;

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

	class private_report_reply final : public private_custom_reply, public report_reply {

	public:
		private_report_reply() : private_custom_reply(), report_reply() {}

		~private_report_reply() = default;

		string reply(const MessageEvent &e, const Target &u) {
			message_tools::send_message(producer_id, "来自" + to_string(u.user_id.value()) + "的反馈：" + e.message);
			return "已成功反馈，感谢支持！";
		}
	};
}

using namespace checker_and_handler;
namespace private_functions { // 私聊功能

	class evaluator {

	public:
		evaluator() = default;

		string reply(string s) {
			return ""; // 复读你妹妹
		}
	} *eval = new evaluator;
}

namespace backend_functions { // 给自己用的后台功能

	class evaluator {

		enum class backend_status { DISABLED, PRIVATE, GROUP } status;
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

#endif