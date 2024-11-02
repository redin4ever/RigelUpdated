#pragma once
#include "IDAdefs.h"
#define ISVALID(x) ((x) && (x) != INVALID_HANDLE_VALUE)
namespace OW {
	class HPComponent
	{
	public:
		char pad_0001[0xDC];
		Vector2 Health;
		char pad_0002[0x21C];
		Vector2 Armor;
		char pad_0003[0x35C];
		Vector2 Barrier;

		Vector2 GetHealth()
		{
			Vector2 HP = SDK->RPM<Vector2>((uint64_t)&this->Health);
			Vector2 Ar = SDK->RPM<Vector2>((uint64_t)&this->Armor);
			Vector2 Br = SDK->RPM<Vector2>((uint64_t)&this->Barrier);
			//	HP.X = Max , HP.Y = Current HP
			return { HP.X + Ar.X + Br.X ,HP.Y + Ar.Y + Br.Y };
		}
	};
	struct health_compo_t {
		union {
			OFF_MEMB(float, health, 0xE0);
			OFF_MEMB(float, health_max, 0xDC);
			OFF_MEMB(float, armor, 0x220);
			OFF_MEMB(float, armor_max, 0x21C);
			OFF_MEMB(float, barrier, 0x360);
			OFF_MEMB(float, barrier_max, 0x35C);
			OFF_MEMB(bool, isImmortal, 0x4A9);
			OFF_MEMB(bool, isBarrierProjected, 0x4A8);
		};
	};
	struct obj_compo_t {
		union {
			OFF_MEMB(XMFLOAT3, obj_pos, 0xE0);
		};
	};
	struct velocity_compo_t {
		union {
			OFF_MEMB(XMFLOAT3, velocity, 0x50);
			OFF_MEMB(XMFLOAT3, location, 0x1B0 + 0x50);
			OFF_MEMB(uint64_t, bonedata, 0x8B0);
		};
	};
	struct hero_compo_t {
		union {
			OFF_MEMB(uint64_t, heroid, 0xE0);
		};
	};
	struct vis_compo_t {
		union {
			OFF_MEMB(uint64_t, key1, 0xA0);
			OFF_MEMB(uint64_t, key2, 0x98);
		};
	};

