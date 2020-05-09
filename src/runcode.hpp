#ifndef RUNCODE_HPP
#define RUNCODE_HPP

#include <string>

#include <cqcppsdk/cqcppsdk.h>

#include "checker_and_handler.hpp"

class python_reply : virtual public custom_reply { // Python

public:
	python_reply() : custom_reply() {}

	virtual ~python_reply() = default;

	virtual string reply(const MessageEvent& e) = 0;
};

class group_python_reply final : public group_custom_reply, public python_reply { // Python功能

public:
	group_python_reply(bool at = false) : group_custom_reply(at), python_reply() {}

	~group_python_reply() = default;

	string reply(const MessageEvent &e, const Target &u) {
		string s = e.message, name = to_string(e.message_id);
		erase_pre(s);

		ofstream g(name + ".py");
		g << "# -*- coding: utf-8 -*- \n" + s;
		g.close();
		if (system(("python " + name + ".py > " + name + ".txt").c_str())) {
			system(("del " + name + ".py").c_str());
			system(("del " + name + ".txt").c_str());
			return "运行时出错";
		}

		ifstream f(name + ".txt");
		ostringstream os;
		os << f.rdbuf();
		f.close();

		system(("del " + name + ".py").c_str());
		system(("del " + name + ".txt").c_str());
		return os.str();
	}
};

class private_python_reply final : public private_custom_reply, public python_reply { // Python功能

public:
	private_python_reply() : private_custom_reply(), python_reply() {}

	~private_python_reply() = default;

	string reply(const MessageEvent& e) {
		string s = e.message, name = to_string(e.message_id);
		erase_pre(s);

		ofstream g(name + ".py");
		g << "# -*- coding: utf-8 -*- \n" + s;
		g.close();
		if (system(("python " + name + ".py > " + name + ".txt").c_str())) {
			system(("del " + name + ".py").c_str());
			system(("del " + name + ".txt").c_str());
			return "运行时出错";
		}

		ifstream f(name + ".txt");
		ostringstream os;
		os << f.rdbuf();
		f.close();

		system(("del " + name + ".py").c_str());
		system(("del " + name + ".txt").c_str());
		return os.str();
	}
};

// ----------------- C++ -------------------

class cpp_reply : virtual public custom_reply { // C++

public:
	cpp_reply() : custom_reply() {}

	virtual ~cpp_reply() = default;

	virtual string reply(const MessageEvent &e, const Target &u) = 0;
};

class group_cpp_reply final : public group_custom_reply, public cpp_reply { // 

public:
	group_cpp_reply(bool at = false) : group_custom_reply(at), cpp_reply() {}

	~group_cpp_reply() = default;

	string reply(const MessageEvent &e, const Target &u) {
		string s = e.message, name = to_string(e.message_id);
		erase_pre(s);

		ofstream g(name + ".cpp");
		g << s;
		g.close();
		if (system(("g++ " + name + ".cpp -std=c++17 -o " + name).c_str())) {
			system(("del " + name + ".cpp").c_str());
			return "编译失败";
		}
		if (system((name + " > " + name + ".txt").c_str())) {
			system(("del " + name + ".cpp").c_str());
			system(("del " + name + ".exe").c_str());
			system(("del " + name + ".txt").c_str());
			return "运行时出错";
		}

		ifstream f(name + ".txt");
		ostringstream os;
		os << f.rdbuf();
		f.close();

		system(("del " + name + ".exe").c_str());
		system(("del " + name + ".txt").c_str());
		return os.str();
	}
};

class private_cpp_reply final : public private_custom_reply, public cpp_reply { // 

public:
	private_cpp_reply() : custom_reply() {}

	~private_cpp_reply() = default;

	string reply(const MessageEvent &e, const Target &u) {
		string s = e.message, name = to_string(e.message_id);
		erase_pre(s);

		ofstream g(name + ".cpp");
		g << s;
		g.close();
		if (system(("g++ " + name + ".cpp -std=c++17 -o " + name).c_str())) {
			system(("del " + name + ".cpp").c_str());
			return "编译失败";
		}
		if (system((name + " > " + name + ".txt").c_str())) {
			system(("del " + name + ".cpp").c_str());
			system(("del " + name + ".exe").c_str());
			system(("del " + name + ".txt").c_str());
			return "运行时出错";
		}

		ifstream f(name + ".txt");
		ostringstream os;
		os << f.rdbuf();
		f.close();

		system(("del " + name + ".exe").c_str());
		system(("del " + name + ".txt").c_str());
		return os.str();
	}
};

#endif