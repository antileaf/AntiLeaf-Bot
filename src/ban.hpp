#ifndef BAN_HPP
#define BAN_HPP

#include <string>
#include <cctype>
#include <regex>

#include "checker_and_handler.hpp"
#include "string_tools.hpp"

using namespace checker_and_handler;

class group_ban_reply final : public group_custom_reply {
	
protected:

public:
	group_ban_reply(bool at = false) : group_custom_reply(at) {}

	~group_ban_reply() = default;

	string reply(const GroupMessageEvent &e) {
		string s = e.message;
		if (!regex_match(s, regex(" *" + PREF + " *禁言 *[0-9]* *")))
			return (at ? ::at(e.user_id) : string("")) + "　非法输入，请重试";

		int t = 0;
		for (auto c : s)
			if (isdigit(c))
				t = t * 10 + c - '0';
		if (t <= 0)
			return (at ? ::at(e.user_id) : string("")) + "　非法输入，请重试";
		if (t > 30 * 24 * 60)
			return (at ? ::at(e.user_id) : string("")) + "　时间过长，禁言时间不能超过30天";

		set_group_ban(e.group_id, e.user_id, t * 60);
		return (at ? ::at(e.user_id) : string("")) + " 已被禁言" + to_string(t) + "分钟";
	}
};

#endif