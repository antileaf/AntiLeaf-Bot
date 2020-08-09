#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include <string>

#include "checker_and_handler.hpp"
#include "string_tools.hpp"

namespace collections {
	class collection {

	private:
		string file_name;

		vector<string> v;

	public:
		collection(string file_name) : file_name(file_name) {}

		~collection() = default;

		int size() const {
			return (int)v.size();
		}

		string get() const {
			if (v.empty())
				return "现在还是空空的哦…";
			return v[gen() % (int)v.size()];
		}

		string add(const string& s) {
			for (auto& t : v)
				if (s == t)
					return "已经添加过了哦~";
			v.push_back(s);
			save();
			return "添加成功"; // 添加成功
		}

		string erase(const string &s) {
			auto i = find(v.begin(), v.end(), s);
			if (i == v.end())
				return "找不到\"" + s + "\"哦~";
			else {
				v.erase(i);
				save();
				return "删除成功";
			}
		}

		string load() {
			try {
				v.clear();
				ifstream f(cq_path + file_name + ".txt");
				string s;
				while (getline(f, s) && s != "")
					v.push_back(s);
				f.close();
				return "加载完成";
			}
			catch (...) {
				return "加载失败，检查一下出了什么问题吧…";
			}
		}

		string save() const { // 返回是否执行成功
			try {
				string s;
				for (auto t : v)
					s += t + "\n";
				ofstream f(cq_path + file_name + ".txt");
				f << s;
				f.close();
				return "";
			}
			catch (...) {
				return "存档失败，检查一下出了什么问题吧…";
			}
		}

		string stat() const { // 统计信息
			return "目前共保存了" + to_string(v.size()) + "条信息";
		}
	} *poems, *monkey;

	void init() {
		poems = new collection("poems");
		poems->load();

		monkey = new collection("monkey");
		monkey->load();
	}
}

using collections::collection;

class collection_reply : virtual public checker_and_handler::custom_reply { // 收集回复，集成添加、加载、存档、查询功能

protected:
	collection *o;

public:
	collection_reply(collection *o) : custom_reply(), o(o) {}

	virtual ~collection_reply() {}

	virtual string reply(const MessageEvent&, const Target&) = 0;
};


class group_collection_reply final : public group_custom_reply, public collection_reply { // 收集回复，集成添加、加载、存档、查询功能

protected:
	// collection* o;

public:
	group_collection_reply(collection* o, bool at = false) : group_custom_reply(at), collection_reply(o) {}

	~group_collection_reply() {}

	string reply(const MessageEvent &e, const Target &u) {
		using namespace string_tools;

		string s = e.message;
		erase_pre(s);
		if (start_with(s, "添加")) {
			erase_pre(s, "添加");
			erase_pre(s);
			erase_suf(s);
			return o->add(s);
		}
		else if (start_with(s, "删除")) {
			erase_pre(s, "添加");
			erase_pre(s);
			erase_suf(s);
			return o->erase(s);
		}
		else if (start_with(s, "加载")) {
			erase_pre(s, "加载");
			return o->load();
		}
		else if (start_with(s, "存档")) {
			erase_pre(s, "存档");
			return o->save();
		}
		else if (start_with(s, "统计")) {
			return o->stat();
		}
		else if (s == "") // 查询
			return o->get();
		return "指令有误，重新试一次吧~";
	}
};


class private_collection_reply : public private_custom_reply, public collection_reply { // 收集回复，集成添加、加载、存档、查询功能

protected:
	// collection* o;

public:
	private_collection_reply(collection* o) : collection_reply(o) {}

	~private_collection_reply() {}

	string reply(const MessageEvent &e, const Target &u) {
		using namespace string_tools;

		string s = e.message;
		erase_pre(s);
		if (start_with(s, "添加")) {
			if (!is_authorized(u.user_id.value()))
				return "抱歉，只有绿鸽鸽和ks才有这个权限哦~";
			erase_pre(s, "添加");
			erase_pre(s);
			erase_suf(s);
			return o->add(s);
		}
		else if (start_with(s, "加载")) {
			if (!is_authorized(u.user_id.value()))
				return "抱歉，只有绿鸽鸽和ks才有这个权限哦~";
			erase_pre(s, "加载");
			return o->load();
		}
		else if (start_with(s, "存档")) {
			if (!is_authorized(u.user_id.value()))
				return "抱歉，只有绿鸽鸽和ks才有这个权限哦~";
			erase_pre(s, "存档");
			return o->save();
		}
		else if (s == "") // 查询
			return o->get();
		return "指令有误，重新试一次吧~";
	}
};

#endif