	inline std::string GetHeroNames(uint64_t HeroID, uint64_t LinkBase) {
		switch (HeroID)
		{
		case eHero::HERO_REAPER:
			return (u8"ËÀÉñ");
		case eHero::HERO_TRACER:
			return (u8"ÁÔ¿Õ");
		case eHero::HERO_MERCY:
			return (u8"ÌìÊ¹");
		case eHero::HERO_HANJO:
			return (u8"°ë²Ø");
		case eHero::HERO_TORBJORN:
			return (u8"ÍÐ±È°º");
		case eHero::HERO_REINHARDT:
			return (u8"À³Òò¹þÌØ");
		case eHero::HERO_PHARAH:
			return (u8"·¨ÀÏÖ®Ó¥");
		case eHero::HERO_WINSTON:
			return (u8"ÎÂË¹¶Ù");
		case eHero::HERO_WIDOWMAKER:
			return (u8"ºÚ°ÙºÏ");
		case eHero::HERO_BASTION:
			return (u8"±¤ÀÝ");
		case eHero::HERO_SYMMETRA:
			return (u8"ÖÈÐòÖ®¹â");
		case eHero::HERO_ZENYATTA:
			return (u8"ìøÑÅËþ");
		case eHero::HERO_GENJI:
			return (u8"Ô´ÊÏ");
		case eHero::HERO_ROADHOG:
			return (u8"Â·°Ô");
		case eHero::HERO_MCCREE:
			return (u8"Âó¿ËÀ×");
		case eHero::HERO_JUNKRAT:
			return (u8"¿ñÊó");
		case eHero::HERO_ZARYA:
			return (u8"²éÀòæ«");
		case eHero::HERO_SOLDIER76:
			return (u8"Ê¿±ø76");
		case eHero::HERO_LUCIO:
			return (u8"Â¬Î÷°Â");
		case eHero::HERO_DVA:
			if (SDK->RPM<uint16_t>(LinkBase + 0xD4) != SDK->RPM<uint16_t>(LinkBase + 0xD8))
				return (u8"D.Va");
			else
				return (u8"¹þÄÈ");
		case eHero::HERO_VENTURE:
			return (u8"Â¬Î÷°");
		case eHero::HERO_MEI:
			return (u8"ÃÀ");
		case eHero::HERO_ANA:
			return (u8"°²ÄÈ");
		case eHero::HERO_SOMBRA:
			return (u8"ºÚÓ°");
		case eHero::HERO_ORISA:
			return (u8"°ÂÀöÉ¯");
		case eHero::HERO_DOOMFIST:
			return (u8"Ä©ÈÕÌúÈ ");
		case eHero::HERO_MOIRA:
			return (u8"ÄªÒÁÀ ");
		case eHero::HERO_BRIGITTE:
			return (u8"²¼Àï¼ªËþ");
		case eHero::HERO_WRECKINGBALL:
			return (u8"ÆÆ»µÇò");
		case eHero::HERO_SOJOURN:
			return (u8"Ë÷½Ü¶÷");
		case eHero::HERO_ASHE:
			return (u8"°¬Ê²");
		case eHero::HERO_BAPTISTE:
			return (u8"°ÍµÙË¹ÌØ");
		case eHero::HERO_KIRIKO:
			return (u8"Îí×Ó");
		case eHero::HERO_JUNKERQUEEN:
			return (u8"Ôü¿ÍÅ®Íõ");
		case eHero::HERO_SIGMA:
			return (u8"Î÷¸ñÂê");
		case eHero::HERO_ECHO:
			return (u8"»ØÉù");
		case eHero::HERO_RAMATTRA:
			return (u8"À ÂêÉ²");
		case eHero::HERO_TRAININGBOT1:
			return (u8"±ê×¼»úÆ÷ÈË");
		case eHero::HERO_TRAININGBOT2:
			return (u8"ÖØ×°»úÆ÷ÈË");
		case eHero::HERO_TRAININGBOT3:
			return (u8"¾Ñ»÷»úÆ÷ÈË");
		case eHero::HERO_TRAININGBOT4:
			return (u8"ÓÑ·½»úÆ÷ÈË");
		case eHero::HERO_TRAININGBOT5:
			return (u8"ÓÑ·½ÖØ×°»úÆ÷ÈË");
		case eHero::HERO_TRAININGBOT6:
			return (u8"»ð¼ý»úÆ÷ÈË");
		case eHero::HERO_TRAININGBOT7:
			return (u8"ÑµÁ·»úÆ÷ÈË");
		case eHero::HERO_LIFEWEAVER:
			return (u8"ÉúÃüÖ®Ëó");
		case eHero::TOBTERT:
			return (u8"ÅÚÌ¨");
		case eHero::SYMTERT:
			return (u8"ÉãÏñÍ·");
		case eHero::HERO_ILLARI:
			return (u8"ÒÁÀ Èñ");
		case eHero::HERO_MAUGA:
			return (u8"Ã«¼Ó");
		case eHero::Bob:
			return (u8"±«²ª");
		default:
			return (u8"Î´Öª");
		}

	}

	inline uint64_t GetParent(uint64_t encrypted) {
		__try {
			auto result = encrypted;
			result -= 0x401C60913E3B91CE;
			result = (result >> 0x20) | (result << 0x20);
			return result;
		}
		__except (1) {

		}
	}

