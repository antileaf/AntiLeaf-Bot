#ifndef BAN_HPP
#define BAN_HPP

#include <string>
#include <cctype>
#include <regex>
#include <ctime>

#include "checker_and_handler.hpp"
#include "string_tools.hpp"

using namespace checker_and_handler;

map<int64_t, map<int64_t, pair<int64_t, int64_t>>> ban_map;

class group_ban_reply final : public group_custom_reply {
	
protected:
	// bool at = false;

public:
	group_ban_reply(bool at = false) : group_custom_reply(at) {}

	~group_ban_reply() = default;

	string reply(const MessageEvent &e, const Target &u) {
		string s = e.message;
		if (!regex_match(s, regex(" *" + PREF + " *禁言 *[0-9]* *")))
			return ::at(u.user_id.value()) + " 非法输入，请重试";

		int t = 0;
		for (auto c : s)
			if (isdigit(c))
				t = t * 10 + c - '0';
		if (t <= 0)
			return ::at(u.user_id.value()) + " 给老子爬";
		if (t > 30 * 24 * 60)
			return ::at(u.user_id.value()) + " 时间过长，禁言时间不能超过30天";

		set_group_ban(u.group_id.value(), u.user_id.value(), t * 60);
		ban_map[u.user_id.value()][u.group_id.value()] = make_pair(time(0), time(0) + t * 60);

		return ::at(u.user_id.value()) + " 已被禁言" + to_string(t) + "分钟\n如需反悔请于/*两分钟内*/在私聊中发送\"解除禁言\"或\"解禁\"";
	}
};

class private_repent_reply final : public private_custom_reply {

public:
	private_repent_reply() : private_custom_reply() {}

	~private_repent_reply() = default;

	string reply(const MessageEvent &e, const Target &u) {
		 bool ok = false;

		 for (auto g : ban_map[u.user_id.value()]) {
			auto o = g.second;
			if (time(0) < o.second) {
				ok = true;
				set_group_ban(g.first, u.user_id.value(), 0);
				message_tools::send_group_message(g.first, ::at(u.user_id.value()) + " 已被解除禁言");
			}
		 }
		 ban_map[u.user_id.value()].clear();

		 if (ok)
			 return "已解除禁言";
		 else
			 return "你现在没有被bot禁言哦…";
	}
};

#endif