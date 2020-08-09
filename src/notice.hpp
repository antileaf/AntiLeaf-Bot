#ifndef NOTICE_HPP
#define NOTICE_HPP

#include <string>

#include <cqcppsdk/cqcppsdk.h>

#include "message_tools.hpp"

using namespace std;

bool handle_notice(const NoticeEvent &e) {
	if (e.detail_type == NoticeEvent::DetailType::GROUP_MEMBER_INCREASE) { // 欢迎新成员
		message_tools::send_group_message(e.target.group_id.value(), "欢迎" + ::at(e.target.user_id.value()) + " 加入本群！");
	}

	else if (e.detail_type == NoticeEvent::DetailType::GROUP_MEMBER_DECREASE) { // 有人退群
		message_tools::send_group_message(e.target.group_id.value(), to_string(e.target.user_id.value()) + " 离开了本群");
	}

	else { // 暂时还没写
		
	}
}

#endif