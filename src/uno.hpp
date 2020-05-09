#ifndef UNO_HPP
#define UNO_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>

#include "string_tools.hpp"
#include "message_tools.hpp"
#include "checker_and_handler.hpp"

namespace uno {

	using namespace std;

	enum CARD_COLOR {COLOR_R = 0, COLOR_G = 1, COLOR_B = 2, COLOR_Y = 3, COLOR_N = 4}; // N:None，打出前为无颜色，打出后须指定颜色
	const string color_str[] = { "红色", "绿色", "蓝色", "黄色", "万能" };
	const vector<string> check_color[] = {
		{"红色", "红", "red", "R", "r"},
		{"绿色", "绿", "green", "G", "g"},
		{"蓝色", "蓝", "blue", "B", "b"},
		{"黄色", "黄", "yellow", "Y", "y"}
	};

	enum CARD_TYPE {TYPE_NUM = 0, TYPE_BAN = 1, TYPE_REV = 2, TYPE_DRW = 3, TYPE_CHG = 4};
	const string type_str[] = {"", "跳过", "反转", "+", "换色"};
	const vector<string> check_type[] = {
		{""},
		{"跳过", "禁止", "ban", "禁"},
		{"反转", "翻转", "反向", "逆"},
		{"+", "加", "罚抽", "抽", "摸"},
		{"换色"}
	};

	class Card {
		

		CARD_COLOR color;

		CARD_TYPE type;

		int value;

	public:
		Card(CARD_COLOR color, CARD_TYPE type, int value = 0) : color(color), type(type), value(value) {}

		CARD_COLOR get_color() const {
			return color;
		}

		bool set_color(CARD_COLOR c) {
			if (color != N)
				return false; // ???
			color = c;
			return true;
		}

		CARD_TYPE get_type() const {
			return type;
		}

		int get_value() const {
			return value;
		}

		void clear_value() {
			value = 0;
		}
		
		bool check(string s) const { // TODO：检查是否是这张牌
			if (color != COLOR_N) {
				for (auto t : check_color[color])
					if (start_with(s, t)) {
						erase_pre(s, t);
						erase_pre(s);
						erase_suf(s);

						for (auto tt : check_type[type])
							if (start_with(s, tt)) {
								erase_pre(s, tt);
								erase_pre(s);
								erase_suf(s);
								if (type == TYPE_NUM || type == TYPE_DRW)
									return s == to_string(value);
								else
									return s == "";
								return false;
							}
						return false;
					}
			}
			else {
				for (auto t : {"变色", "改色", "换色"}) {
					if (s == t)
						return type == TYPE_CHG;
				}
				for (auto t : {"+", "罚抽", "抽"})
					if (start_with(s, t)) {
						erase_pre(s, t);
						erase_pre(s);
						erase_suf(s);
						return s == to_string(value) && type == TYPE_DRW;
					}
			}
			return false;
		}

		friend bool check(const Card &a, const Card &b) {
			if (a.color == b.color)
				return true;
			if (a.type == b.type) {
				if (a.type != TYPE_DRW)
					return a.value == b.value;
				return true;
			}
			if (b.color == N)
				return true;

			return false;
		}

		friend bool operator<(const Card &a, const Card &b) {
			if (a.color != b.color)
				return a.color < b.color;
			if (a.type != b.type)
				return a.type < b.type;
			return a.value == b.value;
		}
	};

	const Card NULL_CARD(COLOR_N, TYPE_NUM);

	class Hand { // 手牌

		vector<Card> cards[5];

	public:
		Hand() {}

		~Hand() = default;

		string to_string() const {
			stringstream s;
			for (int i = 0; i < 5; i++) {
				if (i)
					s << "\n";
				s << color_str[i] << "：";
				for (const auto &c : cards[i]) {
					s << " " << type_str[c.get_type()];
					if (c.get_type() == TYPE_NUM || c.get_type() == TYPE_DRW)
						s << c.get_value();
				}
			}
			return s.str();
		}

		void sort() {
			for (int i = 0; i < 5; i++)
				sort(cards[i].begin(), cards[i].end());
		}