	inline std::string GetHeroEngNames(uint64_t HeroID, uint64_t LinkBase) {
		switch (HeroID)
		{
		case eHero::HERO_JUNO:
			return (u8"Juno");
		case eHero::HERO_REAPER:
			return ("Reaper");
		case eHero::HERO_TRACER:
			return ("Tracer");
		case eHero::HERO_MERCY:
			return ("Mercy");
		case eHero::HERO_HANJO:
			return ("Hanzo");
		case eHero::HERO_TORBJORN:
			return ("Torbjorn");
		case eHero::HERO_REINHARDT:
			return ("Reinhardt");
		case eHero::HERO_PHARAH:
			return ("Pharah");
		case eHero::HERO_WINSTON:
			return ("Winston");
		case eHero::HERO_WIDOWMAKER:
			return ("Widowmaker");
		case eHero::HERO_BASTION:
			return ("Bastion");
		case eHero::HERO_SYMMETRA:
			return ("Symmetra");
		case eHero::HERO_ZENYATTA:
			return ("Zenyatta");
		case eHero::HERO_GENJI:
			return ("Genji");
		case eHero::HERO_ROADHOG:
			return ("Roadhog");
		case eHero::HERO_MCCREE:
			return ("McCree");
		case eHero::HERO_JUNKRAT:
			return ("Junkrat");
		case eHero::HERO_ZARYA:
			return ("Zarya");
		case eHero::HERO_SOLDIER76:
			return ("Soldier 76");
		case eHero::HERO_LUCIO:
			return ("Lucio");
		case eHero::HERO_DVA:
			if (SDK->RPM<uint16_t>(LinkBase + 0xD4) != SDK->RPM<uint16_t>(LinkBase + 0xD8))
				return ("D.Va");
			else
				return ("Hana");
		case eHero::HERO_MEI:
			return ("Mei");
		case eHero::HERO_ANA:
			return ("Ana");
		case eHero::HERO_SOMBRA:
			return ("Sombra");
		case eHero::HERO_ORISA:
			return ("Orisa");
		case eHero::HERO_DOOMFIST:
			return ("Doomfist");
		case eHero::HERO_MOIRA:
			return ("Moira");
		case eHero::HERO_BRIGITTE:
			return ("Brigitte");
		case eHero::HERO_WRECKINGBALL:
			return ("Wrecking Ball");
		case eHero::HERO_SOJOURN:
			return ("Sojourn");
		case eHero::HERO_ASHE:
			return ("Ashe");
		case eHero::HERO_BAPTISTE:
			return ("Baptiste");
		case eHero::HERO_KIRIKO:
			return ("Kiriko");
		case eHero::HERO_JUNKERQUEEN:
			return ("Junker Queen");
		case eHero::HERO_SIGMA:
			return ("Sigma");
		case eHero::HERO_ECHO:
			return ("Echo");
		case eHero::HERO_RAMATTRA:
			return ("Ramattra");
		case eHero::HERO_TRAININGBOT1:
			return ("Standard Bot");
		case eHero::HERO_TRAININGBOT2:
			return ("Tank Bot");
		case eHero::HERO_TRAININGBOT3:
			return ("Sniper Bot");
		case eHero::HERO_TRAININGBOT4:
			return ("Friend Bot");
		case eHero::HERO_TRAININGBOT5:
			return (u8"Friend Tank Bot");
		case eHero::HERO_TRAININGBOT6:
			return (u8"Rocket Bot");
		case eHero::HERO_TRAININGBOT7:
			return (u8"Training Bot");
		case eHero::HERO_LIFEWEAVER:
			return ("LifeWeaver");
		case eHero::TOBTERT:
			return (u8"Tob");
		case eHero::SYMTERT:
			return (u8"Sym");
		case eHero::HERO_ILLARI:
			return (u8"Illari");
		case eHero::HERO_MAUGA:
			return (u8"Mauga");
		case eHero::HERO_VENTURE:
			return (u8"Venture");
		case eHero::Bob:
			return (u8"Bob");
		default:
			return (u8"Unknown");
		}
	}
	// decryptcomp sig: 48 89 5c 24 ? 48 89 6c 24 ? 56 57 41 56 48 83 ec ? 49 8b f1 49 8b e8

