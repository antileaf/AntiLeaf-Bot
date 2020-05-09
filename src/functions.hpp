#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

namespace enabled_functions {

	using namespace checker_and_handler;

	vector<pair<group_checker*, group_handler*>> group_functions;
	vector<pair<private_checker*, private_handler*>> private_functions;

	vector<pair<vector<string>, string>> usage = { // 用法帮助
		{{"直播", "查房"}, "直播+你要查询的群友常用名称\n随机查房（随机返回某位群友的直播间）\nDD查房（随机返回某位vtb的直播间）"},
		{{"直播", "查房"}, "直播+你要查询的群友常用名称"},
	};

	string group_help_info = string("\n\
Bot v1.0  By AntiLeaf\n\
注意：群聊功能请在指令前加上 % 符号！\n\
\n\
群聊功能                      使用方法\n\
--------------------------------------------------\n\
复读            你最好不要知道\n\
网站查询         网站 + 你想查询的网站\n\
直播查询         直播 + 你想查询的群友常用id\n\
随机查房         随机查房\n\
随机看vtb        DD\n\
（网站列表与直播间列表请联系绿鸽鸽或ks提供）\n\
百度 / 谷歌搜索  百度 / 谷歌一下 + 你想搜索的东西\n\
")
#ifdef RUNCODE_HPP
	+ "\
Python / C++   python / py / cpp + 你想执行的指令\n\
"
#endif
#ifdef WORM_HPP
	+ " \
OEIS               oeis / OEIS + 你想搜索的东西\n\
"
#endif
	+ "\
输出            echo / 输出 + 你想输出的东西\n\
贴贴            只有绿鸽鸽钦定的人才能使用\n\
帮助            help / 帮助\n\
反馈            反馈 / report + 你想反馈给绿鸽鸽的东西\n\
诗词收集         诗词 + 内容\n\
（添加 + 诗词：添加句子 存档：保存已收集诗词\n\
加载：读取已存档的诗词 无内容：随机返回已收集诗词）\n\
"
#ifdef BAN_HPP
	+ "\
禁言            禁言 + 分钟数（好好学习吧！）\n\
"
#endif
	+ "\
最近更新：Bot大重构，添加私聊功能框架，添加反馈功能\n\
下一步计划：添加狼人杀助手\n\
TODO：添加Wolfram Alpha查询等爬虫功能\n\
GitHub仓库：https://github.com/AntiLeaf/AntiLeaf-Bot\n\
",
private_help_info = string("\n\
Bot v1.0  By AntiLeaf\n\
注意：私聊功能请勿添加 % 符号！\n\
\n\
私聊功能                      使用方法\n\
--------------------------------------------------\n\
网站查询         网站 + 你想查询的网站\n\
直播查询         直播 + 你想查询的群友常用id\n\
随机查房         随机查房\n\
网站列表         网站列表\n\
随机看vtb        DD\n\
vtb列表         vtb列表\n\
百度 / 谷歌搜索  百度 / 谷歌一下 + 你想搜索的东西\n\
")
#ifdef RUNCODE_HPP
+ "\
Python / C++   python / py / cpp + 你想执行的指令\n\
"
#endif
#ifdef WORM_HPP
+ " \
OEIS               oeis / OEIS + 你想搜索的东西\n\
"
#endif
+ "\
输出            echo / 输出 + 你想输出的东西\n\
贴贴            请你吃桃子\n\
帮助            help / 帮助\n\
反馈            反馈 / report + 你想反馈给绿鸽鸽的东西\n\
诗词收集         诗词 + 内容\n\
（添加 + 诗词：添加句子 存档：保存已收集诗词\n\
加载：读取已存档的诗词 无内容：随机返回已收集诗词）\n\
"
+ "\
最近更新：Bot大重构，添加私聊功能框架，添加反馈功能\n\
下一步计划：添加狼人杀助手\n\
TODO：添加Wolfram Alpha查询等爬虫功能\n\
GitHub仓库：https://github.com/AntiLeaf/AntiLeaf-Bot\n\
";

