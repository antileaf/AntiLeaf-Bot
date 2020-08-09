#ifndef DEVINE_HPP
#define DEVINE_HPP

#include <ctime>

const vector<string> devine = {"吉", "中吉", "中", "凶"};
const vector<vector<string>> desc = {
	{"非常顺利的一天，要保持好心情喔", "1000"},
	{"恋爱吉日，如果有喜欢的人不妨赶快向ta表白吧", "1000"},
	{"财运顺利的一天，记得多加留意，不要失之交臂哦", "1100"},
	{"无论做什么大多都能成功的一天，放手去做自己喜欢的事情吧", "1100"},
	{"异性缘很好的一天，可以试着多去勾搭你喜欢的ta哦", "1100"},
	{"人缘比较好的一天，可以试着多与他人交流，如果有什么矛盾也可以试着在今天缓和一下", "0110"},
	{"沉迷于工作/学习的一天，忙碌之余也要注意劳逸结合哦", "0110"},
	{"平平常常的一天，保持积极的心态度过充实的一天吧", "0010"},
	{"异性缘不太好的一天，和喜欢的ta聊天时多多留意不要踩雷", "0010"},
	{"心情有些波动的一天，如果情绪低落可以买点喜欢的东西安慰自己", "0011"},
	{"运气有点小差的一天，走路时注意不要被绊倒", "0001"},
	{"财运不太顺利的一天，花钱时要三思，不要花冤枉钱哦", "0001"},
};

int get_id(int64_t user_id, time_t time, int p) {
	int64_t day = time / (24 * 60 * 60);
	return (day ^ user_id) % p * (day + user_id * 2) % p * (((day * day) % p + p) + user_id) % p;
}

string get_result(int64_t user_id) {
	const int moduler = 1005060097;
	int t = get_id(user_id, time(0), moduler);
	int o = t % 4;
	vector<string> v;
	for (auto p : desc)
		if (p[1][o] == '1')
			v.push_back(p[0]);
	return "的今日运势：" + devine[o] + " " + v[t % v.size()];
}

class group_devine_reply : public group_custom_reply {

public:
	group_devine_reply() : group_custom_reply(true) {}

	~group_devine_reply() = default;

	string reply(const MessageEvent &e, const Target &u) {
		string s = get_result(u.user_id.value());

		s = ::at(u.user_id.value()) + " " + s;

		return s;
	}
	
};

class private_devine_reply : public private_custom_reply {

public:
	private_devine_reply() : private_custom_reply() {}

	~private_devine_reply() = default;

	string reply(const MessageEvent &e, const Target &u) {
		string s = get_stranger_info(u.user_id.value()).nickname + get_result(u.user_id.value());
		return s;
	}
	
};

#endif