	inline uintptr_t DecryptComponent(uint64_t Parent, uint8_t Type)
	{
		__try {
			if (Parent) {
				const uint64_t a1 = Parent;

				const uint64_t v16 = 1ui64 << (uint64_t)(Type & 0x3F);
				const uint64_t v15 = v16 - 1ui64;
				const uint32_t v13 = Type & 0x3F;
				const uint32_t v17 = Type / 0x3F;

				const uint64_t v8 = SDK->RPM<uint64_t>(a1 + 8i64 * v17 + 0x110);
				const uint64_t v9 = (v15 & v8) - (((v15 & v8) >> 1) & 0x5555555555555555LL);

				uint64_t v14 = __ROL8__(
					__ROL8__(
						SDK->RPM<uint64_t>(SDK->RPM<uint64_t>(a1 + 0x80)
							+ 8
							* (SDK->RPM<uint8_t>(v17 + a1 + 0x130)
								+ ((0x101010101010101i64
									* (((v9 & 0x3333333333333333i64)
										+ ((v9 >> 2) & 0x3333333333333333i64)
										+ (((v9 & 0x3333333333333333LL) + ((v9 >> 2) & 0x3333333333333333i64)) >> 4)) & 0xF0F0F0F0F0F0F0Fi64)) >> 0x38))) ^ SDK->RPM<uint8_t>(SDK->dwGameBase + 0x31BFE18) ^ 0x5952EC877E8DAF4i64,
						10)
					- 0x4A32D540542847AAi64,
					11)
					- 0x11B49508F72CEF96i64;

				v14 = __ROL8__(v14, 19);

				const uint64_t v10 = (SDK->RPM<uint64_t>(SDK->RPM<uint64_t>(SDK->dwGameBase + 0x348A650) + 0xD7) ^ v14) & (int32_t)(-(int64_t)((v16 & v8) >> v13));

				return v10;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return NULL;
		}
		return NULL;
	}
 


	inline unsigned __int64 __fastcall DecryptVis(unsigned __int64 a1)
	{
		auto v2 = SDK->RPM<uint64_t>(a1 + 0x98);
		return v2 = 0;
	}

	enum class HeapManager : uintptr_t
	{
		HeapManagerInstance = 0x3882880,
		HeapManagerXor = 0x38672D3,
		HeapManagerKey = 0x6DE0CEEB548519C3,
		HeapManagerPointer = 0x160,
	};
	inline uintptr_t GetHeapManager(uint8_t index) {
		uintptr_t v0 = SDK->RPM<uintptr_t>(SDK->dwGameBase + 0x34A4530);
		if (v0 != 0) {
			uintptr_t v1 = (__ROL8__(
				__ROR8__(SDK->RPM<_QWORD>(v0 + 0x160) + 0x133A2F0FA05E59FLL, 31) + 0x222D7F42D73446F7LL,
				15) ^ 0xF5EF035545B5E77DuLL);
			if (v1 != 0) {
				uintptr_t v2 = SDK->RPM<uintptr_t>(v1 + 0x8 * index);
				return v2;
			}
		}
		return NULL;
	}

	inline uintptr_t GetSenstivePTR() {
		uintptr_t heap = GetHeapManager(6);
		if (heap) {
			return heap + offset::SensitivePtr;
		}
		return NULL;
	}

	inline std::vector<std::pair<uint64_t, uint64_t>> get_ow_entities()
	{
		std::vector<std::pair<uint64_t, uint64_t>> result;

		struct Entity {
			uint64_t entity;
			uint64_t pad;
		};

		uintptr_t entity_list = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_entity_base);

		MEMORY_BASIC_INFORMATION mbi{};
		VirtualQueryEx(SDK->hProcess, (LPCVOID)entity_list, &mbi, sizeof(mbi));

		SIZE_T entity_list_size = mbi.RegionSize / sizeof(Entity);
		Entity* raw_list = new Entity[entity_list_size];

		if (ReadProcessMemory(SDK->hProcess, (LPCVOID)entity_list, raw_list, mbi.RegionSize, nullptr))
		{
			for (size_t i = 0; i < entity_list_size; ++i)
			{
				uint64_t cur_entity = raw_list[i].entity;
				if (cur_entity)
				{
					uint64_t common_linker = DecryptComponent(cur_entity, TYPE_LINK);
					//printf("LINK:%llx\n", common_linker);
					if (common_linker)
					{
						uint32_t unique_id = SDK->RPM<uint32_t>(common_linker + 0xD4);
						//printf("unique_id:%llx\n", unique_id);	
						//printf("linker:%llx\n", common_linker);
						for (size_t x = 0; x < entity_list_size; ++x)
						{
							uint64_t possible_common = raw_list[x].entity;
							if (possible_common && SDK->RPM<uint32_t>(possible_common + 0x138) == unique_id)
							{
								//printf("%llx", unique_id);
								result.emplace_back(possible_common, cur_entity);
								break;
							}
							else {
								uint64_t Ptr = SDK->RPM<uint64_t>(possible_common + 0x30) & 0xFFFFFFFFFFFFFFC0;
								if (Ptr < 0xFFFFFFFFFFFFFFEF) {
									uint64_t EntityID = SDK->RPM<uint64_t>(Ptr + 0x10);
									if (EntityID == 0x400000000000060 || EntityID == 0x40000000000480A || EntityID == 0x40000000000005F || EntityID == 0x400000000002533) {
										result.emplace_back(possible_common, cur_entity);
									}
								}
							}
						}
					}
				}
			}
		}
		delete[] raw_list;
		return result;
	}