	vector<pair<vector<string>, string>> websites = { // 常用网站
		{ { "百度", "百度搜索", "baidu" }, "www.baidu.com" },
	{ { "谷歌", "google", "Google", "谷歌搜索" }, "www.google.com" },
	{ { "谷歌翻译", "翻译" }, "translate.google.cn" },
	{ { "淘宝", "淘宝网", "桃饱网" }, "www.taobao.com" },
	{ { "京东", "jd", "JD", "二手东" }, "www.jd.com" },
	{ { "b站", "哔哩哔哩", "bilibili" }, "www.bilibili.com" },
	{ { "oeis", "OEIS" }, "oeis.org" },
	{ { "op.gg", "OP.GG" }, "op.gg" },
	{ { "cf", "CF", "codeforce", "codeforces" }, "codeforces.com" },
	{ { "UOJ", "uoj", "Universal OJ", "universal oj", "universal OJ" }, "uoj.ac" },
	{ { "LOJ", "loj", "Loj", "LibreOJ", "Libreoj", "libreoj" }, "loj.ac" },
	{ { "atcoder", "Atcoder", "AtCoder" }, "atcoder.jp" },
	{ { "Pixiv", "P站", "p站", "pixiv", "PIXIV" }, "www.pixiv.net （不要看人家的本子嘛~[CQ:face,id=111]）" },
	{ { "atcoder", "Atcoder", "AtCoder" }, "atcoder.jp" },
	{ { "Pornhub", "pornhub", "PornHub", "P\\*\\**Hub", "P\\*\\**hub", "p\\*\\**hub", "\\*\\**hub", "\\*\\**Hub", "\\*\\**HUB" }, "\n[CQ:face,id=26]你要看什么奇怪的网站，太可怕了QAQ" },
	{ { "Vijos", "vijos", "VIJOS" }, "vijos.org" },
	{ { "bzoj", "Bzoj", "BZOJ", "LYDSY", "lydsy", "八中oj", "八中OJ", "板子OJ", "板子oj" }, "www.lydsy.com" },
	{ { "GitHub", "Github", "github", "gayhub", "GayHub", "Gayhub" }, "github.com" },
	{ { "POJ", "Poj", "poj" }, "poj.org" },
	{ { "OneDrive", "onedrive", "Onedrive" }, "onedrive.live.com" },
	{ { "微云", "Weiyun", "weiyun" }, "weiyun.com" },
	{ { "百度网盘", "度盘" }, "pan.baidu.com" },
	{ { "洛谷", "Luogu", "luogu", "锣鼓" }, "luogu.org" },
	{ { "OierDb", "oierdb", "oier", "OIerdb", "OIerDb", "OierDB", "OIerDB" }, "bytew.net/OIer" },
	{ { "知乎", "zhihu", "逼乎", "某乎" }, "zhihu.com" },
	{ { "维基百科", "维基", "wiki", "wikipedia", "Wikipedia" }, "zh.wikipedia.org" },
	{ { "yande.re", "yande", "Yande" }, "yande.re" },
	{ { "Steam", "steam", "STEAM" }, "store.steampowered.com" },
	{ { "Wolfram Alpha", "wolfram *alpha", "Wolfram *Alpha" }, "https://www.wolframalpha.com/" },
	};

