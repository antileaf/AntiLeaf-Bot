#ifndef WORM_HPP
#define WORM_HPP

// #define OEIS_ENABLED
#define OPGG_ENABLED

#ifdef OEIS_ENABLED

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

#ifdef OPGG_ENABLED

namespace opgg {

	vector<vector<string> > champions_list = {
		{ "aatrox", "暗裔剑魔", "亚托克斯", "剑魔" },
		{ "ahri", "九尾妖狐", "阿狸", "狐狸", "妖狐" },
		{ "akali", "离群之刺", "阿卡丽"},
		{ "alistar", "牛头酋长", "阿利斯塔", "牛头", "哞利斯塔" },
		{ "amumu", "殇之木乃伊", "阿木木", "木木", "木乃伊" },
		{ "anivia", "冰晶凤凰", "艾尼维亚", "冰鸟", "凤凰", "蛋尼维亚" },
		{ "annie", "黑暗之女", "安妮", "提伯斯" },
		{ "aphelios", "残月之肃", "厄斐琉斯", "吴亦凡", "无e烦", "无E烦", "无e凡", "无E凡", "五器大师", "月男" },
		{ "ashe", "寒冰射手", "艾希", "寒冰" },
		{ "aurelionsol", "铸星龙王", "奥瑞利安·索尔", "龙王", "奥瑞利安", "索尔" },
		{ "azir", "沙漠皇帝", "阿兹尔", "黄鸡", "沙皇" },
		{ "bard", "星界游神", "巴德", "游神" },
		{ "blitzcrank", "蒸汽机器人", "布里茨", "机器人" },
		{ "brand", "复仇焰魂", "布兰德", "火男" },
		{ "braum", "弗雷尔卓德之心", "布隆" },
		{ "caitlyn", "皮城女警", "凯特琳", "女警" },
		{ "camille", "青钢影", "卡蜜尔", "卡密尔", "青刚影" },
		{ "cassiopeia", "魔蛇之拥", "卡西奥佩娅", "蛇女", "蛇" },
		{ "chogath", "虚空恐惧", "科加斯", "大虫子", "大虫", "虫子" },
		{ "corki", "英勇投弹手", "库奇", "飞机" },
		{ "darius", "诺克萨斯之手", "德莱厄斯", "诺手", "诺克", "小学生之手"},
		{ "diana", "皎月女神", "戴安娜", "皎月", "月女" },
		{ "draven", "荣耀行刑官", "德莱文" },
		{ "drmundo", "祖安狂人", "蒙多" },
		{ "ekko", "时间刺客", "艾克" },
		{ "elise", "蜘蛛女皇", "伊莉斯", "蜘蛛", "伊利斯" },
		{ "evelynn", "痛苦之拥", "伊芙琳", "寡妇" },
		{ "ezreal", "探险家", "伊泽瑞尔", "ez", "EZ" },
		{ "fiddlesticks", "远古恐惧", "费德提克", "稻草人" },
		{ "fiora", "无双剑姬", "菲奥娜", "剑姬" },
		{ "fizz", "潮汐海灵", "菲兹", "小鱼人" },
		{ "galio", "正义巨像", "加里奥", "巨像" },
		{ "gangplank", "海洋之灾", "普朗克", "船长" },
		{ "garen", "德玛西亚之力", "盖伦", "德玛" },
		{ "gnar", "迷失之牙", "纳尔" },
		{ "gragas", "酒桶", "古拉加斯" },
		{ "graves", "法外狂徒", "格雷福斯" },
		{ "hecarim", "战争之影", "" },
		{ "heimerdinger", "大发明家", "" },
		{ "illaoi", "海兽祭司", "" },
		{ "irelia", "刀锋舞者", "" },
		{ "ivern", "翠神", "" },
		{ "janna", "风暴之怒", "" },
		{ "jarvaniv", "德玛西亚皇子", "" },
		{ "jax", "武器大师", "" },
		{ "jayce", "未来守护者", "" },
		{ "jhin", "戏命师", "" },
		{ "jinx", "暴走萝莉", "" },
		{ "kaisa", "虚空之女", "" },
		{ "kalista", "复仇之矛", "" },
		{ "karma", "天启者", "" },
		{ "karthus", "死亡颂唱者", "" },
		{ "kassadin", "虚空行者", "" },
		{ "katarina", "不祥之刃", "" },
		{ "kayle", "正义天使", "" },
		{ "kayn", "影流之镰", "" },
		{ "kennen", "狂暴之心", "" },
		{ "khazix", "虚空掠夺者", "" },
		{ "kindred", "永猎双子", "" },
		{ "kled", "暴怒骑士", "" },
		{ "kogmaw", "深渊巨口", "" },
		{ "leblanc", "诡术妖姬", "" },
		{ "leesin", "盲僧", "" },
		{ "leona", "曙光女神", "" },
		{ "lissandra", "冰霜女巫", "" },
		{ "lucian", "圣枪游侠", "" },
		{ "lulu", "仙灵女巫", "" },
		{ "lux", "光辉女郎", "" },
		{ "malphite", "熔岩巨兽", "" },
		{ "malzahar", "虚空先知", "" },
		{ "maokai", "扭曲树精", "" },
		{ "masteryi", "无极剑圣", "" },
		{ "missfortune", "赏金猎人", "" },
		{ "monkeyking", "齐天大圣", "" },
		{ "mordekaiser", "铁铠冥魂", "" },
		{ "morgana", "堕落天使", "" },
		{ "nami", "唤潮鲛姬", "" },
		{ "nasus", "沙漠死神", "" },
		{ "nautilus", "深海泰坦", "" },
		{ "neeko", "万花通灵", "" },
		{ "nidalee", "狂野女猎手", "" },
		{ "nocturne", "永恒梦魇", "" },
		{ "nunu", "雪原双子", "" },
		{ "olaf", "狂战士", "" },
		{ "orianna", "发条魔灵", "" },
		{ "ornn", "山隐之焰", "" },
		{ "pantheon", "不屈之枪", "" },
		{ "poppy", "圣锤之毅", "" },
		{ "pyke", "血港鬼影", "" },
		{ "qiyana", "元素女皇", "" },
		{ "quinn", "德玛西亚之翼", "" },
		{ "rakan", "幻翎", "" },
		{ "rammus", "披甲龙龟", "" },
		{ "reksai", "虚空遁地兽", "" },
		{ "renekton", "荒漠屠夫", "" },
		{ "rengar", "傲之追猎者", "" },
		{ "riven", "放逐之刃", "" },
		{ "rumble", "机械公敌", "" },
		{ "ryze", "符文法师", "" },
		{ "sejuani", "北地之怒", "" },
		{ "senna", "涤魂圣枪", "" },
		{ "sett", "腕豪", "" },
		{ "shaco", "恶魔小丑", "" },
		{ "shen", "暮光之眼", "" },
		{ "shyvana", "龙血武姬", "" },
		{ "singed", "炼金术士", "" },
		{ "sion", "亡灵战神", "" },
		{ "sivir", "战争女神", "" },
		{ "sona", "琴瑟仙女", "" },
		{ "soraka", "众星之子", "" },
		{ "swain", "诺克萨斯统领", "" },
		{ "sylas", "解脱者", "" },
		{ "syndra", "暗黑元首", "" },
		{ "tahmkench", "河流之王", "" },
		{ "taliyah", "岩雀", "" },
		{ "talon", "刀锋之影", "" },
		{ "taric", "瓦洛兰之盾", "" },
		{ "teemo", "迅捷斥候", "" },
		{ "thresh", "魂锁典狱长", "" },
		{ "tristana", "麦林炮手", "" },
		{ "trundle", "巨魔之王", "" },
		{ "tryndamere", "蛮族之王", "" },
		{ "twistedfate", "卡牌大师", "" },
		{ "twitch", "瘟疫之源", "" },
		{ "udyr", "兽灵行者", "" },
		{ "urgot", "无畏战车", "" },
		{ "varus", "惩戒之箭", "" },
		{ "vayne", "暗夜猎手", "" },
		{ "veigar", "邪恶小法师", "" },
		{ "velkoz", "虚空之眼", "" },
		{ "vi", "皮城执法官", "" },
		{ "viktor", "机械先驱", "" },
		{ "vladimir", "猩红收割者", "" },
		{ "volibear", "雷霆咆哮", "" },
		{ "warwick", "祖安怒兽", "" },
		{ "xayah", "逆羽", "" },
		{ "xerath", "远古巫灵", "" },
		{ "xinzhao", "德邦总管", "" },
		{ "yasuo", "疾风剑豪", "" },
		{ "yorick", "牧魂人", "" },
		{ "yuumi", "魔法猫咪", "" },
		{ "zac", "生化魔人", "" },
		{ "zed", "影流之主", "" },
		{ "ziggs", "爆破鬼才", "" },
		{ "zilean", "时光守护者", "" },
		{ "zoe", "暮光星灵", "" },
		{ "zyra", "荆棘之兴", "" },
	};

	const string opgg_program_name = "opgg.py";

	enum class POSITION {TOP, JNG, MID, BOT, SUP}; // 上单, 打野, 中单, 下路, 辅助

	string get_tier_list(vector<int> tiers) {
		
	}

	string get_champion_tier(string name) {
		
	}

	string opgg_handle(string s){
	
	}

	class opgg_reply : virtual public custom_reply { // OEIS查询，后面再接一个url最好

	private:

	public:
		opgg_reply(bool at = false) : custom_reply(at) {}

		~oeis_reply() = 0 {};

		string reply(const MessageEvent&,const Target &u) {
			string p = urlencode(e.message);
			logging::info("debug", p);
			ofstream f("opgg.txt");
			f << p;
			f.close();
			system(("python " + opgg_program_name + " < opgg.txt").c_str());

			string u, s, t;
			ifstream g("opgg.txt");
			int k = 5;
			u = "以下是前" + to_string(k) + "个搜索结果：";
			while (g >> s >> t && k--) {
				u += "\n" + s + " " + t;
			}
			g.close();
			return u;
		}
	};

}

#endif

#endif