	inline bool IsSkillActive(uint64_t base, uint16_t index, uint16_t id)
	{
		if (id == 0)
			return false;
		uintptr_t skillList = 0;
		if (index == 0)
		{
			// Use the default skill list for index 0
			skillList = base + 0x570;
		}
		else
		{
			// Get the skill list for the given index
			uint32_t count = SDK->RPM<uint32_t>(base + 0x378);
			if (count <= 0 || count >= 0xFF)
				return false;
			uintptr_t entry = SDK->RPM<uintptr_t>(base + 0x370);
			if (!entry)
				return false;
			for (uint32_t i = 0; i < count; i++, entry += 0x10)
			{
				if (SDK->RPM<uint16_t>(entry + 0x8) == index)
				{
					uintptr_t listStruct = SDK->RPM<uintptr_t>(entry);
					if (!listStruct)
						return false;
					skillList = SDK->RPM<uintptr_t>(listStruct + 0xA8);
					break;
				}
			}
		}
		if (!skillList)
			return false;
		// Get the skill entry from the list using the skill id
		uintptr_t listEntry = skillList + 0x20 * ((id & 0xF) + 1);
		uintptr_t structList = SDK->RPM<uintptr_t>(listEntry);
		if (!structList)
			return false;

		int32_t listIndex = index == 0 ? 0 : SDK->RPM<int32_t>(listEntry + 0x8) - 1;
		if (listIndex < 0 || listIndex >= 0xFF)
			return false;

		uintptr_t skillEntry = structList + 0x10 * listIndex;
		if (SDK->RPM<uint16_t>(skillEntry) == id)
		{
			uintptr_t skill = SDK->RPM<uintptr_t>(skillEntry + 0x8);
			if (!skill)
				return false;

			return  SDK->RPM<uint8_t>(skill + 0x48) == 1;
		}
		return false;
	}

	inline float SkillCD(uint64_t base, uint16_t index, uint16_t id)
	{
		if (id == 0)
			return false;

		uintptr_t skillList = 0;
		if (index == 0)
		{
			// Use the default skill list for index 0
			skillList = base + 0x570;
		}
		else
		{
			// Get the skill list for the given index
			uint32_t count = SDK->RPM<uint32_t>(base + 0x378);
			if (count <= 0 || count >= 0xFF)
				return false;

			uintptr_t entry = SDK->RPM<uintptr_t>(base + 0x370);
			if (!entry)
				return false;

			for (uint32_t i = 0; i < count; i++, entry += 0x10)
			{
				if (SDK->RPM<uint16_t>(entry + 0x8) == index)
				{
					uintptr_t listStruct = SDK->RPM<uintptr_t>(entry);
					if (!listStruct)
						return false;

					skillList = SDK->RPM<uintptr_t>(listStruct + 0xA8);
					break;
				}
			}
		}

		if (!skillList)
			return false;

		// Get the skill entry from the list using the skill id
		uintptr_t listEntry = skillList + 0x20 * ((id & 0xF) + 1);
		uintptr_t structList = SDK->RPM<uintptr_t>(listEntry);
		if (!structList)
			return false;

		int32_t listIndex = SDK->RPM<int32_t>(listEntry + 0x8) - 1;
		if (listIndex < 0 || listIndex >= 0xFF)
			return false;

		uintptr_t skillEntry = structList + 0x10 * listIndex;
		if (SDK->RPM<uint16_t>(skillEntry) == id)
		{
			uintptr_t skill = SDK->RPM<uintptr_t>(skillEntry + 0x8);
			if (!skill)
				return false;

			return  SDK->RPM<float>(skill + 0x60);
		}
		return false;
	}

	inline uintptr_t SkillStructCheck(uint64_t a1, uint16_t a2)
	{
		__int64 v2; // r8
		__int64 i; // rax

		if (SDK->RPM<uint32_t>(a1 + 0x2A8) <= 0)
			return 0i64;
		v2 = 0i64;
		for (i = SDK->RPM<uintptr_t>(a1 + 0x2A0); SDK->RPM<uint16_t>(i + 8) != a2; i += 16i64)
		{
			if (++v2 >= SDK->RPM<uint32_t>(a1 + 0x2A8))
				return 0i64;
		}
		return SDK->RPM<uintptr_t>(i);
	}