	vector<pair<vector<string>, string>> live = { // 直播速达
		{{"AntiLeaf", "绿鸽鸽", "绿哥哥", "antileaf", "反叶子", "反对叶子", "绿叶", "绿"}, "live.bilibili.com/21493332" },
		{{"KS", "ks", "KsKun", "Kskun", "kskun"}, "https://ksmeow.moe/live"},
		{{"M晓", "m晓", "咕咕晓"}, "live.bilibili.com/30014"},
		{{"Megumin", "megumin", "蘑菇民", "蘑菇米", "mgm"}, "live.bilibili.com/14866481"},
		{{"北啻", "北帝口"}, "live.bilibili.com/152745"},
		{{"Ringoer", "ringoer"}, "live.bilibili.com/706938"},
		{{"Kamigen", "kamigen"}, "live.bilibili.com/9508073"},
		{{"Pantw", "pantw", "PTW", "Ptw", "ptw", "群主", "葡萄味"}, "live.bilibili.com/4299357"},
		{{"MegaOwler", "Megaowler", "megaowler", "百万猫头鹰"}, "live.bilibili.com/917033"},
		{{"Slanterns", "SLanterns", "slanterns", "slantern", "Slantern"}, "live.bilibili.com/627355"},
		{{"Democracy", "democracy"}, "live.bilibili.com/4620643"},
		{{"icy", "Icy", "绿泡泡", "ICY"}, "live.bilibili.com/8815853"},
	}, DD = {
		{{"夏色祭", "夏哥", "祭妹", "matsuri", "Matsuri", "祭", "夏色まつり", "まつり", "马自立"}, "live.bilibili.com/13946381"},
		{{"白上吹雪", "小狐狸", "屑狐狸", "FBK", "Fubuki", "fbk", "fubuki", "吹雪", "白上フブキ", "フブキ", "kon", "konkon"}, "live.bilibili.com/11588230"},
		{{"神楽Mea", "mea", "Mea", "屑女仆", "神乐mea", "神乐Mea", "神楽mea", "咩啊"}, "live.bilibili.com/12235923"},
		{{"神乐七奈", "かぐらなな", "狗妈", "神楽七奈", "神乐花菜", "神楽花菜", "猫妈", "妈妈"}, "live.bilibili.com/21304638"},
		{{"百鬼绫目", "百鬼あやめ", "あやめ", "狗狗"}, "live.bilibili.com/21130785"},
		{{"赤井心", "心心", "はあと", "赤井", "心"}, "live.bilibili.com/14275133"},
		{{"凑阿库娅", "凑-阿库娅", "湊あくあ", "aqua", "Aqua", "凑阿夸", "傻夸", "阿夸", "阿库娅"}, "live.bilibili.com/14917277"},
		{{"天音彼方", "天音", "Kana碳", "天音碳", "天音かなた", "Kanata", "kanata"}, "live.bilibili.com/21752681"},
		{{"润羽露西娅", "露西亚", "露西娅", "Rushia", "砧板"}, "live.bilibili.com/21545232"},
		{{ "花丸晴琉", "花丸はれる", "花丸", "hanamaru"}, "live.bilibili.com/21547895"},
		{{"鹿乃", "kano"}, "live.bilibili.com/21547895"},
		{{"野々宮ののの", "野野宫ののの", "ののの", "nonono", "nonomiya"}, "live.bilibili.com/21671418"},
		{{ "小东人鱼", "小東ひとな", "小东人魚", "小东", "kohigashi"}, "live.bilibili.com/21547904"},
		{{"泠鸢yousa", "泠鸢", "yousa", "冷鸟"}, "live.bilibili.com/593"}
	};

	vector<pair<vector<string>, vector<string>>> conver = { // 对话
		{{"爬", "给爷爬", "爪巴", "给爷爪巴"}, {"你怎么能让可爱的我爬呢，你坏坏QAQ"}},
		{{"AntiLeaf", "antileaf", "绿鸽鸽", "绿哥哥"}, {"绿鸽鸽好帅，最喜欢他了！QwQ"}},
		{{"呐呐", "呐呐呐", "呐 呐", "呐 呐 呐"}, {"自 动 声 呐 系 统", "巧了我也是二次元[CQ:face,id=109]"}},
		{{"Bot", "bot", "BOT", "⑨" }, { "这里是绿鸽鸽的可爱Bot哦~"}},
		{{"复读", "复读机", "复读姬" }, { "小火汁，原来你也是复读机？"}},
	};

	vector<pair<pair<string, string>, pair<string, string>>> single_para = { // 单参数命令
		{{"echo", ""}, {"", ""}},
		{{"输出", ""}, {"", ""}}
	};

	vector<pair<vector<string>, vector<string>>> dear = { // 贴贴
		{{"亲亲", "\\[CQ:face,id=109\\]"}, {"[CQ:face,id=109][CQ:face,id=109][CQ:face,id=109]"}},
		{{"贴贴" }, {"绿鸽鸽好帅，我和绿鸽鸽贴贴[CQ:face,id=109][CQ:face,id=109]", "阿姨洗铁路哟~[CQ:face,id=109]"}},
		{{"说几句好听的", "说两句好听的", "来两句好听的", "来几句好听的", "来句好听的", "说句好听的" }, { "绿鸽鸽最帅了[CQ:face,id=109]", "我最喜欢绿鸽鸽了[CQ:face,id=109]", "绿鸽鸽帅帅！[CQ:face,id=109]"}},
		{{"卖个萌", "来卖个萌", "卖萌", "qwq", "qwqwq"}, {"[CQ:face,id=111][CQ:face,id=111][CQ:face,id=111]", "QwQ", "喵喵喵"}},
	};

	// 诗词句子收集写在专用的命名空间中

