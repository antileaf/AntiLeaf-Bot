#ifndef MESSAGE_TOOLS_HPP
#define MESSAGE_TOOLS_HPP

#include <string>

#include <cqcppsdk/cqcppsdk.h>

#include "string_tools.hpp"

namespace message_tools {

	using namespace std;
	using namespace string_tools;

	const int MAX_TRIES = 50;
	// const int DELAY = 500;

	void send_message(int64_t user_id, string s) {
		length_check(s);
		for (int k = 0; k < MAX_TRIES; k++) {
			try {
				send_message(Target::user(user_id), s);
				break;
			}
			catch (ApiError&) {}
			catch (...) {
				logging::error("错误", "未知错误");
				//break;
			}
		}
	}

	void send_group_message(int64_t group_id, string s) {
		length_check(s);
		for (int k = 0; k < MAX_TRIES; k++) {
			try {
				cq::send_group_message(group_id, s);
				break;
			}
			catch (ApiError&) {}
			catch (...) {
				logging::error("错误", "未知错误");
				//break;
			}
		}
	}
}

#endif