		Card check(string s) { // 是否能出这张牌，不能则返回NULL_CARD
			for (int i = 0; i < 5; i++)
				for (const auto &c : cards[i])
					if (c.check(s))
						return c;
			return NULL_CARD;
		}

		Card play(string s) {
			for (int i = 0; i < 5; i++)
				for (const auto &c : cards[i])
					if (c.check(s)) {
						cards[i].erase(&c);
						return c;
					}
			return NULL_CARD;
		}

		void add(const Card &c) {
			cards[c.get_color()].push_back(c);
			this->sort();
		}
	};

	class Game;

	enum PLAYER_STATUS { PLAYER_WAIT_ACTION, PLAYER_WAIT_DECIDE, PLAYER_WAIT_CHOOSE, PLAYER_COMPLETED };

	class Player { // 玩家

		int64_t user_id;

		string nickname;

		Hand hand;

		Game *game;

		bool skipped;

		PLAYER_STATUS status;

	public:
		Player() {} // todo

		int64_t get_user_id() const {
			return user_id;
		}

		string get_nickname() const {
			return nickname;
		}

		Game *get_game() const {
			return game;
		}

		bool is_skipped() const{
			return skipped;
		}

		void skip() {
			skipped = true;
		}

		Card check(string s) {
			return hand.check(s);
		}

		string draw(int cnt) { // 返回抽到的牌的字符串
			for (int i = 0; i < cnt; i++) {
				
			}
		}

		void evaluate() { // 在不接牌时处理抽牌
			int k = game->get_cnt();
			draw(k);
		}

		void action() {
			if (skipped) {
				message_tools::send_message(user_id, "你的回合被跳过了！");
				skipped = false;
			}
			status = PLAYER_WAIT_ACTION;
		}

		void play_card(Card c) {
			
		}

		void draw_card() {
			
		}

		void choose_play(bool t) {
			
		}

		void choose() { // 选择颜色
			message_tools::send_message(user_id, "选择一个颜色~");
			status = PLAYER_WAIT_CHOOSE;
		}

		void choose_color() {

		}

		void complete() { // 出完牌了
			status = PLAYER_COMPLETED;

		}
	};

	class Game { // 对局

		vector<Card> deck;
		Card last;

		vector<Player*> v; // 第一个人为拥有者
		int cur, d, cnt;
		
	public:
		Game() : last(NULL_CARD) {} // TODO:生成函数不可能是空的

		~Game() {
			for (auto o : v)
				delete o;

		}

		void send(Player *player, string s) const {
			message_tools::send_message(player->get_user_id(), s);
		}

		void send_all(string s) const {
			for (auto o : v)
				send(o, s);
		}

		void send_except(Player* player, string s) const {
			for (auto o : v)
				if (o != player)
					send(o, s);
		}

		void send_announcement(Player *player, string s) const {
			for (auto o : v)
				send(o, (player == o ? "你" : player->get_nickname()) + s);
		}

		Card draw() {
			if (deck.empty())
				return NULL_CARD;
			auto c = deck.back();
			deck.pop_back();
			return c;
		}

		void reverse() {
			d *= -1;
		}

		Card get_last() const {
			return last;
		}

		int get_cnt() const {
			return cnt;
		}

		void add_cnt(int d) {
			cnt += d;
		}

		void clear_cnt(Card c) {
			cnt = 0;
		}

		void next() { // 指向下家
			cur = (cur + d + (int)v.size()) % n;
			
		}

		void init() {
			srand(time(0));
			random_shuffle(deck.begin(), deck.end());
			
		}

		void add_player(int64_t user_id) {
			
		}

		void start() {
			cur = rand() % (int)v.size(); // 随机一个庄家
			d = 1;
		}
		
		bool started() {
			return d != 0;
		}
	};

	// ------------------------------------

	class uno_reply : public custom_reply {

	private:
		map <int, Player*> players;

	public:
		uno_reply() {}

		~uno_reply() {
			for (auto p : players)
				delete p.second;
		}
	};
}

#endif