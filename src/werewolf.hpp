#ifndef WEREWOLF_HPP
#define WEREWOLF_HPP

//*****************************************
//*******狼人杀助手，方便起见仅在私聊中可用*******
//*****************************************

namespace definations {
	enum class TIME { DAY, NIGHT };
	
	enum class STEP { ELECTION, DISCUSSING, VOTE, NIGHT };

	enum class TYPE { UNKNOWN,
		VILLAGER, SEER, WITCH, HUNTER, FOOL, GUARDIAN, EXECUTIONER, KNIGHT // 人 / 神
	//	  平民    预言家  女巫     猎人   白痴     守卫        猎魔人      骑士

		WOLF, BLACKWOLF, WHITEWOLF, NIGHTMARE, WOLFBEAUTY, HIDDENWOLF // 狼
	//	 狼人    狼王       白狼王       梦魇       狼美人        隐狼

		CUPID, THIEF // 第三方
	//	丘比特   盗贼
	};

	enum class GROUP { JUSTICE, EVIL, THIRD }; // 阵营

	enum class DEATH { KILLED, SHOT, POISONED, VOTED }; // 怎么死的

	class charactor {

	protected:
		int order;
		TYPE type, public_type;
		bool dead;

	public:
		charactor() : order(0), dead(false), public_type(UNKNOWN) {}

		virtual ~charactor() = 0 {};

		void set_order(int k) {
			if (!order)
				order = k;
		}

		virtual void death(DEATH) = 0;

		TYPE get_type() const {
			return type;
		}

		TYPE get_public_type() const {
			return public_type;
		}

		bool is_dead() const {
			return dead;
		}

		virtual bool check() const = 0; // 被预言家查验，true为好人，false为坏人

		virtual bool check_ability() const = 0;

		virtual void ability() const = 0;
	};

	class villager final : public charactor {
		
	public:
		villager() : charactor() {}

		~villager() = default;

		void death(DEATH t) {
			dead = true; //　平民只能乖乖去死
		}

		bool check() const {
			return true;
		}
	};

	class seer final : public charactor {
		
	public:
		seer() : charactor() {}

		~seer() = default;

		void death(DEATH t) {
			dead = true; // 预言家也只能乖乖去死
		}

		bool check() {
			return true; // 问题是哪个预言家会查自己啊？
		}
	};

	class witch final : public charactor {
		
	protected:
		bool poison_used, antidote_used; // 女巫的两瓶药 

	public:
		witch() : charactor() , poison_used(false), antidote_used(false) {}

		~witch() = default;

		void death(DEATH t) {
			dead = true;
		}

		bool check() const {
			return true;
		}
	};

	class hunter final : public charactor {
		
	public:
		hunter() : charactor() {}

		~hunter() = default;
		
		void death(DEATH t) {
			if (t != POISONED) {
				// TODO：大哥打谁？
			}
			else {
				// TODO：小老弟你被毒了
			}
		}

		bool check() const {
			return true;
		}
	};

	class fool final : public charactor {
		
	protected:
		bool voted; // 是否已翻牌

	public:
		fool() : charactor(), voted(false) {}

		~fool() = default;

		void death(DEATH t) {
			if (t == VOTED) {
				// TODO：青春期中二病白神也想翻牌
			}
		}
	};

}

#endif