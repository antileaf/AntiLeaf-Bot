#ifndef WORM_HPP
#define WORM_HPP

class oeis_reply final : public custom_reply { // OEIS查询，后面再接一个url最好

private:
	const string program_name = "tem.pyw";

public:
	oeis_reply(bool at = false) : custom_reply(at) {}

	~oeis_reply() = default;

	string reply(const GroupMessageEvent& e) {
		string p = urlencode(e.message);
		logging::info("debug", p);
		ofstream f("oeis.txt");
		f << p;
		f.close();
		system(("python " + program_name + " < oeis.txt").c_str());

		string u, s, t;
		ifstream g("oeis.txt");
		int k = 5;
		u = "以下是前" + to_string(k) + "个搜索结果：";
		while (g >> s >> t && k--) {
			u += "\n" + s + " " + t;
		}
		g.close();
		return u;
	}
};

#endif