	inline uint64_t FnSkillStruct(__m128* a1, uint16_t* a2)
	{

		__int64 v2; // rbx
		uint16_t* v3;
		__int16 v4; // dx
		__int64 v5; // rcx
		__int64 v6; // r9
		__int64 v7; // rax
		unsigned __int16 v8; // r8
		__int64 v9; // rdx
		int v10; // eax
		__int64 v11; // rcx
		__int64 v12; // rax
		__int64 v13; // rdi

		v2 = 0i64;
		v3 = a2;
		if (!a2[1])
			return 0i64;
		v4 = *a2;
		v5 = a1->m128_i64[1];
		if (!v4)
		{
			v6 = v5 + 0x4A0;
			goto LABEL_6;
		}
		v7 = SkillStructCheck(v5, v4);
		if (!v7)
			return 0i64;
		v6 = SDK->RPM<uintptr_t>(v7 + 168);
	LABEL_6:
		v8 = v3[1];
		v9 = 32 * ((v3[1] & 0xF) + 1i64);
		v10 = SDK->RPM<uint32_t>(v9 + v6 + 8) - 1;
		if (v10 < 0)
			return 0x0;
		v11 = v10;
		v12 = SDK->RPM<uintptr_t>(v9 + v6) + 16i64 * v10;
		while (SDK->RPM<uint16_t>(v12) != v8)
		{
			v12 -= 16i64;
			if (--v11 < 0)
				return 0x0;
		}
		v13 = SDK->RPM<uintptr_t>(v12 + 8);
		if (!v13)
			return 0x0;

		if (*((uint32_t*)v3 + 4) <= 0)
			return v13;

		return v13;
	}

	inline bool IsSkillActivate1(uint64_t base, uint16_t skillIdx, uint16_t skillIdx2)
	{
		__m128 skillStruct{};
		uint16_t skillId[15] = { skillIdx, skillIdx2 };
		skillStruct.m128_u64[1] = base + 0xD0;
		uint64_t skill = FnSkillStruct(&skillStruct, skillId);
		bool ret = SDK->RPM<uint8_t>(skill + 0x48) == 1;
		if (!skill)
			return false;
		return ret;
	}

	inline float readskillcd(uint64_t base, uint16_t skillIdx, uint16_t skillIdx2)
	{
		__m128 skillStruct{};
		uint16_t skillId[15] = { skillIdx, skillIdx2 };
		skillStruct.m128_u64[1] = base + 0xD0;
		uint64_t skill = FnSkillStruct(&skillStruct, skillId);
		if (!skill)
			return false;
		float ret = SDK->RPM<float>(skill + 0x48);
		if (!ret) return ret;
		ret = SDK->RPM<float>(skill + 0x60);
		if (ret != 0)
			return ret;
		else return 1;
	}

	inline float readult(uint64_t base, uint16_t skillIdx, uint16_t skillIdx2)
	{
		__m128 skillStruct{};
		uint16_t skillId[15] = { skillIdx, skillIdx2 };
		skillStruct.m128_u64[1] = base + 0xD0;
		uint64_t skill = FnSkillStruct(&skillStruct, skillId);
		if (!skill)
			return false;
		float ret = SDK->RPM<float>(skill + 0x60);
		return ret;
	}

	inline float cdready(uint64_t base, uint16_t skillIdx, uint16_t skillIdx2)
	{
		__m128 skillStruct{};
		uint16_t skillId[15] = { skillIdx, skillIdx2 };
		skillStruct.m128_u64[1] = base + 0xD0;
		uint64_t skill = FnSkillStruct(&skillStruct, skillId);
		if (!skill)
			return false;
		float ret = SDK->RPM<float>(skill + 0x48);
		return ret;
	}


	uintptr_t GetOutlineStruct(uintptr_t a1)
	{
		__try
		{
			uint64_t result = NULL;
			int v1 = SDK->RPM<uint32_t>(a1 + 0x68);
			if (v1 <= 0)
				result = 0;
			else
				result = (uint64_t)(0x20 * v1 + SDK->RPM<uint64_t>(a1 + 0x60) - 0x20);

			return result;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}

		return NULL;
	}


	uint64_t GetThickNessStruct(uint64_t a1)
	{
		__try
		{
			uint64_t result = NULL;
			int v1 = SDK->RPM<uint32_t>(a1 + 0x1C8);
			if (v1 <= 0)
				result = 0;
			else
				result = 1;

			return result;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}

		return NULL;
	}
	inline uint64_t DecryptOutline(uint64_t a1)
	{
		uint64_t result = NULL;
		int v1 = SDK->RPM<uint32_t>(a1);
		return v1 = 0;
	}

	inline void SetBorderLine(uint32_t BorderType, DWORD_PTR base)
	{
		uint64_t BorderStruct = GetOutlineStruct((uint64_t)(base + 0x20)); // 0x5A Components + 0x20
		uint64_t DecryptData = DecryptOutline(SDK->RPM<uint64_t>(BorderStruct + 0x18));
		SDK->WPM<uint32_t>(BorderStruct + 0x8, (DecryptData ^ BorderType));
		SDK->WPM<uint32_t>(BorderStruct + 0x10, (DecryptData ^ BorderType));	
	}
}
