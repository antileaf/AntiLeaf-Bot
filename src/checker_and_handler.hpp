#ifndef CHECKER_AND_HANDLER_HPP
#define CHECKER_AND_HANDLER_HPP

#include <string>

#include <cqcppsdk/cqcppsdk.h>

#include "string_tools.hpp"

namespace checker_and_handler { // 定义checker/handler的框架，这些都是虚类

	using namespace std;
	using namespace string_tools;
	
	class checker {

	protected:
		bool is_finally = true; // 是否在识别为该类型指令后break，默认为真，目前尚未启用

	public:
		checker() = default;

		virtual ~checker() = 0 {};

		virtual bool check (const MessageEvent&, const Target&) = 0;

		bool is_final() const {
			return is_finally;
		}
	};

	class multiple_checker : public checker { // 复合检查

	protected:
		// vector<checker*> v;

	public:
		multiple_checker() : checker() {}

		virtual ~multiple_checker() = 0 {};

		virtual bool check (const MessageEvent&, const Target&) = 0;
	};

	class and_checker : public multiple_checker { // 与

	public:
		and_checker() : multiple_checker() {}

		virtual ~and_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class or_checker : public multiple_checker { // 或

	public:
		or_checker() : multiple_checker() {}

		virtual ~or_checker() = 0 {};

		virtual bool check(const MessageEvent &e, const Target &u) = 0;

	};

	class not_checker : public checker { // 非

		// checker* o;

	public:
		not_checker() : checker() {}

		virtual ~not_checker() = 0 {};
		
		virtual bool check(const MessageEvent& e, const Target &u) = 0;
	};

	class regex_checker : public checker { // 正则表达式

	protected:
		regex r;

	public:
		regex_checker(string t) : r(t) {}

		virtual ~regex_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class complete_match : public regex_checker { // 正则表达式（完全匹配）

	public:
		complete_match(string t) : regex_checker(t) {} // 私聊不需要前缀，所以没加

		// complete_match(Message m) : regex_checker((string)m) {} // 这里也加了指令前缀

		virtual ~complete_match() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class presuf_match : public regex_checker { // 前后缀匹配
		
	protected:
		string pre, suf;

	public:
		presuf_match(string pre, string suf = "") : regex_checker(pre + "[\\s\\S]*" + suf) {}

		// presuf_match(Message pre, Message suf = "") : regex_checker((string)pre + "[\\s\\S]*" + (string)suf) {}

		virtual ~presuf_match() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class substr_match : public regex_checker { // 子串匹配

	public:
		substr_match(string t) : regex_checker(t) {}

		// substr_match(Message m) : regex_checker((string)m) {}

		virtual ~substr_match() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	class custom_checker : public checker { // 自定义检查器

	public:
		custom_checker() = default;

		virtual ~custom_checker() = 0 {};

		virtual bool check(const MessageEvent&, const Target &) = 0;
	};

	class user_id_check : public custom_checker { // 检查是否由特定用户发送

	protected:
		int64_t user_id;

	public:
		user_id_check(int64_t user_id) : user_id(user_id) {}

		virtual ~user_id_check() = 0 {};

		virtual bool check(const MessageEvent&, const Target&) = 0;
	};

	/*
	class always_check : public custom_checker {

		bool value;

	public:
		always_check(bool value) : value(value) {}

		~always_check() = 0 {};

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

	class handler {

	protected:
		// bool at = false; // 这个玩意只有群聊才有用

	public:
		handler() = default;

		virtual ~handler() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0; // 重要！不再支持多条消息回复，可通过调用多个handler完成
	};

	class multiple_reply : virtual public handler { // 同时对一条消息执行多个回复，分行输出

		// vector<handler*> v; // 这个东西要分情况，到时候分别定义

	public:
		multiple_reply() : handler() {}

		~multiple_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};

	class orderly_reply : virtual public handler { // 对一条消息依次（递归）执行多个回复
		// vector<handler*> v;

	public:
		orderly_reply() : handler() {}

		virtual ~orderly_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target &) = 0;
	};

	class fixed_reply : virtual public handler { // 固定回复

	protected:
		string s;

	public:
		fixed_reply(string s) : handler(), s(s) {}

		~fixed_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};

	class random_reply : virtual public handler {

	protected:
		vector<string> v;

	public:
		random_reply(vector<string> &v) : handler(), v(v) {}

		~random_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};

	class single_para_reply : virtual public handler { // 单参数回复

	protected:
		string pre, suf, p, s;

	public:
		single_para_reply(string pre, string suf, string p, string s) :
			pre(pre), suf(suf), p(p), s(s) {}

		virtual ~single_para_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};

	class custom_reply : virtual public handler { // 自定义回复

	public:
		custom_reply() : handler() {}

		virtual ~custom_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};

	/*
	class repeater_reply : public custom_reply { // 我是一只复读机

	public:
		repeater_reply() : custom_reply(false) {}

		~repeater_reply() = 0 {};

		string reply(const MessageEvent& e, const Target &u) {
			return e.message;
		}
	};
	*/

	class url_reply : virtual public custom_reply { // 搜索引擎回复

	protected:
		string pre, suf;

	public:
		url_reply(string pre, string suf, bool at = false) : custom_reply(), pre(pre), suf(suf) {}

		virtual ~url_reply() = 0 {};

		virtual string reply(const MessageEvent& e, const Target &u) = 0;
	};

	class report_reply : virtual public custom_reply {
	
	public:
		report_reply() : custom_reply() {}

		virtual ~report_reply() = 0 {};

		virtual string reply(const MessageEvent&, const Target&) = 0;
	};
}

using namespace checker_and_handler;

#endif