	vector<pair<vector<string>, vector<string>>> greetings = { // 问好
		{ { "早安", "说早安", "早上好", "早" }, { "早安~", "早上好~新的一天也要元气满满哦！" } },
		{ { "上午好" }, { "上午好~事不宜迟，开始忙碌的一天吧！" } },
		{ { "午安", "说午安", "中午好" }, { "午安~", "中午好~不要忘记睡午觉喔~" } },
		{ { "下午好" }, { "下午好~吃点什么零食呢？" } },
		{ { "晚上好" }, { "晚上好~晚上干活虽好，也要注意少熬夜呢~" } },
		{ { "晚安", "说晚安" }, { "晚安，好好休息一下吧~" } },
	};

	vector<string> poker_deck;

#define add_group_function(a, b) group_functions.push_back(make_pair((new a), (new b)))
#define add_group_fixed(a, b, c) add_group_function(group_complete_match(a), group_fixed_reply(b, c))
#define group_and_check(a, b) group_and_checker(vector<group_checker*>({new a, new b}))
#define group_multi_reply(a, b, c) group_multiple_reply(vector<group_handler*>({new a, new b}), c)
#define group_order_reply(a, b, c) group_orderly_reply(vector<group_handler*>({new a, new b}), c)

	void init() {
		for (string s : {"♥", "♠", "♣", "♦"})
			for (string t : {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"})
				poker_deck.push_back(s + t);
		poker_deck.push_back("JOKER");
		poker_deck.push_back("joker");
		for (auto &s : poker_deck)
			s = "你抽到了：" + s;
	}

	void group_init() {

		for (auto s : { "help", "帮助" })
			add_group_function(group_complete_match(s), group_fixed_reply(group_help_info, true));

		for (auto p : websites)
			for (auto o : p.first)
				add_group_function(group_complete_match("网站 *" + o), group_fixed_reply(p.first[0] + "： " + p.second, true)); // 网站查询

		string s = "支持的网站列表：";
		for (auto p : websites)
			s += "\n" + p.first[0];
		add_group_function(group_and_check(group_user_id_check(1094054222), group_complete_match("网站列表")), group_fixed_reply(s, false)); // 网站列表

		for (auto p : live)
			for (auto o : p.first)
				for (string s : {"直播", "查房"})
					add_group_fixed(s + " *" + o, p.first[0] + "的直播间： " + p.second, true); // 直播查询
		for (auto p : DD)
			for (auto o : p.first)
				for (string s : {"直播", "查房"})
					add_group_fixed(s + " *" + o, p.first[0] + "的直播间： " + p.second, true); // 直播查询

		vector<string> v;
		for (auto p : live)
			v.push_back(p.first[0] + "的直播间： " + p.second);
		add_group_function(group_complete_match("随机查房 *"), group_random_reply(v, true)); // 随机查房

		v.clear();
		for (auto p : DD)
			v.push_back(p.first[0] + "的直播间： " + p.second);
		for (string s : {"DD", "DD查房", "滴滴", "臭滴滴", "臭DD", "dd", "臭dd", "随机滴滴", "随机DD", "随机dd"})
			add_group_function(group_complete_match(s + " *"), group_random_reply(v, true)); // DD查房

		s = "直播列表：";
		for (auto p : live)
			s += "\n" + p.first[0] + " : " + p.second;
		add_group_function(group_and_check(group_user_id_check(1094054222), group_complete_match("直播列表")), group_fixed_reply(s, false)); // 直播列表
		s = "Vtuber列表：";
		for (auto p : DD)
			s += "\n" + p.first[0] + " : " + p.second;
		add_group_function(group_and_check(group_user_id_check(1094054222), group_complete_match("vtb列表")), group_fixed_reply(s, false)); // DD列表

		for (string s : {"扑克牌", "扑克", "纸牌", "poker", "Poker", "牌", "发牌"})
			add_group_function(group_complete_match(s + " *"), group_random_reply(poker_deck, true)); // 发牌员来张牌

		for (auto p : conver)
			for (auto o : p.first)
				add_group_function(group_complete_match(o), group_random_reply(p.second, true)); // 对话

		for (auto p : single_para)
			add_group_function(group_presuf_match(p.first.first, p.first.second), group_single_para_reply(p.first.first, p.first.second, p.second.first, p.second.second)); // 单参数命令

		for (auto s : { "百度一下", "baidu一下", "Baidu一下", "百度", "baidu", "Baidu" })
			add_group_function(group_presuf_match(s, ""), group_order_reply(group_single_para_reply(s, "", "", ""), group_url_reply("www.baidu.com/s?wd=", ""), true)); // 百度搜索

		for (auto s : { "谷歌一下", "Google一下", "google一下", "谷歌", "google", "Google" })
			add_group_function(group_presuf_match(s, ""), group_order_reply(group_single_para_reply(s, "", "", ""), group_url_reply("www.google.com/search?q=", ""), true)); // 谷歌搜索

#ifdef RUNCODE_HPP

		for (auto s : { "Python", "python", "py", "Py" })
			add_group_function(group_presuf_match(s, ""), group_order_reply(group_single_para_reply(s, "", "", ""), group_python_reply(false), true)); // Python

		for (auto s : { "C\\+\\+", "c\\+\\+", "cpp", "CPP", "c艹", "C艹" })
			add_group_function(group_presuf_match(s, ""), group_order_reply(group_single_para_reply(s, "", "", ""), group_cpp_reply(false), true)); // C++

#endif

		for (auto p : dear)
			for (auto o : p.first)
				for (auto id : AUTHORIZED_USERS)
					add_group_function(group_and_check(group_user_id_check(id), group_complete_match(o)), group_random_reply(p.second, true)); // 贴贴（仅限自己使用）
		for (auto p : dear)
			for (auto o : p.first)
				add_group_function(group_complete_match(o), group_fixed_reply("请你吃桃子", true)); // 贴贴（让群友吃桃子）

		for (auto p : greetings)
			for (auto o : p.first)
				add_group_function(group_complete_match(o), group_random_reply(p.second, true)); // 问好

#ifdef COLLECTION_HPP

		add_group_function(group_presuf_match("诗词", ""), group_order_reply(group_single_para_reply("诗词", "", "", ""), group_collection_reply(collections::poems), true)); // 诗词收集

#endif

#ifdef WORM_HPP

		for (auto s : { "OEIS", "oeis", "Oeis" })
			add_group_function(group_presuf_match(s, ""), group_order_reply(group_single_para_reply(s, "", "", ""), group_multi_reply(oeis_reply(), url_reply("更多结果： http://oeis.org/search?q=", ""), false), true));

#endif

#ifdef BAN_HPP

		add_group_function(group_presuf_match("禁言", ""), group_ban_reply(true));

#endif

		add_group_function(group_repeater_check(), group_repeater_reply()); // 复读机

		for (string s : {"反馈", "report"})
			add_group_function(group_presuf_match(s, ""), group_order_reply(group_single_para_reply(s, "", "", ""), group_report_reply(), true));

		// TODO : 添加更多功能

		add_group_function(group_presuf_match(""), group_fixed_reply("不好意思，人家现在还听不懂哦~[CQ:face,id=111]\n如有需要可以联系绿鸽鸽添加相关功能~[CQ:face,id=111]", true)); // 没听懂
	}

#define add_private_function(a, b) private_functions.push_back(make_pair((new a), (new b)))
#define add_private_fixed(a, b) add_private_function(private_complete_match(a), private_fixed_reply(b))
#define private_and_check(a, b) private_and_checker(vector<private_checker*>({new a, new b}))
#define private_multi_reply(a, b) private_multiple_reply(vector<private_handler*>({new a, new b}))
#define private_order_reply(a, b) private_orderly_reply(vector<private_handler*>({new a, new b}))

	void private_init() {
		for (auto s : { "help", "帮助" })
			add_private_function(private_complete_match(s), private_fixed_reply(private_help_info));

		for (auto p : websites)
			for (auto o : p.first)
				add_private_function(private_complete_match("网站 *" + o), private_fixed_reply(p.first[0] + "： " + p.second)); // 网站查询

		string s = "支持的网站列表：";
		for (auto p : websites)
			s += "\n" + p.first[0];
		add_private_function(private_complete_match("网站列表"), private_fixed_reply(s)); // 网站列表

		for (auto p : live)
			for (auto o : p.first)
				for (string s : {"直播", "查房"})
					add_private_fixed(s + " *" + o, p.first[0] + "的直播间： " + p.second); // 直播查询
		for (auto p : DD)
			for (auto o : p.first)
				for (string s : {"直播", "查房"})
					add_private_fixed(s + " *" + o, p.first[0] + "的直播间： " + p.second); // 直播查询

		vector<string> v;
		for (auto p : live)
			v.push_back(p.first[0] + "的直播间： " + p.second);
		add_private_function(private_complete_match("随机查房 *"), private_random_reply(v)); // 随机查房

		v.clear();
		for (auto p : DD)
			v.push_back(p.first[0] + "的直播间： " + p.second);
		for (string s : {"DD", "DD查房", "滴滴", "臭滴滴", "臭DD", "dd", "臭dd", "随机滴滴", "随机DD", "随机dd"})
			add_private_function(private_complete_match(s + " *"), private_random_reply(v)); // DD查房

		s = "直播列表：";
		for (auto p : live)
			s += "\n" + p.first[0] + " : " + p.second;
		add_private_function(private_complete_match("直播列表"), private_fixed_reply(s)); // 直播列表
		s = "Vtuber列表：";
		for (auto p : DD)
			s += "\n" + p.first[0] + " : " + p.second;
		add_private_function(private_complete_match("vtb列表"), private_fixed_reply(s)); // DD列表

		for (string s : {"扑克牌", "扑克", "纸牌", "poker", "Poker", "牌", "发牌"})
			add_private_function(private_complete_match(s + " *"), private_random_reply(poker_deck)); // 发牌员来张牌

		for (auto p : conver)
			for (auto o : p.first)
				add_private_function(private_complete_match(o), private_random_reply(p.second)); // 对话

		for (auto p : single_para)
			add_private_function(private_presuf_match(p.first.first, p.first.second), private_single_para_reply(p.first.first, p.first.second, p.second.first, p.second.second)); // 单参数命令

		for (auto s : { "百度一下", "baidu一下", "Baidu一下", "百度", "baidu", "Baidu" })
			add_private_function(private_presuf_match(s, ""), private_order_reply(private_single_para_reply(s, "", "", ""), private_url_reply("www.baidu.com/s?wd=", ""))); // 百度搜索

		for (auto s : { "谷歌一下", "Google一下", "google一下", "谷歌", "google", "Google" })
			add_private_function(private_presuf_match(s, ""), private_order_reply(private_single_para_reply(s, "", "", ""), private_url_reply("www.google.com/search?q=", ""))); // 谷歌搜索

#ifdef RUNCODE_HPP

		for (auto s : { "Python", "python", "py", "Py" })
			add_private_function(private_presuf_match(s, ""), private_order_reply(private_single_para_reply(s, "", "", ""), private_python_reply(false))); // Python

		for (auto s : { "C\\+\\+", "c\\+\\+", "cpp", "CPP", "c艹", "C艹" })
			add_private_function(private_presuf_match(s, ""), private_order_reply(private_single_para_reply(s, "", "", ""), private_cpp_reply(false))); // C++

#endif

		for (auto p : dear)
			for (auto o : p.first)
				for (auto id : AUTHORIZED_USERS)
					add_private_function(private_and_check(private_user_id_check(id), private_complete_match(o)), private_random_reply(p.second)); // 贴贴（仅限自己使用）
		for (auto p : dear)
			for (auto o : p.first)
				add_private_function(private_complete_match(o), private_fixed_reply("请你吃桃子")); // 贴贴（让群友吃桃子）

		for (auto p : greetings)
			for (auto o : p.first)
				add_private_function(private_complete_match(o), private_random_reply(p.second)); // 问好

#ifdef COLLECTION_HPP

		add_private_function(private_presuf_match("诗词", ""), private_order_reply(private_single_para_reply("诗词", "", "", ""), private_collection_reply(collections::poems))); // 诗词收集

#endif

#ifdef WORM_HPP

		for (auto s : { "OEIS", "oeis", "Oeis" })
			add_private_function(private_presuf_match(s, ""), private_order_reply(private_single_para_reply(s, "", "", ""), private_multi_reply(oeis_reply(), url_reply("更多结果： http://oeis.org/search?q=", ""))));

#endif

		for (string s : {"反馈", "report"})
			add_private_function(private_presuf_match(s, ""), private_order_reply(private_single_para_reply(s, "", "", ""), private_report_reply(), true));

		// TODO : 添加更多功能

		add_private_function(private_presuf_match(PREF, ""), private_fixed_reply("私聊请不要添加 % 符号哦~")); // 别加%了

		add_private_function(private_presuf_match(""), private_fixed_reply("不好意思，人家现在还听不懂哦~[CQ:face,id=111]\n如有需要可以联系绿鸽鸽添加相关功能~[CQ:face,id=111]")); // 没听懂
	}

}

namespace disabled_functions { // 行 为 艺 术

}

#endif