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

#include <cqcppsdk/cqcppsdk.h>

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;
auto at = MessageSegment::at; // 这个auto感觉好蠢

const string cq_path = ""; // "C:\\Users\\AntiLeaf\\Desktop\\酷Q Pro\\";
const string PREF = "% *";
mt19937 gen;
const int64_t self_id = 625938638, producer_id = 1094054222;

// const set<int64_t> ENABLED_GROUPS = {441254450, 228725393, 780135491, 463471261, 301538258, 1047201693};
const set<int64_t> DISABLED_GROUPS = {};
const set<int64_t> NOTICE_GROUPS = {441254450, 228725393, 780135491, 1047201693};
const set<int64_t> AUTHORIZED_USERS = {1094054222, 780768723, 2966286680}; // AntiLeaf, KsKun, Cyanoki
bool is_authorized(int64_t user_id) {
	return AUTHORIZED_USERS.count(user_id);
}

#include "string_tools.hpp"
#include "message_tools.hpp"
#include "checker_and_handler.hpp"
#include "group.hpp"
#include "private.hpp"
#include "collection.hpp"
// #include "runcode.hpp"
// #include "worm.hpp"
// #include "uno.hpp"
#include "ban.hpp"
#include "devine.hpp"
#include "functions.hpp" // 必须包含
#include "notice.hpp" // 必须包含

auto send_message = message_tools::send_message;
auto send_group_message = message_tools::send_group_message;

using namespace message_tools;

using namespace checker_and_handler;
using namespace enabled_functions;

CQ_INIT {
	on_enable([]{
		logging::info("启用", "Bot已启用");

		gen = mt19937(time(0));
		collections::init();
		enabled_functions::init();
		enabled_functions::group_init();
		enabled_functions::private_init();

		for (auto group_id : NOTICE_GROUPS)
			message_tools::send_group_message(group_id, "Bot已启用，OEIS功能已上线\n珍爱Bot生命，请勿刷屏~[CQ:face,id=111][CQ:face,id=111]");
	});

	on_group_message([](const GroupMessageEvent &e){
		// if (ENABLED_GROUPS.count(e.group_id) == 0) return;
		if (DISABLED_GROUPS.count(e.group_id)) return;

		/*
		if (backend_functions::eval->check_group(e.group_id)) // 如果开启了群聊转发功能则转发消息
			::send_message(producer_id, backend_functions::eval->get_name(e.user_id) + "：" + e.message);
		*/

		for (auto o : group_functions) {
			if (o.first->check(e, e.target)) {
				auto s = o.second->reply(e, e.target);
				::send_group_message(e.group_id, s);

				/*
				if (backend_functions::eval->check_group(e.group_id)) // 如果开启了群聊转发功能则转发消息
					::send_message(producer_id, backend_functions::eval->get_name(self_id) + "：" + s);
				*/
				
				if (o.first->is_final()) {
					e.block();
					return;
				}
			}
		}
	});

	on_private_message([](const PrivateMessageEvent &e) {
		/*
		if (e.user_id == producer_id) {
			message_tools::send_message(producer_id, backend_functions::eval->reply(e.message));
		}
		else {
			if (backend_functions::eval->check_private(e.user_id))
				message_tools::send_message(producer_id, e.message); // 如果启用了私聊转发则转发消息
			else message_tools::send_message(e.user_id, private_functions::eval->reply(e.message));
		}
		e.block();
		*/

		for (auto o : enabled_functions::private_functions) {
			if (o.first->check(e, e.target)) {
				auto s = o.second->reply(e, e.target);
				::send_message(e.target.user_id.value(), s);
				// ::send_message(1094054222, "???");

				/*
				if (backend_functions::eval->check_group(e.group_id)) // 如果开启了群聊转发功能则转发消息
					::send_message(producer_id, backend_functions::eval->get_name(self_id) + "：" + s);
				*/
					
				if (o.first->is_final()) {
					e.block();
					return;
				}
			}
		}
	});

	on_notice([](const NoticeEvent &e) {
		if (DISABLED_GROUPS.count(e.target.group_id.value()))
			return;
		handle_notice(e);
		// e.block();
	});
}
