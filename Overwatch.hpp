﻿
#pragma once
#include "Target.hpp"
#include"imgui_toggle.h"
#include"candy.h"
#include "imgui_notify.h"
#include "tahoma.h"
#include "simhei.h"
#include "imgui-knobs.h"
#include "icon.h"
#define M_PI        3.14159265358979323846f
#define M_RADPI        57.295779513082f
#define M_PI_F        ((float)(M_PI))
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#define ANGLE2SHORT(x) ((int)((x)*65536/360) & 65535)
#define SHORT2ANGLE(x)    ((x)*(360.0/65536))
#include <windows.h>
#include <vfw.h>
#include<ctime>
#include <D3DX11tex.h>
#include "vampimg.h"
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib,"vfw32.lib")

time_t expiretime;
time_t nowtime;

float WX, WY;

std::mutex mutex;
ID3D11ShaderResourceView* Image = nullptr;
ID3D11ShaderResourceView* Image2 = nullptr;
ID3D11ShaderResourceView* Image3 = nullptr;
ID3D11ShaderResourceView* Image4 = nullptr;
ID3D11ShaderResourceView* Image5 = nullptr;
namespace OW {


	inline void configsavenloadthread();
	inline void aimbot_thread();
	int abletotread=0;
	int howbigentitysize;
	inline void entity_scan_thread() {
		while (Config::doingentity == 1) {
			if (abletotread == 0) {
				ow_entities_scan = get_ow_entities();
				abletotread = 1;
			}
			Sleep(10);
		}
	}

	int entitytime = 0;
	int jumpentity = 0;
	Vector3 lastpos = { 0, 0, 0 };
	inline void entity_thread() {
		while (Config::doingentity == 1) {
			if (entitytime == 0) entitytime = GetTickCount();
			if (GetTickCount() - entitytime >= 100 && abletotread)
			{
				mutex.lock();
				ow_entities = ow_entities_scan;
				abletotread = 0;
				entitytime = GetTickCount();
				mutex.unlock();
			}
			if (!(ow_entities.size() > 0)) {
				mutex.lock();
				entities = {};
				hp_dy_entities = {};
				mutex.unlock();
				Sleep(1000);
				continue;
			}
			std::vector<c_entity> tmp_entities{};
			std::vector<hpanddy> hpdy_entities{};
			c_entity lastentity{};
			for (int i = 0; i < ow_entities.size(); i++) {
				c_entity entity{};
				if (!ow_entities[i].first || !ow_entities[i].second) continue;
				if (i > ow_entities.size()) continue;
				const auto& [ComponentParent, LinkParent] = ow_entities[i];//注意这里的&
				entity.address = ComponentParent;
				if (!entity.address) continue;
				if (!LinkParent) continue;

				uint64_t Ptr = SDK->RPM<uint64_t>(ComponentParent + 0x30) & 0xFFFFFFFFFFFFFFC0;
				if (Ptr < 0xFFFFFFFFFFFFFFEF) {
					uint64_t EntityID = SDK->RPM<uint64_t>(Ptr + 0x10);
					if (EntityID == 0x400000000000060 || EntityID == 0x40000000000480A || EntityID == 0x40000000000005F || EntityID == 0x400000000002533) {
						hpanddy hpdyentity{};
						hpdyentity.entityid = EntityID;
						hpdyentity.MeshBase = DecryptComponent(ComponentParent, TYPE_VELOCITY);
						hpdyentity.POS = SDK->RPM<XMFLOAT3>(hpdyentity.MeshBase + 0x380 + 0x50);
						hpdy_entities.push_back(hpdyentity);
						continue;
					}
				}

				entity.HealthBase = DecryptComponent(ComponentParent, TYPE_HEALTH);
				entity.LinkBase = DecryptComponent(LinkParent, TYPE_LINK);
				entity.TeamBase = DecryptComponent(ComponentParent, TYPE_TEAM);
				entity.VelocityBase = DecryptComponent(ComponentParent, TYPE_VELOCITY);
				entity.HeroBase = DecryptComponent(LinkParent, TYPE_P_HEROID);
				entity.OutlineBase = DecryptComponent(ComponentParent, TYPE_OUTLINE);
				entity.BoneBase = DecryptComponent(ComponentParent, TYPE_BONE);
				entity.RotationBase = DecryptComponent(ComponentParent, TYPE_ROTATION);
				entity.SkillBase = DecryptComponent(ComponentParent, TYPE_SKILL);
				entity.VisBase = DecryptComponent(LinkParent, TYPE_P_VISIBILITY);
				entity.AngleBase = DecryptComponent(LinkParent, TYPE_PLAYERCONTROLLER);
				entity.EnemyAngleBase = DecryptComponent(ComponentParent, TYPE_ANGLE);
				health_compo_t health_compo{};
				velocity_compo_t velo_compo{};
				hero_compo_t hero_compo{};
				vis_compo_t vis_compo{};
				if (entity == lastentity) continue;
				else lastentity = entity;

				if (entity.HealthBase) {
					health_compo = SDK->RPM<health_compo_t>(entity.HealthBase);
					Vector2 healthext = SDK->RPM<Vector2>(entity.HealthBase + 0xF0);
					entity.PlayerHealth = health_compo.health + health_compo.armor + health_compo.barrier + healthext.Y;
					entity.PlayerHealthMax = health_compo.health_max + health_compo.armor_max + health_compo.barrier_max + healthext.X;
					entity.MinHealth = health_compo.health;
					entity.MaxHealth = health_compo.health_max;
					entity.MinArmorHealth = health_compo.armor;
					entity.MaxArmorHealth = health_compo.armor_max;
					entity.MinBarrierHealth = health_compo.barrier;
					entity.MaxBarrierHealth = health_compo.barrier_max;
					entity.Alive = (entity.PlayerHealth > 0.f) ? true : false;
					entity.imort = health_compo.isImmortal;
					entity.barrprot = health_compo.isBarrierProjected;
				}
				else continue;
				if (entity.RotationBase) {
					uint64_t baseAddress = SDK->RPM<uint64_t>(entity.RotationBase + 0x7B0);
					entity.Rot = SDK->RPM<Vector3>(baseAddress + 0x8FC);
				}
				if (entity.VelocityBase) {
					velo_compo = SDK->RPM<velocity_compo_t>(entity.VelocityBase);

					entity.pos = Vector3(velo_compo.location.x, velo_compo.location.y - 1.f, velo_compo.location.z);
					entity.velocity = Vector3(velo_compo.velocity.x, velo_compo.velocity.y, velo_compo.velocity.z);
					int head_index = entity.GetSkel()[0];
					int neck_index = entity.GetSkel()[1];
					int chest_index = entity.GetSkel()[2];
					entity.head_pos = entity.GetBonePos(head_index);
					entity.neck_pos = entity.GetBonePos(neck_index);
					entity.chest_pos = entity.GetBonePos(chest_index);
				}
				if (entity.HeroBase) {
					hero_compo = SDK->RPM<hero_compo_t>(entity.HeroBase);
					entity.HeroID = hero_compo.heroid;
					if (entity.HeroID == eHero::HERO_WRECKINGBALL) {
						int head_index = entity.GetSkel()[0];
						int neck_index = entity.GetSkel()[1];
						int chest_index = entity.GetSkel()[2];
						entity.head_pos = entity.GetBonePos(head_index);
						entity.head_pos.Y += 0.02;
						entity.neck_pos = entity.GetBonePos(neck_index);
						entity.chest_pos = entity.GetBonePos(chest_index);
					}
					if (entity.HeroID == eHero::HERO_DVA && GetHeroEngNames(entity.HeroID, entity.LinkBase) != u8"Hana") {
						entity.imort = false;
						entity.head_pos.Y -= 0.1;
						entity.chest_pos = entity.neck_pos;
						entity.chest_pos.Y -= 0.3;
					}
					if (entity.HeroID == eHero::HERO_TRAININGBOT1 || entity.HeroID == eHero::HERO_TRAININGBOT2 || entity.HeroID == eHero::HERO_TRAININGBOT3 || entity.HeroID == eHero::HERO_TRAININGBOT4 || entity.HeroID == eHero::HERO_TRAININGBOT5 || entity.HeroID == eHero::HERO_TRAININGBOT6 || entity.HeroID == eHero::HERO_TRAININGBOT7)
						entity.chest_pos = entity.GetBonePos(83);
				}
				else {
					if (entity.MaxHealth == 225) {
						XMFLOAT3 temppos = SDK->RPM<XMFLOAT3>(entity.VelocityBase + 0x380 + 0x50);
						entity.head_pos = Vector3(temppos.x, temppos.y + 1, temppos.z);
						entity.HeroID = 0x16dd;
						entity.neck_pos = entity.head_pos;
						entity.chest_pos = entity.head_pos;
						entity.pos = entity.neck_pos;
					}
					else if (entity.MaxHealth == 30) {
						XMFLOAT3 temppos = SDK->RPM<XMFLOAT3>(entity.VelocityBase + 0x380 + 0x50);
						entity.head_pos = Vector3(temppos.x, temppos.y, temppos.z);
						entity.HeroID = 0x16ee;
						entity.neck_pos = entity.head_pos;
						entity.chest_pos = entity.head_pos;
						entity.pos = entity.neck_pos;
					}
					else if (entity.MaxHealth == 1000) {
						entity.HeroID = 0x16bb;
					}
					else continue;
				}
				if (Config::draw_info && Config::drawbattletag) {
					entity.statcombase = DecryptComponent(LinkParent, TYPE_STAT);
					if (entity.statcombase) {
						char buffer[64] = u8"";
						if (entity != local_entity) {
							uintptr_t off = SDK->RPM<uintptr_t>(entity.statcombase + 0xE0);
							SDK->read_buf(off, buffer, sizeof(char) * 64);
							entity.battletag = buffer;
						}
					}
				}
				if (entity.TeamBase) {
					entity.Team = (entity.GetTeam() == eTeam::TEAM_DEATHMATCH ||
						entity.GetTeam() != local_entity.GetTeam()) ? true : false;
				}
				if (entity.VisBase) {
					vis_compo = SDK->RPM<vis_compo_t>(entity.VisBase);
					entity.Vis = (DecryptVis(vis_compo.key1) ^ vis_compo.key2) ? true : false;
				}
				if (entity.SkillBase) {
					entity.skill1act = IsSkillActive(entity.SkillBase + 0x40, 0, 0x28E3);
					entity.skill2act = IsSkillActive(entity.SkillBase + 0x40, 0, 0x28E9);
					entity.ultimate = readult(entity.SkillBase + 0x40, 0, 0x1e32);
					if (entity.HeroID == eHero::HERO_SOMBRA && entity.Team && !Config::Rage && !Config::fov360 && !Config::silent && !Config::fakesilent) {
						entity.Vis = (entity.Vis && !IsSkillActivate1(entity.SkillBase + 0x40, 0, 0x7C5));
					}
				}

				if (entity.OutlineBase)
				{
					if (Config::externaloutline && !entity.Vis)	SetBorderLine(0x2, entity.OutlineBase);
					else SetBorderLine(0x1, entity.OutlineBase);
					if (Config::externaloutline) {
						if (entity.Team && i != Config::Targetenemyi && !Config::healthoutline && !Config::rainbowoutline) {
							if (entity.Vis) {
								Config::visenemy = convertToHex(Config::enargb);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::visenemy);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::visenemy);
							}
							else if (i != Config::Targetenemyi) {
								Config::invisenemy = convertToHex(Config::invisenargb);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::invisenemy);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::invisenemy);
							}
						}
						else if (entity.Team && i != Config::Targetenemyi && Config::healthoutline && !Config::rainbowoutline) {
							if (entity.PlayerHealth == entity.MaxHealth) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(0.2, 0.8, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(0.2, 0.8, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth >= 0.7) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(0.2, 1, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(0.2, 1, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth >= 0.3 && entity.PlayerHealth / entity.MaxHealth < 0.7) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(1, 0.8, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(1, 0.8, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth > 0 && entity.PlayerHealth / entity.MaxHealth < 0.3) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(1, 0, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(1, 0, 0.2, 1)));
							}
						}
						else if (entity.Team && i != Config::Targetenemyi && !Config::healthoutline && Config::rainbowoutline) {
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(Config::rainbowargb));
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(Config::rainbowargb));
							if (Config::cps1 == 0) Config::rainbowargb.x += 0.0002;
							else if (Config::cps1 == 1) Config::rainbowargb.x -= 0.0003;
							if (Config::cps2 == 0) Config::rainbowargb.y += 0.0005;
							else if (Config::cps2 == 1) Config::rainbowargb.y -= 0.0002;
							if (Config::cps3 == 0) Config::rainbowargb.z += 0.0003;
							else if (Config::cps3 == 1) Config::rainbowargb.z -= 0.0005;
							if (Config::rainbowargb.x <= 0) Config::cps1 = 0;
							else if (Config::rainbowargb.x >= 1) Config::cps1 = 1;
							if (Config::rainbowargb.y <= 0) Config::cps2 = 0;
							else if (Config::rainbowargb.y >= 1) Config::cps2 = 1;
							if (Config::rainbowargb.z <= 0) Config::cps3 = 0;
							else if (Config::rainbowargb.z >= 1) Config::cps3 = 1;
						}
						if (!entity.Team) {
							Config::Allycolor = convertToHex(Config::allyargb);
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::Allycolor);
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::Allycolor);

						}
					}
					else if (Config::teamoutline && !entity.Team) {
						if (!entity.Vis) SetBorderLine(0x2, entity.OutlineBase);
						else SetBorderLine(0x1, entity.OutlineBase);
						Config::Allycolor = convertToHex(Config::allyargb);
						SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::Allycolor);
						SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::Allycolor);
					}
				}
				if (entity.AngleBase)
				{
					float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(entity.head_pos);
					if (dist <= 1 && GetHeroEngNames(entity.HeroID, entity.LinkBase) != (u8"Unknown")) {
						entity.skillcd1 = readskillcd(entity.SkillBase + 0x40, 0, 0x189c);
						entity.skillcd2 = readskillcd(entity.SkillBase + 0x40, 0, 0x1f89);
						local_entity = entity;
						Config::reloading = IsSkillActivate1(local_entity.SkillBase + 0x40, 0, 0x4BF);

						if (Config::namespoofer) {
							local_entity.statcombase = DecryptComponent(LinkParent, TYPE_STAT);
							uintptr_t off = SDK->RPM<uintptr_t>(local_entity.statcombase + 0xE0);
							SDK->write_buf(off, Config::fakename, sizeof(Config::fakename));
						}
						SDK->g_player_controller = entity.AngleBase;
						if (local_entity.GetTeam() == eTeam::TEAM_DEATHMATCH) entity.Team = false;
					}
				}
				if (ComponentParent && LinkParent && GetHeroNames(entity.HeroID, entity.LinkBase) != (u8"未知"))
					tmp_entities.push_back(entity);
			}
			entities = tmp_entities;
			hp_dy_entities = hpdy_entities;
			Sleep(3);
		}
	}

	inline void viewmatrix_thread() {
		__try {
			while (true) {
				auto viewMatrixVal = ((SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base) + offset::offset_viewmatrix_xor_key) ^ offset::offset_viewmatrix_xor_key2) - offset::offset_viewmatrix_xor_key3;
				Vector2 WindowSize = SDK->RPM<Vector2>(viewMatrixVal + 0x41C);

				static RECT TempRect = { NULL };
				static POINT TempPoint;
				GetClientRect(SDK->Windowsizehd, &TempRect);
				ClientToScreen(SDK->Windowsizehd, &TempPoint);
				TempRect.left = TempPoint.x;
				TempRect.top = TempPoint.y;
				WX = TempRect.right;
				WY = TempRect.bottom;
				viewMatrix = SDK->RPM<Matrix>(viewMatrixPtr);
				viewMatrix_xor = SDK->RPM<Matrix>(viewMatrix_xor_ptr);
				Sleep(5);
			}
		}
		__except (1) {
		}
	}

	inline void PlayerInfo() {
		if (entities.size() > 0) {
			for (c_entity entity : entities){
				if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0) {
					Vector3 Vec3 = entity.head_pos;
					float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(Vec3);
					Vector2 Vec2_A{}, Vec2_B{};
					Vector2 Vec2_C{}, Vec2_D{};
					Vector2 Vec2_E{}, Vec2_F{};

					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y-1.5f , Vec3.Z), &Vec2_A, Vector2(WX, WY)))
						continue;
					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y +1.f, Vec3.Z), &Vec2_B, Vector2(WX, WY)))
						continue;
					float height = abs(Vec2_A.Y - Vec2_B.Y);
					float width = height * 0.85;
					float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
					float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
					float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
					int num7 = (float)(1500 / (int)Height2);
					float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
					float ypos = Vec2_A.Y + Size / 5;

					Size /= 5;
					if (Size < 12) Size = 12;
					if (Size > 16) Size = 16;
					float centerX = (Vec2_A.X + Vec2_B.X) / 2.f;
					float centerY = (Vec2_A.Y + Vec2_B.Y) / 2.f;
					float RealCenter = (centerY + centerX) / 2.f;
					if(Config::healthbar) Render::DrawHealthBar(Vector2(Vec2_A.X+ width/4, Vec2_A.Y), RealCenter, entity.PlayerHealth, entity.PlayerHealthMax);
					if (Config::drawhealth)Render::DrawStrokeText(ImVec2(Vec2_A.X - width / 6, Vec2_A.Y), ImGui::GetColorU32(ImVec4(0, 0.9, 1, 1)), ((u8"[血量：") + std::to_string((int)entity.PlayerHealth)+ (u8"]")).c_str(), Size);
					if (Config::ult)Render::DrawStrokeText(ImVec2(Vec2_A.X - width / 6, Vec2_A.Y + Height2 / 5), ImGui::GetColorU32(ImVec4(0, 0.9, 1, 1)), ((u8"[终极技能：") + std::to_string((int)entity.ultimate) + (u8"]")).c_str(), Size);
					if (Config::dist)Render::DrawStrokeText(ImVec2(Vec2_B.X - width / 6,  Vec2_B.Y), ImGui::GetColorU32(ImVec4(0, 0.9, 1, 1)), ((u8"[距离：") + std::to_string((int)dist) + (u8"]")).c_str(), Size);
					if (Config::name)Render::DrawStrokeText(ImVec2(Vec2_B.X - width / 6, Vec2_B.Y+ Height2/5), ImGui::GetColorU32(ImVec4(0, 0.9, 1, 1)), ((u8"[名称：") + GetHeroNames(entity.HeroID, entity.LinkBase) + (u8"]")).c_str(), Size);
				}
			}
		}
	}

	inline void skillinfo() {
		if (entities.size() > 0) {
			int i = 10;
			for (c_entity entity : entities) {
				std::string heroname = GetHeroEngNames(entity.HeroID, entity.LinkBase).c_str();
				if (entity.Team && heroname != u8"Bot" && heroname != u8"Unknown" && entity.HeroID != 0x16dd && entity.HeroID != 0x16ee && entity.HeroID != 0x16bb) {
					float xpos = WX;
					float ypos = WY + i;
					std::string skillstring;
					float ult = entity.ultimate;
					skillstring = (u8"Enemy:") + heroname + (u8" Ult:") + std::to_string((int)ult);
					Render::DrawSKILL(ImVec2(70, 30 + i), skillstring);
					i += 20;
				}
				else if (entity.Team && (entity.HeroID == 0x16dd || entity.HeroID == 0x16ee || entity.HeroID == 0x16bb)) {
					std::string skillstring;
					skillstring = (u8"Enemy Entity:") + heroname + (u8" HP：") + std::to_string((int)entity.PlayerHealth) + (u8"/") + std::to_string((int)entity.MaxHealth);
					Render::DrawSKILL(ImVec2(70, 30 + i), skillstring);
					i += 20;
				}
			}
			i += 60;
			for (c_entity entity : entities) {//传常量引用
				std::string heroname = GetHeroEngNames(entity.HeroID, entity.LinkBase).c_str();
				if (!entity.Team && heroname != u8"Bot" && heroname != u8"Unknown" && entity.HeroID != 0x16dd && entity.HeroID != 0x16ee && entity.HeroID != 0x16bb) {
					float xpos = WX;
					float ypos = WY + i;
					std::string skillstring;
					float ult = entity.ultimate;
					skillstring = (u8"Ally:") + heroname + (u8" Ult:") + std::to_string((int)ult);
					Render::DrawSKILL(ImVec2(70, 30 + i), skillstring);
					i += 20;
				}
				else if (!entity.Team && (entity.HeroID == 0x16dd || entity.HeroID == 0x16ee || entity.HeroID == 0x16bb)) {
					std::string skillstring;
					skillstring = (u8"Ally entity:") + heroname + (u8" HP:") + std::to_string((int)entity.PlayerHealth) + (u8"/") + std::to_string((int)entity.MaxHealth);
					Render::DrawSKILL(ImVec2(70, 30 + i), skillstring);
					i += 20;
				}
			}
		}
	}


	inline void Draw_Skel() {
		if (entities.size() > 0) {
			for (c_entity entity : entities) {
				if (entity.HeroID == 0x16dd) continue;
				if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0) {
					espBone a = entity.getBoneList(entity.GetSkel());
					DWORD Col = entity.Vis ? Color{ 0, 0, 0, 0 }.RGBA2ARGB(255) : Color{ 0, 0, 0, 0 }.RGBA2ARGB(255);
					DWORD Col2 = entity.Vis ? Color{ 0, 255, 0, 200 }.RGBA2ARGB(255) : Color{ 255, 0, 0, 200 }.RGBA2ARGB(255);
					Vector2 outPos{};
					if (!a.boneerror) {
						if (viewMatrix.WorldToScreen(entity.head_pos, &outPos, Vector2(WX, WY))) {
							if (entity.HeroID == eHero::HERO_TRAININGBOT1 || entity.HeroID == eHero::HERO_TRAININGBOT2 || entity.HeroID == eHero::HERO_TRAININGBOT3 || entity.HeroID == eHero::HERO_TRAININGBOT4) {
								Render::RenderLine(a.head, a.neck, Col, 2.f);
								Render::RenderLine(a.neck, a.body_1, Col, 2.f);
								Render::RenderLine(a.body_1, a.l_1, Col, 2.f);
								Render::RenderLine(a.body_1, a.r_1, Col, 2.f);

								Render::RenderLine(a.head, a.neck, Col2, 2.f);
								Render::RenderLine(a.neck, a.body_1, Col2, 2.f);
								Render::RenderLine(a.body_1, a.l_1, Col2, 2.f);
								Render::RenderLine(a.body_1, a.r_1, Col2, 2.f);
							}
							else {
								Render::RenderLine(a.head, a.neck, Col, 2.f);
								Render::RenderLine(a.neck, a.body_1, Col, 2.f);
								Render::RenderLine(a.body_1, a.body_2, Col, 2.f);
								Render::RenderLine(a.neck, a.l_1, Col, 2.f);
								Render::RenderLine(a.neck, a.r_1, Col, 2.f);
								Render::RenderLine(a.l_1, a.l_d_1, Col, 2.f);
								Render::RenderLine(a.r_1, a.r_d_1, Col, 2.f);
								Render::RenderLine(a.l_d_1, a.l_d_2, Col, 2.f);
								Render::RenderLine(a.r_d_1, a.r_d_2, Col, 2.f);
								Render::RenderLine(a.body_2, a.l_a_1, Col, 2.f);
								Render::RenderLine(a.body_2, a.r_a_1, Col, 2.f);
								Render::RenderLine(a.l_a_1, a.l_a_2, Col, 2.f);
								Render::RenderLine(a.r_a_1, a.r_a_2, Col, 4.f);

								Render::RenderLine(a.r_a_2, a.sex, Col, 2.f);
								Render::RenderLine(a.l_a_2, a.sex1, Col, 2.f);
								Render::RenderLine(a.sex, a.sex2, Col, 2.f);
								Render::RenderLine(a.sex1, a.sex3, Col, 2.f);

								Render::RenderLine(a.head, a.neck, Col2, 2.f);
								Render::RenderLine(a.neck, a.body_1, Col2, 2.f);
								Render::RenderLine(a.body_1, a.body_2, Col2, 2.f);
								Render::RenderLine(a.neck, a.l_1, Col2, 2.f);
								Render::RenderLine(a.neck, a.r_1, Col2, 2.f);
								Render::RenderLine(a.l_1, a.l_d_1, Col2, 2.f);
								Render::RenderLine(a.r_1, a.r_d_1, Col2, 2.f);
								Render::RenderLine(a.l_d_1, a.l_d_2, Col2, 2.f);
								Render::RenderLine(a.r_d_1, a.r_d_2, Col2, 2.f);
								Render::RenderLine(a.body_2, a.l_a_1, Col2, 2.f);
								Render::RenderLine(a.body_2, a.r_a_1, Col2, 2.f);
								Render::RenderLine(a.l_a_1, a.l_a_2, Col2, 2.f);
								Render::RenderLine(a.r_a_1, a.r_a_2, Col2, 2.f);

								Render::RenderLine(a.r_a_2, a.sex, Col2, 2.f);
								Render::RenderLine(a.l_a_2, a.sex1, Col2, 2.f);
								Render::RenderLine(a.sex, a.sex2, Col2, 2.f);
								Render::RenderLine(a.sex1, a.sex3, Col2, 2.f);
							}
						}
					}
				}
			}
		}
	}
	inline void draw3dbox() {
		
			if (entities.size() > 0)
			{
				for (c_entity entity : entities){
					if (entity.HeroID == 0x16dd) continue;
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector2 Vec2_A{}, Vec2_B{}, Vec2_C{}, Vec2_D{}, Vec2_E{}, Vec2_F{}, Vec2_G{}, Vec2_H{};
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X - 0.5f, Vec3.Y + 0.3f, Vec3.Z - 0.5f), &Vec2_A, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X - 0.5f, Vec3.Y + 0.3f, Vec3.Z + 0.5f), &Vec2_B, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X + 0.5f, Vec3.Y + 0.3f, Vec3.Z - 0.5f), &Vec2_C, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X + 0.5f, Vec3.Y + 0.3f, Vec3.Z + 0.5f), &Vec2_D, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X - 0.5f, Vec3.Y - 1.2f, Vec3.Z - 0.5f), &Vec2_E, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X - 0.5f, Vec3.Y - 1.2f, Vec3.Z + 0.5f), &Vec2_F, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X + 0.5f, Vec3.Y - 1.2f, Vec3.Z - 0.5f), &Vec2_G, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X + 0.5f, Vec3.Y - 1.2f, Vec3.Z + 0.5f), &Vec2_H, Vector2(WX, WY)))
							continue;

						float height = abs(Vec2_A.Y - Vec2_B.Y);
						float width = height * 0.85;
						float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
						float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
						float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
						int num7 = (float)(1500 / (int)Height2);
						float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
						float ypos = Vec2_A.Y + Size / 5;

						Render::DrawLine(Vec2_A, Vec2_B, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_B, Vec2_D, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_D, Vec2_C, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_C, Vec2_A, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_A, Vec2_E, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_B, Vec2_F, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_C, Vec2_G, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_D, Vec2_H, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_E, Vec2_F, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_F, Vec2_H, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_H, Vec2_G, Color(255, 255, 255, 255), 2.f);
						Render::DrawLine(Vec2_G, Vec2_E, Color(255, 255, 255, 255), 2.f);
					}
				}
			}
		
	}

	inline void drawline() {
		if (entities.size() > 0)
		{
			for (c_entity entity : entities)
			{
				if (entity.HeroID == 0x16dd) continue;
				if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0)
				{
					Vector3 Vec3 = entity.head_pos;
					Vector2 Vec2_A{};
					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y, Vec3.Z), &Vec2_A, Vector2(WX, WY)))
						continue;

					Render::DrawLine(Vector2(WX / 2, WY / 2), Vec2_A, Color(160, 218, 255, 150), 2.f);

				}
			}
		}
	}
	inline void esp() {
		ImDrawList* Draw = ImGui::GetForegroundDrawList();
		ImVec2 CrossHair = ImVec2(WX / 2.0f, WY / 2.0f);
		if (Config::draw_hp_pack) {
			for (hpanddy hppack : hp_dy_entities) {
				if (hppack.entityid == 0x400000000002533) continue;
				Vector2 resultvec1{}, resultvec2{}, resultvec3{}, resultvec4{}, resultvec5{}, resultvec6{}, resultvec7{}, resultvec8{}, stringresult{};
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x, hppack.POS.y + 0.5f, hppack.POS.z), &stringresult, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y, hppack.POS.z - 0.5f), &resultvec1, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y, hppack.POS.z - 0.5f), &resultvec1, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y, hppack.POS.z + 0.5f), &resultvec2, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x + 0.5f, hppack.POS.y, hppack.POS.z - 0.5f), &resultvec3, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x + 0.5f, hppack.POS.y, hppack.POS.z + 0.5f), &resultvec4, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y + 1.f, hppack.POS.z - 0.5f), &resultvec5, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y + 1.f, hppack.POS.z + 0.5f), &resultvec6, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x + 0.5f, hppack.POS.y + 1.f, hppack.POS.z - 0.5f), &resultvec7, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x + 0.5f, hppack.POS.y + 1.f, hppack.POS.z + 0.5f), &resultvec8, Vector2(WX, WY)))
					continue;
				float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(Vector3(hppack.POS.x, hppack.POS.y, hppack.POS.z));
				Render::DrawStrokeText(ImVec2(stringresult.X - 5.f, stringresult.Y - 8.f), ImGui::GetColorU32(ImVec4(1, 0, 0.2f, 1)), ((u8"HP PACK")), 16.f);
				Render::DrawStrokeText(ImVec2(stringresult.X - 5.f, stringresult.Y + 8.f), ImGui::GetColorU32(ImVec4(1, 0, 0.2f, 1)), ((u8"[DIST：") + std::to_string((int)dist) + (u8"]")).c_str(), 16.f);
				Render::DrawLine(resultvec1, resultvec2, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec1, resultvec3, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec2, resultvec4, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec3, resultvec4, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec5, resultvec6, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec5, resultvec7, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec6, resultvec8, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec7, resultvec8, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec1, resultvec5, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec2, resultvec6, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec3, resultvec7, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec4, resultvec8, Color(102, 153, 255, 100), 2.f);
			}
		}
		if ((Config::draw_info || Config::draw_skel || Config::skillinfo || Config::drawbox3d || Config::draw_edge || Config::drawline || Config::radar || Config::eyeray || Config::crosscircle) && entities.size() > 0) {
			int drawradar = 0;
			for (c_entity entity : entities)
			{
				if (Config::radar) {
					if (!drawradar) {
						Draw->AddCircleFilled(ImVec2(WX - 200, WY - 400), 200, ImU32(IM_COL32(0, 0, 0, 120)));
						float fovline = 0;
						if (Config::enablechangefov) {
							fovline = Config::CHANGEFOV;
						}
						else {
							fovline = SDK->RPM<float>(SDK->dwGameBase + offset::changefov);
						}
						fovline /= 2;
						float liney = 200 * cos(DEG2RAD(fovline));
						float linex = sqrt(200 * 200 - liney * liney);
						Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(WX - 200 - linex, WY - 400 - liney), Color(153, 102, 255, 255), 2.f);
						Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(WX - 200 + linex, WY - 400 - liney), Color(153, 102, 255, 255), 2.f);

						Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(WX - 200, WY - 400 - 200), Color(255, 255, 0, 120), 1.f);

						drawradar = 1;
					}
					if (entity == local_entity) {
						Draw->AddCircleFilled(ImVec2(WX - 200, WY - 400), 4.f, ImU32(IM_COL32(255, 255, 51, 255)));
						continue;
					}
					if (entity.pos != Vector3(0, 0, 0) && entity.PlayerHealth > 0)
					{
						DirectX::XMFLOAT3 Result{};
						XMMATRIX rotMatrix = XMMatrixRotationY(local_entity.Rot.X/* - atan2(this->Rot.Z, this->Rot.X)*/);
						XMMATRIX inverseRotMatrix = XMMatrixInverse(nullptr, rotMatrix);
						DirectX::XMFLOAT3 worldPoint = { entity.head_pos.X - viewMatrix_xor.get_location().x,entity.head_pos.Y - viewMatrix_xor.get_location().y,entity.head_pos.Z - viewMatrix_xor.get_location().z };
						worldPoint.x *= 2.5;
						worldPoint.y *= 2.5;
						worldPoint.z *= 2.5;
						DirectX::XMVECTOR worldVector = XMLoadFloat3(&worldPoint);
						DirectX::XMVECTOR characterVector = XMVector3Transform(worldVector, inverseRotMatrix);
						DirectX::XMFLOAT3 characterPoint;
						XMStoreFloat3(&characterPoint, characterVector);
						float therad = sqrt(characterPoint.x * characterPoint.x + characterPoint.z * characterPoint.z);
						if (therad > 200) {
							characterPoint.x = (characterPoint.x / therad) * 200;
							characterPoint.z = (characterPoint.z / therad) * 200;
						}
						ImVec2 pointPosition(WX - 200 - characterPoint.x, WY - 400 - characterPoint.z);
						float pointRadius = 3.4f;
						Vector3 whereiseye;
						Vector3 whereisarrow1, whereisarrow2, whereisarrow3, whereisarrow4;
						entity.GetEyeRayPoint(whereiseye, whereisarrow1, whereisarrow2, whereisarrow3, whereisarrow4);
						DirectX::XMFLOAT3 eyePoint = { (whereiseye.X - entity.head_pos.X) * 4 + whereiseye.X - viewMatrix_xor.get_location().x,whereiseye.Y - viewMatrix_xor.get_location().y,(whereiseye.Z - entity.head_pos.Z) * 4 + whereiseye.Z - viewMatrix_xor.get_location().z };
						eyePoint.x *= 2.5;
						eyePoint.y *= 2.5;
						eyePoint.z *= 2.5;
						DirectX::XMVECTOR theeyeworldVector = XMLoadFloat3(&eyePoint);
						DirectX::XMVECTOR theeyeVector = XMVector3Transform(theeyeworldVector, inverseRotMatrix);
						DirectX::XMFLOAT3 theeyePoint;
						XMStoreFloat3(&theeyePoint, theeyeVector);
						float theradforeye = sqrt(theeyePoint.x * theeyePoint.x + theeyePoint.z * theeyePoint.z);
						if (theradforeye > 195) {
							theeyePoint.x = (theeyePoint.x / theradforeye) * 195;
							theeyePoint.z = (theeyePoint.z / theradforeye) * 195;
						}
						ImVec2 eyepointPosition(WX - 200 - theeyePoint.x, WY - 400 - theeyePoint.z);
						ImU32 pointColor;
						if (entity.Vis)
							pointColor = IM_COL32(255, 0, 51, 255);
						else pointColor = IM_COL32(204, 0, 153, 255);
						if (!entity.Team) {
							Draw->AddCircleFilled(pointPosition, pointRadius, IM_COL32(102, 255, 255, 255));
						}
						else {
							Draw->AddCircleFilled(pointPosition, pointRadius, pointColor);
							Render::DrawLine(Vector2(eyepointPosition.x, eyepointPosition.y), Vector2(pointPosition.x, pointPosition.y), Color(255, 0, 0, 255), 2.2f);
							if (Config::radarline) {
								if (entity.Vis)
									Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(pointPosition.x, pointPosition.y), Color(160, 218, 255, 150), 1.f);
								else
									Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(pointPosition.x, pointPosition.y), Color(204, 102, 102, 150), 1.f);
							}
						}
					}
				}
				if (Config::draw_info) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0) {
						Vector3 Vec3 = entity.head_pos;
						float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(Vec3);
						Vector2 Vec2_A{}, Vec2_B{};
						Vector2 Vec2_C{}, Vec2_D{};
						Vector2 Vec2_E{}, Vec2_F{};
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y - 1.5f, Vec3.Z), &Vec2_A, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y + 1.f, Vec3.Z), &Vec2_B, Vector2(WX, WY)))
							continue;
						float height = abs(Vec2_A.Y - Vec2_B.Y);
						float width = height * 0.85;
						float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
						float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
						float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
						int num7 = (float)(1500 / (int)Height2);
						float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
						float ypos = Vec2_A.Y + Size / 5;
						Size /= 2;
						if (Size < 16) Size = 16;
						if (Size > 20) Size = 20;
						if (Config::drawbattletag) Render::DrawStrokeText(ImVec2(Vec2_A.X - width / 6, Vec2_B.Y - Size), ImGui::GetColorU32(ImVec4(0, 0.9, 1, 1)), ((u8"[ID：") + entity.battletag + (u8"]")).c_str(), Size);
						if (Config::healthbar) Render::DrawSeerLikeHealth(Vec2_B.X, Vec2_B.Y - 20.f, (int)(1.25f * entity.ultimate), 125, (int)entity.PlayerHealth, (int)entity.PlayerHealthMax);
						//if(Config::healthbar) Render::DrawHealthBar(Vector2(Vec2_A.X, Vec2_A.Y), Height2/3, entity.PlayerHealth, entity.PlayerHealthMax);
						if (Config::drawhealth)Render::DrawStrokeText(ImVec2(Vec2_A.X - width / 6, Vec2_A.Y), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), ((u8"[HP：") + std::to_string((int)entity.PlayerHealth) + (u8"]")).c_str(), Size);
						if (Config::ult)Render::DrawStrokeText(ImVec2(Vec2_A.X - width / 6, Vec2_A.Y + Size), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), ((u8"[ULT：") + std::to_string((int)entity.ultimate) + (u8"]")).c_str(), Size);
						if (Config::dist)Render::DrawStrokeText(ImVec2(Vec2_B.X - width / 6, Vec2_B.Y), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), ((u8"[DIST：") + std::to_string((int)dist) + (u8"]")).c_str(), Size);
						if (Config::name)Render::DrawStrokeText(ImVec2(Vec2_B.X - width / 6, Vec2_B.Y + Size), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), ((u8"[NAME：") + GetHeroEngNames(entity.HeroID, entity.LinkBase) + (u8"]")).c_str(), Size);
					}
				} 
				if (Config::draw_skel) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd) {
						espBone a = entity.getBoneList(entity.GetSkel());
						DWORD Col = entity.Vis ? Color{ 0, 0, 0, 0 }.RGBA2ARGB(255) : Color{ 0, 0, 0, 0 }.RGBA2ARGB(255);
						DWORD Col2 = entity.Vis ? Color{ 0, 255, 0, 200 }.RGBA2ARGB(255) : Color{ 255, 0, 0, 200 }.RGBA2ARGB(255);
						ImU32 pointcolor = IM_COL32(102, 153, 255, 255);
						float radisofpoint = 2.f;
						Vector2 outPos{};
						if (!a.boneerror) {
							if (viewMatrix.WorldToScreen(entity.head_pos, &outPos, Vector2(WX, WY))) {
								if (entity.HeroID == eHero::HERO_TRAININGBOT1 || entity.HeroID == eHero::HERO_TRAININGBOT2 || entity.HeroID == eHero::HERO_TRAININGBOT3 || entity.HeroID == eHero::HERO_TRAININGBOT4) {
									Render::RenderLine(a.head, a.neck, Col, 2.f);
									Render::RenderLine(a.neck, a.body_1, Col, 2.f);
									Render::RenderLine(a.body_1, a.l_1, Col, 2.f);
									Render::RenderLine(a.body_1, a.r_1, Col, 2.f);

									Render::RenderLine(a.head, a.neck, Col2, 2.f);
									Render::RenderLine(a.neck, a.body_1, Col2, 2.f);
									Render::RenderLine(a.body_1, a.l_1, Col2, 2.f);
									Render::RenderLine(a.body_1, a.r_1, Col2, 2.f);
									Draw->AddCircleFilled(ImVec2(a.head.X, a.head.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.body_1.X, a.body_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.neck.X, a.neck.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.l_1.X, a.l_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.r_1.X, a.r_1.Y), radisofpoint, pointcolor);
								}
								else {
									Render::RenderLine(a.head, a.neck, Col, 2.f);
									Render::RenderLine(a.neck, a.body_1, Col, 2.f);
									Render::RenderLine(a.body_1, a.body_2, Col, 2.f);
									Render::RenderLine(a.neck, a.l_1, Col, 2.f);
									Render::RenderLine(a.neck, a.r_1, Col, 2.f);
									Render::RenderLine(a.l_1, a.l_d_1, Col, 2.f);
									Render::RenderLine(a.r_1, a.r_d_1, Col, 2.f);
									Render::RenderLine(a.l_d_1, a.l_d_2, Col, 2.f);
									Render::RenderLine(a.r_d_1, a.r_d_2, Col, 2.f);
									Render::RenderLine(a.body_2, a.l_a_1, Col, 2.f);
									Render::RenderLine(a.body_2, a.r_a_1, Col, 2.f);
									Render::RenderLine(a.l_a_1, a.l_a_2, Col, 2.f);
									Render::RenderLine(a.r_a_1, a.r_a_2, Col, 4.f);

									Render::RenderLine(a.r_a_2, a.sex, Col, 2.f);
									Render::RenderLine(a.l_a_2, a.sex1, Col, 2.f);
									Render::RenderLine(a.sex, a.sex2, Col, 2.f);
									Render::RenderLine(a.sex1, a.sex3, Col, 2.f);

									Render::RenderLine(a.head, a.neck, Col2, 2.f);
									Render::RenderLine(a.neck, a.body_1, Col2, 2.f);
									Render::RenderLine(a.body_1, a.body_2, Col2, 2.f);
									Render::RenderLine(a.neck, a.l_1, Col2, 2.f);
									Render::RenderLine(a.neck, a.r_1, Col2, 2.f);
									Render::RenderLine(a.l_1, a.l_d_1, Col2, 2.f);
									Render::RenderLine(a.r_1, a.r_d_1, Col2, 2.f);
									Render::RenderLine(a.l_d_1, a.l_d_2, Col2, 2.f);
									Render::RenderLine(a.r_d_1, a.r_d_2, Col2, 2.f);
									Render::RenderLine(a.body_2, a.l_a_1, Col2, 2.f);
									Render::RenderLine(a.body_2, a.r_a_1, Col2, 2.f);
									Render::RenderLine(a.l_a_1, a.l_a_2, Col2, 2.f);
									Render::RenderLine(a.r_a_1, a.r_a_2, Col2, 2.f);

									Render::RenderLine(a.r_a_2, a.sex, Col2, 2.f);
									Render::RenderLine(a.l_a_2, a.sex1, Col2, 2.f);
									Render::RenderLine(a.sex, a.sex2, Col2, 2.f);
									Render::RenderLine(a.sex1, a.sex3, Col2, 2.f);
									Draw->AddCircleFilled(ImVec2(a.head.X, a.head.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.body_1.X, a.body_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.body_2.X, a.body_2.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.neck.X, a.neck.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.l_1.X, a.l_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.r_1.X, a.r_1.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.l_d_1.X, a.l_d_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.l_d_2.X, a.l_d_2.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.r_d_1.X, a.r_d_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.r_d_2.X, a.r_d_2.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.l_a_1.X, a.l_a_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.l_a_2.X, a.l_a_2.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.r_a_1.X, a.r_a_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.r_a_2.X, a.r_a_2.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.sex.X, a.sex.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.sex2.X, a.sex2.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.sex1.X, a.sex1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.sex3.X, a.sex3.Y), radisofpoint, pointcolor);
								}
							}
						}
					}
				}
				if (Config::drawline)
				{
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector2 Vec2_A{};
						if (viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y, Vec3.Z), &Vec2_A, Vector2(WX, WY))) {
							if (entity.Vis)
								Render::DrawLine(Vector2(WX / 2, WY / 2), Vec2_A, Color(160, 218, 255, 150), 2.f);
							else
								Render::DrawLine(Vector2(WX / 2, WY / 2), Vec2_A, Color(204, 102, 102, 150), 2.f);
						}
					}
				}
				if (Config::draw_edge) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector2 Vec2_A{}, Vec2_B{};
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y + 0.1f, Vec3.Z), &Vec2_A, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y - 1.8f, Vec3.Z), &Vec2_B, Vector2(WX, WY)))
							continue;

						float height = abs(Vec2_A.Y - Vec2_B.Y);
						float width = height * 0.85;
						float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
						float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
						float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
						int num7 = (float)(1500 / (int)Height2);
						float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
						float ypos = Vec2_A.Y + Size / 5;

						Render::DrawCorneredBox(Vec2_A.X - (width / 2) - 1, Vec2_A.Y - (height / 5) - 1, width + 2, height + 12, ImGui::GetColorU32(Config::EnemyCol));
						Render::DrawFilledRect(Vec2_A.X - (width / 2) - 1, Vec2_A.Y - (height / 5) - 1, width + 2, height + 12, ImColor(0, 0, 0, 60));
					}

				}
				if (Config::drawbox3d) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector3 veca{}, vecb{}, vecc{}, vecd{}, vece{}, vecf{}, vecg{}, vech{};
						Vector2 Vec2_A{}, Vec2_B{}, Vec2_C{}, Vec2_D{}, Vec2_E{}, Vec2_F{}, Vec2_G{}, Vec2_H{};
						entity.Get3DBoxPos(veca, vecb, vecc, vecd, vece, vecf, vecg, vech);
						if (!viewMatrix.WorldToScreen(veca, &Vec2_A, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(vecb, &Vec2_B, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(vecc, &Vec2_C, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(vecd, &Vec2_D, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(vece, &Vec2_E, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(vecf, &Vec2_F, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(vecg, &Vec2_G, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(vech, &Vec2_H, Vector2(WX, WY)))
							continue;

						float height = abs(Vec2_A.Y - Vec2_B.Y);
						float width = height * 0.85;
						float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
						float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
						float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
						int num7 = (float)(1500 / (int)Height2);
						float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
						float ypos = Vec2_A.Y + Size / 5;

						if (entity.Vis) {
							Render::DrawLine(Vec2_A, Vec2_B, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_B, Vec2_D, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_D, Vec2_C, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_C, Vec2_A, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_A, Vec2_E, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_B, Vec2_F, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_C, Vec2_G, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_D, Vec2_H, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_E, Vec2_F, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_F, Vec2_H, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_H, Vec2_G, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_G, Vec2_E, Color(0, 204, 204, 150), 1.2f);
						}
						else {
							Render::DrawLine(Vec2_A, Vec2_B, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_B, Vec2_D, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_D, Vec2_C, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_C, Vec2_A, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_A, Vec2_E, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_B, Vec2_F, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_C, Vec2_G, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_D, Vec2_H, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_E, Vec2_F, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_F, Vec2_H, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_H, Vec2_G, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_G, Vec2_E, Color(255, 51, 153, 150), 1.2f);
						}
					}
				}
				if (Config::eyeray) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector3 eyepoint{}, whereisarrow1{}, whereisarrow2{}, whereisarrow3{}, whereisarrow4{};
						Vector2 Vec2_A{}, Vec2_B{}, Vec2_C{}, Vec2_D{}, Vec2_E{}, Vec2_F{};
						entity.GetEyeRayPoint(eyepoint, whereisarrow1, whereisarrow2, whereisarrow3, whereisarrow4);
						if (!viewMatrix.WorldToScreen(Vec3, &Vec2_A, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(eyepoint, &Vec2_B, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(whereisarrow1, &Vec2_C, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(whereisarrow2, &Vec2_D, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(whereisarrow3, &Vec2_E, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(whereisarrow4, &Vec2_F, Vector2(WX, WY)))
							continue;

						if (entity.Vis) {
							Render::DrawLine(Vec2_A, Vec2_B, Color(255, 0, 0, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_C, Color(255, 51, 102, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_D, Color(255, 51, 102, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_E, Color(255, 51, 102, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_F, Color(255, 51, 102, 255), 3.f);
						}
						else {
							Render::DrawLine(Vec2_A, Vec2_B, Color(102, 153, 255, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_C, Color(102, 153, 255, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_D, Color(102, 153, 255, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_E, Color(102, 153, 255, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_F, Color(102, 153, 255, 255), 3.f);
						}
					}
				}
				if (Config::crosscircle && (GetAsyncKeyState(0x1) || GetAsyncKeyState(0x2) || GetAsyncKeyState(VK_XBUTTON2) || GetAsyncKeyState(VK_XBUTTON1))) {
					Vector3 Vec3 = entity.chest_pos;
					Vector2 Vec2_A{};
					if (!viewMatrix.WorldToScreen(Vec3, &Vec2_A, Vector2(WX, WY)))
						continue;
					if (Config::crss == 0) {
						Config::locx = rand() % 20;
						Config::locy = rand() % 10;
						Config::therad = rand() % 30;
						Config::pon = rand() % 10;
						if (Config::pon >= 5) Config::locx = -Config::locx;
						Config::pon = rand() % 10;
						if (Config::pon >= 5) Config::locy = -Config::locy;
					}
					Config::crss++;
					if (Config::crss == 14) Config::crss = 0;
					if (entity.Alive && entity.Vis && local_entity.address && entity.Team)
						Draw->AddCircle(ImVec2(Vec2_A.X + Config::locx, Vec2_A.Y + Config::locy), Config::therad, IM_COL32(255, 255, 255, 255), 3000, 3);
				}
			}
		}

		if (Config::skillinfo) {
			skillinfo();
		}
		if (Config::draw_fov)
		{
			Draw->AddCircle(CrossHair, Config::Fov, ImGui::GetColorU32(Config::fovcol), 3000);
			if (Config::secondaim)
				Draw->AddCircle(CrossHair, Config::Fov2, ImGui::GetColorU32(Config::fovcol2), 3000);
		}
	}
	HWND hwnd;
	inline void overlay_thread() {
		__try {
			int tab_index = 5;
			int subindex = 1;
			std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

			HWND tWnd = FindWindowA(("TankWindowClass"), NULL);

			WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, ("TestyCheat"), NULL };
			RegisterClassEx(&wc);
			hwnd = CreateWindow(wc.lpszClassName, ("TestyCheat"), WS_POPUP, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
			if (!CreateDeviceD3D(hwnd))
			{
				CleanupDeviceD3D();
				UnregisterClass(wc.lpszClassName, wc.hInstance);
			}

			MARGINS margins = { -1 };
			DwmExtendFrameIntoClientArea(hwnd, &margins);

			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);

			ShowWindow(hwnd, SW_SHOWDEFAULT);
			UpdateWindow(hwnd);

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImFont* ico = nullptr;
			ImFont* ico234 = nullptr;
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImFontConfig font_cfg;
			font_cfg.FontDataOwnedByAtlas = false;
			io.Fonts->AddFontFromMemoryTTF((void*)SimHei_data, sizeof(SimHei_data), 24.f, &font_cfg, io.Fonts->GetGlyphRangesChineseFull());
			ImGui::MergeIconsWithLatestFont(25.f, false);
			io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 18.f, &font_cfg);
			ImGui::MergeIconsWithLatestFont(25.f, false);
			if (ico == nullptr)
				ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 65.f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			if(ico234==nullptr)
				ico234 = io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 65.f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			ImCandy::Theme_Cyberpunk();

			D3DX11_IMAGE_LOAD_INFO info;
			ID3DX11ThreadPump* pump{ nullptr };
			D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, CompChip, sizeof(CompChip), &info,
				pump, &Image, 0);
			ImGui_ImplWin32_Init(tWnd);
			ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


			D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, ViewMonitor, sizeof(ViewMonitor), &info,
				pump, &Image2, 0);
			ImGui_ImplWin32_Init(tWnd);
			ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

			D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, Vamp, sizeof(Vamp), &info,
				pump, &Image3, 0);
			ImGui_ImplWin32_Init(tWnd);
			ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

			D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, PadlockUser, sizeof(PadlockUser), &info,
				pump, &Image4, 0);
			ImGui_ImplWin32_Init(tWnd);
			ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

			D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, Programming, sizeof(Programming), &info,
				pump, &Image5, 0);
			ImGui_ImplWin32_Init(tWnd);
			ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			int FPS;
			int doingone = 0;
			float colora, colorb, colorc;
			colora = 40.f;
			colorb = 255.f; 
			colorc = 0.f;
			float jj1 = 1, jj2 = 0, jj3 = 1;

			float colora2, colorb2, colorc2;
			colora2 = 70.f;
			colorb2 = 225.f;
			colorc2 = 30.f;
			float jj12 = 1, jj22 = 0, jj32 = 1;

			float colora3, colorb3, colorc3;
			colora3 = 100.f;
			colorb3 = 195.f;
			colorc3 = 60.f;
			float jj13 = 1, jj23 = 0, jj33 = 1;

			float colora4, colorb4, colorc4;
			colora4 = 130.f;
			colorb4 = 165.f;
			colorc4 = 90.f;
			float jj14 = 1, jj24 = 0, jj34 = 1;

			float colora5, colorb5, colorc5;
			colora5 = 160.f;
			colorb5 = 135.f;
			colorc5 = 120.f;
			float jj15 = 1, jj25 = 0, jj35 = 1;
			DEVMODE dm;
			while (FindWindowA(("TankWindowClass"), NULL))
			{
				
				dm.dmSize = sizeof(DEVMODE);

				EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
				if(!Config::Menu)
					Sleep(5);
				DWORD Style = GetWindowLong(tWnd, GWL_STYLE);
				RECT rect;
				GetWindowRect(tWnd, &rect);

				SetWindowPos(hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOZORDER);

				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

				if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}

				if (GetAsyncKeyState(VK_LBUTTON))
				{
					ImGui::GetIO().MouseDown[0] = true;
				}
				else
				{
					ImGui::GetIO().MouseDown[0] = false;
				}

				if (GetAsyncKeyState(VK_INSERT)) {
					Config::Menu = !Config::Menu;
					Sleep(250);
				}
				if (!Config::Menu&&doingone==0) {
					doingone = 1;
					SetFocus(tWnd);
					SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
				}
				if (Config::Menu && doingone == 1)
				{
					SetFocus(hwnd);
					ShowCursor(TRUE);
					SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW);
					doingone = 0;
				}
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(30.f / 255.f, 102.f / 255.f, 150.f / 255.f, 70.f / 255.f));
				ImGui::RenderNotifications();
				ImGui::PopStyleVar(1);
				ImGui::PopStyleColor(1);
				if (Config::Menu)
				{
					ImGui::SetNextWindowPos(ImVec2(OW::WX / 2.f - 550.f, OW::WY / 2.f - 425.f));
					ImGui::SetNextWindowSize(ImVec2(1100, 850));
					bool _visible = true;
					if (ImGui::Begin((u8"ow2 test"), &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar)) {
						ImGui::Spacing();
						ImGui::PushFont(ico);
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora / 255.f, colorb / 255.f, colorc / 255.f, 255.f / 255.f));
						ImGui::PopStyleColor();
						ImGui::SameLine();
						ImGui::PopFont();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora4 / 255.f, colorb4 / 255.f, colorc4 / 255.f, 255.f / 255.f));
						ImGui::PushFont(ico234);
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Text(("                    ow2 test"));
						ImGui::PopStyleColor();
						ImGui::PopFont();
						ImGui::PushFont(ico);// from here it makes text into img
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora / 255.f, colorb / 255.f, colorc / 255.f, 255.f / 255.f));
						ImGui::Text("   ");
						ImGui::SameLine();
						ImGui::Image((PVOID)Image, ImVec2(60, 60)); // replace with computer chip
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora / 255.f, colorb / 255.f, colorc / 255.f, 255.f / 255.f));
						ImGui::Text("       ");
						ImGui::PopStyleColor();
						if (jj1 == 0) colora -= 0.7;
						else colora += 0.7;
						if (colora <= 0) jj1 = 1;
						else if (colora >= 255) jj1 = 0;

						if (jj2 == 0) colorb -= 0.7;
						else colorb += 0.7;
						if (colorb <= 0) jj2 = 1;
						else if (colorb >= 255) jj2 = 0;

						if (jj3 == 0) colorc -= 0.7;
						else colorc += 0.7;
						if (colorc <= 0) jj3 = 1;
						else if (colorc >= 255) jj3 = 0;
						ImGui::SameLine(); // Ensures all images and text are on the same line

						ImGui::SameLine(); // Keep Image2 and text on the same line
						ImGui::Image((PVOID)Image2, ImVec2(60, 60)); // replace with monitor
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora2 / 255.f, colorb2 / 255.f, colorc2 / 255.f, 255.f / 255.f));
						ImGui::Text("       ");
						ImGui::PopStyleColor();
						if (jj12 == 0) colora2 -= 0.7;
						else colora2 += 0.7;
						if (colora2 <= 0) jj12 = 1;
						else if (colora2 >= 255) jj12 = 0;

						if (jj22 == 0) colorb2 -= 0.7;
						else colorb2 += 0.7;
						if (colorb2 <= 0) jj22 = 1;
						else if (colorb2 >= 255) jj22 = 0;

						if (jj32 == 0) colorc2 -= 0.7;
						else colorc2 += 0.7;
						if (colorc2 <= 0) jj32 = 1;
						else if (colorc2 >= 255) jj32 = 0;

						ImGui::SameLine();
						ImGui::Image((PVOID)Image3, ImVec2(60, 60)); // replace with fangs
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora3 / 255.f, colorb3 / 255.f, colorc3 / 255.f, 255.f / 255.f));
						ImGui::Text("       ");
						ImGui::PopStyleColor();
						if (jj13 == 0) colora3 -= 0.7;
						else colora3 += 0.7;
						if (colora3 <= 0) jj13 = 1;
						else if (colora3 >= 255) jj13 = 0;

						if (jj23 == 0) colorb3 -= 0.7;
						else colorb3 += 0.7;
						if (colorb3 <= 0) jj23 = 1;
						else if (colorb3 >= 255) jj23 = 0;

						if (jj33 == 0) colorc3 -= 0.7;
						else colorc3 += 0.7;
						if (colorc3 <= 0) jj33 = 1;
						else if (colorc3 >= 255) jj33 = 0;

						ImGui::SameLine();
						ImGui::Image((PVOID)Image4, ImVec2(60, 60)); // replace with padlock user
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora4 / 255.f, colorb4 / 255.f, colorc4 / 255.f, 255.f / 255.f));
						ImGui::Text("       ");
						ImGui::PopStyleColor();
						if (jj14 == 0) colora4 -= 0.7;
						else colora4 += 0.7;
						if (colora4 <= 0) jj14 = 1;
						else if (colora4 >= 255) jj14 = 0;

						if (jj24 == 0) colorb4 -= 0.7;
						else colorb4 += 0.7;
						if (colorb4 <= 0) jj24 = 1;
						else if (colorb4 >= 255) jj24 = 0;

						if (jj34 == 0) colorc4 -= 0.7;
						else colorc4 += 0.7;
						if (colorc4 <= 0) jj34 = 1;
						else if (colorc4 >= 255) jj34 = 0;

						ImGui::SameLine();
						ImGui::Image((PVOID)Image5, ImVec2(60, 60)); // replace with programming
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora5 / 255.f, colorb5 / 255.f, colorc5 / 255.f, 255.f / 255.f));
						ImGui::Text("       ");
						ImGui::PopStyleColor();
						if (jj15 == 0) colora5 -= 0.7;
						else colora5 += 0.7;
						if (colora5 <= 0) jj15 = 1;
						else if (colora5 >= 255) jj15 = 0;

						if (jj25 == 0) colorb5 -= 0.7;
						else colorb5 += 0.7;
						if (colorb5 <= 0) jj25 = 1;
						else if (colorb5 >= 255) jj25 = 0;

						if (jj35 == 0) colorc5 -= 0.7;
						else colorc5 += 0.7;
						if (colorc5 <= 0) jj35 = 1;
						else if (colorc5 >= 255) jj35 = 0;
						ImGui::PopFont();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						if (ImGui::Button((u8"AIM"), ImVec2(170, 50)))
							tab_index = 1;
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();

						if (ImGui::Button((u8"ESP"), ImVec2(170, 50)))
							tab_index = 2;
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						if (ImGui::Button((u8"RAGE"), ImVec2(170, 50)))
							tab_index = 3;
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						if (ImGui::Button((u8"LEGIT"), ImVec2(170, 50)))
							tab_index = 4;
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						ImGui::Spacing();
						ImGui::SameLine();
						if (ImGui::Button((u8"MISC"), ImVec2(170, 50)))
							tab_index = 5;

						ImGui::Separator();

						if (tab_index == 1) {
							ImGui::Spacing();
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(153.f / 255.f, 153.f / 255.f, 255.f / 255.f, 255.f / 255.f));
							ImGui::BulletText(Config::nowhero.data());
							ImGui::PopStyleColor(1);
							ImGui::Spacing();
							if (ImGui::Button((u8"Main"), ImVec2(100, 30)))
								subindex = 1;
							ImGui::SameLine();
							ImGui::Spacing();
							ImGui::SameLine();
							ImGui::Spacing();
							ImGui::SameLine();
							if (ImGui::Button((u8"Secondary"), ImVec2(100, 30)))
								subindex = 2;
							ImGui::SameLine();
							ImGui::Spacing();
							ImGui::SameLine();
							ImGui::Spacing();
							ImGui::SameLine();
							if (ImGui::Button((u8"Aim Misc"), ImVec2(100, 30)))
								subindex = 3;
							if (subindex == 1) {
								ImGui::Spacing();

								ImGui::Spacing();
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
								ImGui::Toggle((u8"Trigger Bot"), &Config::triggerbot, ImGuiToggleFlags_Animated);
								if (Config::triggerbot) {
									Config::Flick = false;
									Config::hanzo_flick = false;
									Config::Tracking = false;
									Config::silent = false;
								}
								ImGui::Toggle((u8"Tracking"), &Config::Tracking, ImGuiToggleFlags_Animated);
								if (Config::Tracking) {
									Config::Flick = false;
									Config::hanzo_flick = false;
									Config::triggerbot = false;
									Config::silent = false;
								}
								ImGui::Toggle((u8"Flickbot"), &Config::Flick, ImGuiToggleFlags_Animated);
								if (Config::Flick) {
									Config::Tracking = false;
									Config::hanzo_flick = false;
									Config::triggerbot = false;
									Config::silent = false;
								}
								ImGui::Toggle((u8"Prediction"), &Config::Prediction, ImGuiToggleFlags_Animated);
								if (Config::Prediction) ImGui::Toggle((u8"Gravity Predict"), &Config::Gravitypredit, ImGuiToggleFlags_Animated);
								else Config::Gravitypredit = false;
								if (local_entity.HeroID == eHero::HERO_HANJO) {
									ImGui::Toggle((u8"Hanzo Flick"), &Config::hanzo_flick, ImGuiToggleFlags_Animated);
									ImGui::Toggle((u8"Auto Calculate Speed"), &Config::hanzoautospeed, ImGuiToggleFlags_Animated);
								}
								else Config::hanzoautospeed = false;
								if (Config::hanzo_flick) {
									Config::Flick = false;
									Config::triggerbot = false;
									Config::Tracking = false;
									Config::silent = false;
								}
								else Config::hanzo_flick = false;
								if (Config::hanzo_flick || Config::Prediction) {
									ImGui::Separator();
									ImGui::BulletText((u8"PredictLevel"));
									ImGui::SliderFloat((u8"BulletTravelSpeed"), &Config::predit_level, 0.f, 200.f, ("%.2f"));
								}
								if (Config::hanzo_flick || Config::Flick || Config::silent) {
									ImGui::Toggle((u8"AutoShoot"), &Config::AutoShoot, ImGuiToggleFlags_Animated);
									if (Config::AutoShoot) {
										ImGui::SliderInt((u8"Interval(ms)"), &Config::Shoottime, 0, 1500, ("%.2f"));
									}
								}
								ImGui::PopStyleColor(1);
								ImGui::BulletText((u8"Keybind"));
								if (ImGui::BeginCombo((u8"##Key"), keys))
								{
									for (int i = 0; i < 5; i++)
									{
										const bool type = keys == key_type[i];
										if (ImGui::Selectable(key_type[i], type))
										{
											keys = key_type[i];
											switch (i)
											{
											case 0:
												Config::aim_key = VK_LBUTTON;
												break;
											case 1:
												Config::aim_key = VK_RBUTTON;
												break;
											case 2:
												Config::aim_key = VK_MBUTTON;
												break;
											case 3:
												Config::aim_key = VK_XBUTTON1;
												break;
											case 4:
												Config::aim_key = VK_XBUTTON2;
												break;
											}
										}
									}
									ImGui::EndCombo();
								}
								ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.6f, 1.f, 1.0f));
								ImGui::Spacing();
								ImGui::Spacing();
								ImGui::Spacing();
								ImGui::Spacing();
								ImGui::Spacing();
								ImGui::Spacing();
								ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
								ImGuiKnobs::Knob((u8"FOV(MIN)"), &Config::minFov1, 1.f, 500.f, 1.f, "%.1f", ImGuiKnobVariant_Space, 100.f);
								if (!Config::autoscalefov)
									Config::Fov = Config::minFov1;
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGuiKnobs::Knob((u8"HitBox"), &Config::hitbox, 0.05f, 0.28f, 0.0001f, "%.2f", ImGuiKnobVariant_Stepped, 100.f);
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								if (local_entity.HeroID != eHero::HERO_GENJI) {
									ImGuiKnobs::Knob((u8"Tracking Speed"), &Config::Tracking_smooth, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
								}
								else {
									ImGuiKnobs::Knob((u8"Blade(Turn around speed)"), &Config::Tracking_smooth, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGuiKnobs::Knob((u8"Blade Speed"), &Config::bladespeed, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
								}
								ImGuiKnobs::Knob((u8"Flick Speed"), &Config::Flick_smooth, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGuiKnobs::Knob((u8"Tracking Accelerate"), &Config::accvalue, 0.01f, 1.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
								ImGui::Separator();
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::PopStyleColor(1);
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
								ImGui::Toggle((u8"Closest"), &Config::autobone, ImGuiToggleFlags_Animated);
								ImGui::Toggle((u8"Main Aim ally"), &Config::switch_team, ImGuiToggleFlags_Animated);
								if (!Config::autobone) {
									if (Config::Bone == 1) {
										Config::BoneName = (u8"Head");
									}
									else if (Config::Bone == 2) {
										Config::BoneName = (u8"Neck");
									}
									else {
										Config::BoneName = (u8"Chest");
									}
									ImGui::Spacing();
									ImGui::SliderInt(Config::BoneName, &Config::Bone, 1, 3);
								}
								ImGui::PopStyleColor(1);
							}

							if (subindex == 2) {
								ImGui::BulletText(u8"SecondAim doesnt available with silent and legit");
								ImGui::Spacing();
								ImGui::Spacing();
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
								ImGui::Toggle((u8"Secondary aim on/off"), &Config::secondaim, ImGuiToggleFlags_Animated);
								ImGui::PopStyleColor(1);
								if (Config::secondaim) {
									ImGui::Spacing();
									ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
									ImGui::Toggle((u8"High priority"), &Config::highPriority, ImGuiToggleFlags_Animated);
									ImGui::Toggle((u8"FlickBot2"), &Config::Flick2, ImGuiToggleFlags_Animated);
									if (Config::Flick2) Config::Tracking2 = false;
									ImGui::Toggle((u8"Tracking2"), &Config::Tracking2, ImGuiToggleFlags_Animated);
									if (Config::Tracking2) Config::Flick2 = false;
									ImGui::Toggle((u8"Prediction2"), &Config::Prediction2, ImGuiToggleFlags_Animated);
									ImGui::Toggle((u8"Secondary Aim ally"), &Config::switch_team2, ImGuiToggleFlags_Animated);

									if (Config::Prediction2) ImGui::Toggle((u8"Gravity Predict2"), &Config::Gravitypredit2, ImGuiToggleFlags_Animated);
									else Config::Gravitypredit2 = false;

									if (Config::Prediction2) {
										ImGui::Separator();
										ImGui::BulletText((u8"Predict level2："));
										ImGui::SliderFloat((u8"Bullet Travel Speed 2"), &Config::predit_level2, 0.f, 200.f, ("%.2f"));
									}

									ImGui::PopStyleColor(1);
									ImGui::BulletText((u8"Keybind2"));

									if (ImGui::BeginCombo((u8"##Key2"), keys2))
									{
										for (int i = 0; i < 5; i++)
										{
											const bool type2 = keys2 == key_type2[i];
											if (ImGui::Selectable(key_type2[i], type2))
											{
												keys2 = key_type2[i];
												switch (i)
												{
												case 0:
													Config::aim_key2 = VK_LBUTTON;
													break;
												case 1:
													Config::aim_key2 = VK_RBUTTON;
													break;
												case 2:
													Config::aim_key2 = VK_MBUTTON;
													break;
												case 3:
													Config::aim_key2 = VK_XBUTTON1;
													break;
												case 4:
													Config::aim_key2 = VK_XBUTTON2;
													break;
												}
											}
										}
										ImGui::EndCombo();
									}

									ImGui::BulletText((u8"Toggle button2"));

									if (ImGui::BeginCombo((u8"##FIREKey2"), keys3))
									{
										for (int i = 0; i < 5; i++)
										{
											const bool type3 = keys3 == key_type3[i];
											if (ImGui::Selectable(key_type3[i], type3))
											{
												keys3 = key_type3[i];
												switch (i)
												{
												case 0:
													Config::togglekey = 0;
													break;
												case 1:
													Config::togglekey = 1;
													break;
												case 2:
													Config::togglekey = 2;
													break;
												case 3:
													Config::togglekey = 3;
													break;
												case 4:
													Config::togglekey = 4;
													break;
												}
											}
										}
										ImGui::EndCombo();
									}

									ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
									ImGui::Toggle((u8"Closest"), &Config::autobone2, ImGuiToggleFlags_Animated);
									ImGui::PopStyleColor(1);
									if (!Config::autobone2) {
										ImGui::Spacing();
										if (Config::Bone2 == 1) {
											Config::BoneName2 = (u8"Head");
										}
										else if (Config::Bone2 == 2) {
											Config::BoneName2 = (u8"Neck");
										}
										else {
											Config::BoneName2 = (u8"Chest");
										}
										ImGui::SliderInt(Config::BoneName2, &Config::Bone2, 1, 3);
									}

									ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.6f, 1.f, 1.0f));
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
									ImGuiKnobs::Knob((u8"FOV2(MIN)"), &Config::minFov2, 1.f, 500.f, 1.f, "%.1f", ImGuiKnobVariant_Space, 100.f);
									if (!Config::autoscalefov)
										Config::Fov2 = Config::minFov2;
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGuiKnobs::Knob((u8"HitBox2"), &Config::hitbox2, 0.05f, 0.28f, 0.0001f, "%.2f", ImGuiKnobVariant_Stepped, 100.f);
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGuiKnobs::Knob((u8"Tracking Speed2"), &Config::Tracking_smooth2, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGuiKnobs::Knob((u8"Flick Speed2"), &Config::Flick_smooth2, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGuiKnobs::Knob((u8"Flick acc 2"), &Config::accvalue2, 0.01f, 1.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
									ImGui::Separator();
									ImGui::Spacing();
									ImGui::SameLine();
									ImGui::PopStyleColor(1);
								}
							}
							if (subindex == 3) {
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
								ImGui::Toggle((u8"AI AIM(Beta)"), &Config::aiaim, ImGuiToggleFlags_Animated);
								ImGui::Toggle((u8"Auto FOV"), &Config::autoscalefov, ImGuiToggleFlags_Animated);
								if (!Config::autoscalefov) {
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
								ImGui::Toggle((u8"Lock On Target"), &Config::lockontarget, ImGuiToggleFlags_Animated);
								ImGui::Toggle((u8"Track Compensate"), &Config::trackcompensate, ImGuiToggleFlags_Animated);
								ImGui::SliderFloat((u8"Compensate Area"), &Config::comarea, 0.f, 0.1f, ("%.3f"));
								ImGui::SliderFloat((u8"Compensate Speed"), &Config::comspeed, 0.f, 1.f, ("%.2f"));
								ImGui::Toggle((u8"Vertical Recoil Restrain"), &Config::norecoil, ImGuiToggleFlags_Animated);
								ImGui::SliderFloat((u8"Restrain Value"), &Config::recoilnum, 0.f, 1.f, ("%.2f"));
								ImGui::Toggle((u8"Disable Horizontal Recoil"), &Config::horizonreco, ImGuiToggleFlags_Animated);
								ImGui::PopStyleColor(1);
								if (local_entity.HeroID == eHero::HERO_GENJI) {
									ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
									ImGui::Toggle((u8"Genji Blade"), &Config::GenjiBlade, ImGuiToggleFlags_Animated);
									ImGui::Toggle((u8"Genji Auto Dash"), &Config::AutoShiftGenji, ImGuiToggleFlags_Animated);
									ImGui::PopStyleColor(1);
								}
								else {
									Config::GenjiBlade = false;
									Config::AutoShiftGenji = false;
								}
								if (local_entity.HeroID == eHero::HERO_WIDOWMAKER) {
									ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
									ImGui::Toggle((u8"Auto Unscope"), &Config::widowautounscope, ImGuiToggleFlags_Animated);
									ImGui::PopStyleColor(1);
								}
								else Config::widowautounscope = false;
							}
						}
						if (tab_index == 2) {
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
							ImGui::Toggle((u8"Info"), &Config::draw_info, ImGuiToggleFlags_Animated);
							if (Config::draw_info) {
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Toggle((u8"BattleTag"), &Config::drawbattletag, ImGuiToggleFlags_Animated);
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Toggle((u8"HP"), &Config::drawhealth, ImGuiToggleFlags_Animated);
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Toggle((u8"HP BAR"), &Config::healthbar, ImGuiToggleFlags_Animated);
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Toggle((u8"Distance"), &Config::dist, ImGuiToggleFlags_Animated);
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Toggle((u8"Name"), &Config::name, ImGuiToggleFlags_Animated);
								ImGui::Spacing();
								ImGui::SameLine();
								ImGui::Toggle((u8"Ult"), &Config::ult, ImGuiToggleFlags_Animated);
							}
							ImGui::Toggle((u8"testCrossHair"), &Config::crosscircle, ImGuiToggleFlags_Animated);
							ImGui::Toggle((u8"Bone"), &Config::draw_skel, ImGuiToggleFlags_Animated);
							ImGui::Toggle((u8"Radar"), &Config::radar, ImGuiToggleFlags_Animated);
							if (Config::radar) {
								ImGui::Toggle((u8"Radar Line"), &Config::radarline, ImGuiToggleFlags_Animated);
							}
							else Config::radarline = false;
							ImGui::Toggle((u8"Ult info"), &Config::skillinfo, ImGuiToggleFlags_Animated);
							ImGui::PopStyleColor(1);
							ImGui::Spacing();
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
							ImGui::Toggle((u8"HeadShotLine"), &Config::eyeray, ImGuiToggleFlags_Animated);
							ImGui::Toggle((u8"Enemy/Ally Outline"), &Config::externaloutline, ImGuiToggleFlags_Animated);
							ImGui::Toggle((u8"Only Ally Outline"), &Config::teamoutline, ImGuiToggleFlags_Animated);
							ImGui::Toggle((u8"Outline hp colour"), &Config::healthoutline, ImGuiToggleFlags_Animated);
							if (Config::healthoutline) Config::rainbowoutline = 0;
							ImGui::Toggle((u8"Outline rainbow colour"), &Config::rainbowoutline, ImGuiToggleFlags_Animated);
							if (Config::rainbowoutline) Config::healthoutline = 0;
							ImGui::Toggle((u8"Health Pack"), &Config::draw_hp_pack, ImGuiToggleFlags_Animated);
							ImGui::Toggle((u8"2D box"), &Config::draw_edge, ImGuiToggleFlags_Animated);
							ImGui::Toggle((u8"3D box"), &Config::drawbox3d, ImGuiToggleFlags_Animated);
							ImGui::Toggle((u8"Line"), &Config::drawline, ImGuiToggleFlags_Animated);
							ImGui::PopStyleColor(1);
							ImGui::ColorEdit3((u8"Box colour"), (float*)&Config::EnemyCol);
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
							ImGui::Toggle((u8"Draw Fov"), &Config::draw_fov, ImGuiToggleFlags_Animated);
							ImGui::PopStyleColor(1);
							ImGui::ColorEdit3((u8"FOV colour"), (float*)&Config::fovcol);
							ImGui::ColorEdit3((u8"FOV2 colour"), (float*)&Config::fovcol2);
							ImGui::BulletText((u8"outline colour"));
							ImGui::ColorEdit3((u8"Invisible Enemy"), (float*)&Config::invisenargb);
							ImGui::SliderFloat((u8"Invisible Enemy Highlight"), &Config::invisenargb.w, 0.0f, 1.f, ("%.2f"));
							ImGui::ColorEdit3((u8"Visible Enemy"), (float*)&Config::enargb);
							ImGui::SliderFloat((u8"Visible Enemy Highlight"), &Config::enargb.w, 0.0f, 1.f, ("%.2f"));
							ImGui::ColorEdit3((u8"Target"), (float*)&Config::targetargb);
							ImGui::SliderFloat((u8"Target Highlight"), &Config::targetargb.w, 0.0f, 1.f, ("%.2f"));
							ImGui::ColorEdit3((u8"Target2"), (float*)&Config::targetargb2);
							ImGui::SliderFloat((u8"Target2 Highlight"), &Config::targetargb2.w, 0.0f, 1.f, ("%.2f"));
							ImGui::ColorEdit3((u8"Ally"), (float*)&Config::allyargb);
							ImGui::SliderFloat((u8"Ally Highlight"), &Config::allyargb.w, 0.0f, 1.f, ("%.2f"));
							ImGui::Separator();
						}
						if (tab_index == 3) {
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
							ImGui::Toggle((u8"Aim Instantly"), &Config::Rage, ImGuiToggleFlags_Animated);
							ImGui::SameLine(); Render::Help((u8"Aim Instantly"));
							if (Config::Rage) {
								ImGui::Toggle((u8"Fake Silent"), &Config::fakesilent, ImGuiToggleFlags_Animated);
							}
							ImGui::Toggle((u8"Non fov limit"), &Config::fov360, ImGuiToggleFlags_Animated);
							if (!Config::fov360 && Config::Fov == 6000) Config::Fov = 200;
							if (!Config::fov360 && Config::Fov2 == 6000) Config::Fov2 = 200;
							ImGui::BulletText((u8"Risky"));
							ImGui::PopStyleColor(1);
						}
						if (tab_index == 4) {
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
							ImGui::BulletText((u8"Play more legit"));

							ImGui::Toggle((u8"Auto miss FLICK ONLY"), &Config::dontshot, ImGuiToggleFlags_Animated);

							ImGui::SliderFloat((u8"miss hitbox(missbox)"), &Config::missbox, 0.f, 1.f, ("%.2f"));
							ImGui::SliderInt((u8"miss frequency"), &Config::shotmanydont, 0, 6);
							ImGui::Spacing();
							ImGui::Toggle((u8"Switching target delay"), &Config::targetdelay, ImGuiToggleFlags_Animated);

							ImGui::SliderInt((u8"Delay(ms)"), &Config::targetdelaytime, 0, 1000, ("%.2f"));
							ImGui::Toggle((u8"Shoot when Timeout"), &Config::hitboxdelayshoot, ImGuiToggleFlags_Animated);

							ImGui::SliderInt((u8":timeout(ms)"), &Config::hiboxdelaytime, 0, 1000, ("%.2f"));
							ImGui::PopStyleColor(1);

						}
						if (tab_index == 5) {
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
							ImGui::Toggle((u8"Change ingame fov"), &Config::enablechangefov, ImGuiToggleFlags_Animated);
							if (Config::enablechangefov)
								ImGui::SliderFloat((u8":degree"), &Config::CHANGEFOV, 1.f, 179.f, ("%.2f"));

							ImGui::Toggle((u8"Name Spoofer"), &Config::namespoofer, ImGuiToggleFlags_Animated);
							if (Config::namespoofer) {
								ImGui::InputText(u8"Spoof Name", Config::fakename, sizeof(Config::fakename));
							}

							ImGui::Toggle((u8"Auto melee"), &Config::AutoMelee, ImGuiToggleFlags_Animated);
							ImGui::PopStyleColor(1);
							ImGui::SliderFloat((u8"Auto melee HP"), &Config::meleehealth, 0.f, 80.f, ("%.2f"));
							ImGui::SliderFloat((u8"Auto melee dist"), &Config::meleedistance, 0.f, 10.f, ("%.2f"));
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
							ImGui::Toggle((u8"Auto Heal Skill"), &Config::AutoSkill, ImGuiToggleFlags_Animated);
							ImGui::SliderFloat((u8"Auto Heal Skill Trigger HP"), &Config::SkillHealth, 0.f, 80.f, ("%.2f"));
							ImGui::PopStyleColor(1);
							
							if (ImGui::Button(u8"Manual Save"))
							{
								Config::manualsave = true;
								ImGui::InsertNotification({ ImGuiToastType_Success, 3000, (u8"Manual saved successfully"), "" });
							}
							ImGui::BulletText((u8"Press when nothing is working"));
							if (ImGui::Button(u8"Reboot thread")) {
								Config::doingentity = 0;
								Sleep(1000);
								abletotread = 0;
								Config::doingentity = 1;
								_beginthread((_beginthread_proc_type)entity_scan_thread, 0, 0);
								Sleep(51);
								_beginthread((_beginthread_proc_type)entity_thread, 0, 0);
								Sleep(51);
								ImGui::InsertNotification({ ImGuiToastType_Success, 3000, (u8"Reboot Successfully"), "" });
							}
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora5 / 255.f, colorb5 / 255.f, colorc5 / 255.f, 255.f / 255.f));
							ImGui::Text(u8"Expires in：%dDays %dHours %dMins %dSecs", (expiretime - nowtime) / 60 / 60 / 24, ((expiretime - nowtime) / 60 / 60) % 24, ((expiretime - nowtime) / 60) % 60, (expiretime - nowtime) % 60);
							if (Config::loginornot && (expiretime - nowtime) < 0) abort();
							ImGui::Text(u8"FPS: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
							ImGui::PopStyleColor();
							ImGui::End();
						}
					}
					ImGui::End();
				}
				if (entities.size() > 0)
					esp();
				ImGui::EndFrame();
				ImGui::Render();
				g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
				auto ss = ImVec4(0, 0, 0, 0);
				g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&ss);
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

				g_pSwapChain->Present(1, 0);
			}
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			CleanupDeviceD3D();
			::DestroyWindow(hwnd);
			::UnregisterClass(wc.lpszClassName, wc.hInstance);
		}
		__except (1) {

		}
	}
	inline void aimbot_thread()
	{
		__try {
			int hitbotdelaytime = 0;
			int afterdelaytime = 0;
			bool dodelay = 0;
			TCHAR buf[100];
			Vector2 CrossHair = Vector2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);
			static float origin_sens = 0.f;
			while (true) {
				if (entities.size() > 0) {
					if (SDK->RPM<float>(GetSenstivePTR()))
						origin_sens = SDK->RPM<float>(GetSenstivePTR());
					else if (origin_sens)
						SDK->WPM<float>(GetSenstivePTR(), origin_sens);
					if (Config::triggerbot && GetAsyncKeyState(Config::aim_key)) {
						auto vec = GetVector3(Config::Prediction ? true : false);
						if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI)) {
							auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
							auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
							auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
							auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
							if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::hitbox)) {
								if (Config::lockontarget)
									SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x1);
								Sleep(2);
								if (Config::lockontarget)
									SDK->WPM<float>(GetSenstivePTR(), origin_sens);
							}
						}
					}
					if (Config::Tracking) {

						while (GetAsyncKeyState(Config::aim_key) && !Config::reloading)
						{
							auto vec = GetVector3(Config::Prediction ? true : false);
							if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI) && ((!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) || Config::switch_team)) {
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (Config::aiaim) {
									int x;
									if (rand() % 10 > 5)
										Target.X += (double)(rand()) / RAND_MAX / 500;
									else Target.X -= (double)(rand()) / RAND_MAX / 500;
									if (rand() % 10 > 5)
										Target.Y += (double)(rand()) / RAND_MAX / 500;
									else Target.Y -= (double)(rand()) / RAND_MAX / 500;
									if (rand() % 10 > 5)
										Target.Z += (double)(rand()) / RAND_MAX / 500;
									else Target.Z -= (double)(rand()) / RAND_MAX / 500;
									if (Config::minFov1 > 500)Config::minFov1 = 500;
									if (Config::Fov > 500)Config::Fov = 500;
									if (Config::minFov2 > 500)Config::minFov1 = 500;
									if (Config::Fov2 > 500)Config::Fov = 500;
									if (Config::fov360) Config::fov360 = false;
								}
								if (Target != Vector3(0, 0, 0)) {
									if (Config::targetdelay) {
										if (Config::lastenemy != Config::Targetenemyi)  Config::doingdelay = 1;
										if (Config::doingdelay == 1) {
											Config::lastenemy = Config::Targetenemyi;
											if (Config::timebeforedelay == 0) {
												Config::timebeforedelay = GetTickCount();
												continue;
											}
											if (GetTickCount() - Config::timebeforedelay < Config::targetdelaytime) continue;
											else Config::timebeforedelay = 0;
											Config::doingdelay = 0;
										}
									}
									if (Config::Rage) SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, vec_calc_target);
									else
										SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, Target);
									float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
									if (Config::health <= Config::meleehealth && dist <= Config::meleedistance && Config::AutoMelee) {
										SetKey(0x800);
									}

								}
								if (local_entity.PlayerHealth < Config::SkillHealth) {
									break;
								}
							}
							Sleep(1);
							if (Config::autoscalefov) {
								auto vec = GetVector3forfov();
								if (vec != Vector3(0, 0, 0)) {
									Vector2 high;
									Vector2 low;
									if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
									{
										Config::Fov = -(high.Y - low.Y) * 4;
										if (Config::Fov > 500) Config::Fov = 500;
										else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
										Config::Fov2 = -(high.Y - low.Y) * 4;
										if (Config::Fov2 > 500) Config::Fov2 = 500;
										else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
									}
									else
									{
										Config::Fov = Config::minFov1;
										Config::Fov2 = Config::minFov2;
									}

								}
								else
								{
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
							}
							if (Config::highPriority && GetAsyncKeyState(Config::aim_key2)) break;
							else if (Config::highPriority && !GetAsyncKeyState(Config::aim_key2) && Config::shooted2) Config::shooted2 = false;
						}
					}
					else if (Config::Flick) {
						if (Config::hitboxdelayshoot) {
							if (Config::shooted || !GetAsyncKeyState(Config::aim_key)) {
								dodelay = 1;
								hitbotdelaytime = 0;
							}
						}
						while (GetAsyncKeyState(Config::aim_key) && !Config::shooted && !Config::reloading)
						{
							auto vec = GetVector3(Config::Prediction ? true : false);
							if (vec == Vector3(0, 0, 0)) break;
							if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI) && ((!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) || Config::switch_team)) {
								if (Config::targetdelay) {//目标切换延迟 doingdelay
									if (Config::lastenemy != Config::Targetenemyi)  Config::doingdelay = 1;
									if (Config::doingdelay == 1) {
										Config::lastenemy = Config::Targetenemyi;
										if (Config::timebeforedelay == 0) {
											Config::timebeforedelay = GetTickCount();
											continue;
										}
										if (GetTickCount() - Config::timebeforedelay < Config::targetdelaytime) continue;
										else Config::timebeforedelay = 0;
										Config::doingdelay = 0;
										hitbotdelaytime = GetTickCount();
									}
								}
								else if (Config::doingdelay) Config::doingdelay = 0;
								if (dodelay && !Config::doingdelay) {
									hitbotdelaytime = GetTickCount();
									dodelay = 0;
								}
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothAccelerate(local_angle, vec_calc_target, Config::Flick_smooth / 10.f, Config::accvalue);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (Config::aiaim) {
									int x;
									if (rand() % 10 > 5)
										Target.X += (double)(rand()) / RAND_MAX / 300;
									else Target.X -= (double)(rand()) / RAND_MAX / 300;
									if (rand() % 10 > 5)
										Target.Y += (double)(rand()) / RAND_MAX / 300;
									else Target.Y -= (double)(rand()) / RAND_MAX / 300;
									if (rand() % 10 > 5)
										Target.Z += (double)(rand()) / RAND_MAX / 300;
									else Target.Z -= (double)(rand()) / RAND_MAX / 300;
									if (Config::minFov1 > 500)Config::minFov1 = 500;
									if (Config::Fov > 500)Config::Fov = 500;
									if (Config::minFov2 > 500)Config::minFov1 = 500;
									if (Config::Fov2 > 500)Config::Fov = 500;
									if (Config::fov360) Config::fov360 = false;
								}
								if (Target != Vector3(0, 0, 0)) {
									if (Config::hitboxdelayshoot && hitbotdelaytime != 0) {
										afterdelaytime = GetTickCount();
										if (afterdelaytime - hitbotdelaytime > Config::hiboxdelaytime && !Config::doingdelay) {
											if (local_entity.HeroID == eHero::HERO_GENJI || local_entity.HeroID == eHero::HERO_KIRIKO) {
												SetKey(0x2);
											}
											else
												SetKey(0x1);
											Config::shooted = true;
											continue;
										}
									}
									if (Config::Rage) {
										if (Config::fakesilent) {
											Vector3 orangle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
											SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, vec_calc_target);
											if (Config::lockontarget)
												SDK->WPM<float>(GetSenstivePTR(), 0);
											SetKey(0x1);
											Sleep(25);
											if (Config::lockontarget)
												SDK->WPM<float>(GetSenstivePTR(), origin_sens);
											Config::shooted = true;
											SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, orangle);
											continue;
										}
										else {
											SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, vec_calc_target);
											if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), 0);
											SetKey(0x1);
											Sleep(1);
											if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), origin_sens);
											Config::shooted = true;
											continue;
										}
									}
									SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, Target);
									if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::hitbox)) {
										if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), 0);
										if (local_entity.HeroID == eHero::HERO_GENJI || local_entity.HeroID == eHero::HERO_KIRIKO) {
											SetKey(0x2);
											if (Config::dontshot) Config::shotcount++;
										}
										else {
											if ((local_entity.HeroID == eHero::HERO_ANA || local_entity.HeroID == eHero::HERO_WIDOWMAKER || local_entity.HeroID == eHero::HERO_ASHE) && GetAsyncKeyState(0x2))
												SetKeyscopeHold(0x1, 30);
											else SetKey(0x1);
										}
										if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), origin_sens);
										Config::shooted = true;
										if (Config::dontshot) Config::shotcount++;
										break;
										
									}
									else if (Config::dontshot && Config::shotcount >= Config::shotmanydont) {
										if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::missbox)) {
											Config::shotcount = 0;
											if (local_entity.HeroID == eHero::HERO_GENJI || local_entity.HeroID == eHero::HERO_KIRIKO) {
												SetKey(0x2);
												if (!Config::sskilled) Sleep(10);
											}
											else {
												SetKey(0x1);
											}
											Config::shooted = true;
											continue;
										}
									}
								}
								else hitbotdelaytime = 0;

							}
							Sleep(1);
							if (Config::autoscalefov) {
								auto vec = GetVector3forfov();
								if (vec != Vector3(0, 0, 0)) {
									Vector2 high;
									Vector2 low;
									if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
									{
										Config::Fov = -(high.Y - low.Y) * 4;
										if (Config::Fov > 500) Config::Fov = 500;
										else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
										Config::Fov2 = -(high.Y - low.Y) * 4;
										if (Config::Fov2 > 500) Config::Fov2 = 500;
										else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
									}
									else
									{
										Config::Fov = Config::minFov1;
										Config::Fov2 = Config::minFov2;
									}

								}
								else
								{
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
							}
							if (Config::highPriority && GetAsyncKeyState(Config::aim_key2)) break;
							else if (Config::highPriority && !GetAsyncKeyState(Config::aim_key2) && Config::shooted2) Config::shooted2 = false;
						}
					}
					else if (Config::hanzo_flick)
					{
						if (Config::hitboxdelayshoot) {
							if (Config::shooted || !GetAsyncKeyState(Config::aim_key)) {
								dodelay = 1;
								hitbotdelaytime = 0;
							}
						}
						while (GetAsyncKeyState(Config::aim_key) && !Config::shooted)
						{
							auto vec = GetVector3(true);
							if (vec == Vector3(0, 0, 0)) {
								break;
							}
							if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI) && ((!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) || Config::switch_team)) {

								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothAccelerate(local_angle, vec_calc_target, Config::Flick_smooth / 10.f, Config::accvalue);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (dodelay && !Config::doingdelay) {
									hitbotdelaytime = GetTickCount();
									dodelay = 0;
								}
								if (Config::aiaim) {
									int x;
									if (rand() % 10 > 5)
										Target.X += (double)(rand()) / RAND_MAX / 300;
									else Target.X -= (double)(rand()) / RAND_MAX / 300;
									if (rand() % 10 > 5)
										Target.Y += (double)(rand()) / RAND_MAX / 300;
									else Target.Y -= (double)(rand()) / RAND_MAX / 300;
									if (rand() % 10 > 5)
										Target.Z += (double)(rand()) / RAND_MAX / 300;
									else Target.Z -= (double)(rand()) / RAND_MAX / 300;
									if (Config::minFov1 > 500)Config::minFov1 = 500;
									if (Config::Fov > 500)Config::Fov = 500;
									if (Config::minFov2 > 500)Config::minFov1 = 500;
									if (Config::Fov2 > 500)Config::Fov = 500;
									if (Config::fov360) Config::fov360 = false;
								}
								if (Target != Vector3(0, 0, 0)) {
									if (Config::hitboxdelayshoot && hitbotdelaytime != 0) {
										afterdelaytime = GetTickCount();
										if (afterdelaytime - hitbotdelaytime > Config::hiboxdelaytime && !Config::doingdelay) {
											if (local_entity.skill2act) SetKey(0x1);
											else SetKeyHold(0x1000, 100);
											Config::shooted = true;
											continue;
										}
									}
									if (Config::Rage) {
										if (Config::fakesilent) {
											Vector3 orangle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
											SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, vec_calc_target);
											if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), 0);
											if (local_entity.skill2act) SetKey(0x1);
											else SetKeyHold(0x1000, 100);
											Sleep(25);
											if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), origin_sens);
											Config::shooted = true;
											SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, orangle);
											continue;
										}
										else
										{
											SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, vec_calc_target);
											if (local_entity.skill2act) SetKey(0x1);
											else SetKeyHold(0x1000, 100);
											Config::shooted = true;
											continue;
										}
									}

									if (Config::targetdelay) {
										if (Config::lastenemy != Config::Targetenemyi)  Config::doingdelay = 1;
										if (Config::doingdelay == 1) {
											Config::lastenemy = Config::Targetenemyi;
											if (Config::timebeforedelay == 0) {
												Config::timebeforedelay = GetTickCount();
												continue;
											}
											if (GetTickCount() - Config::timebeforedelay < Config::targetdelaytime) continue;
											else Config::timebeforedelay = 0;
											Config::doingdelay = 0;
											hitbotdelaytime = GetTickCount();
										}
									}
									else if (Config::doingdelay) Config::doingdelay = 0;
									SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, Target);
									if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::hitbox)) {
										if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), 0);
										if (local_entity.skill2act) SetKey(0x1);
										else SetKeyHold(0x1000, 100);
										Sleep(1);
										if (Config::dontshot) Config::shotcount++;
										if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), origin_sens);
										Config::shooted = true;
									}
									else if (Config::dontshot && Config::shotcount >= Config::shotmanydont) {
										if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::missbox)) {
											Config::shotcount = 0;
											if (local_entity.skill2act) {
												SetKey(0x1);
											}
											else {
												SetKeyHold(0x1000, 100);
											}
											Config::shooted = true;
											continue;
										}
									}
								}
								else hitbotdelaytime = 0;
							}
							Sleep(1);
							if (Config::autoscalefov) {
								auto vec = GetVector3forfov();
								if (vec != Vector3(0, 0, 0)) {
									Vector2 high;
									Vector2 low;
									if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
									{
										Config::Fov = -(high.Y - low.Y) * 4;
										if (Config::Fov > 500) Config::Fov = 500;
										else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
										Config::Fov2 = -(high.Y - low.Y) * 4;
										if (Config::Fov2 > 500) Config::Fov2 = 500;
										else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
									}
									else
									{
										Config::Fov = Config::minFov1;
										Config::Fov2 = Config::minFov2;
									}

								}
								else
								{
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
							}
						}
					}
					if (Config::GenjiBlade && GetAsyncKeyState(0x51) && local_entity.HeroID == eHero::HERO_GENJI && local_entity.ultimate == 100) {
						Config::Qstarttime = GetTickCount();
						Config::Qtime = Config::Qstarttime;
						Config::lastenemy = -1;
						Sleep(1000);
						int detecttoggle = 0;
						int first = 1;
						float speed = 0;
						while (Config::GenjiBlade && (Config::Qtime - Config::Qstarttime) <= 7000) {
							if (!local_entity.skillcd1) speed = Config::Tracking_smooth;
							else speed = Config::bladespeed;
							Config::Qtime = GetTickCount();
							auto vec = GetVector3forgenji();
							if (vec != Vector3(0, 0, 0)) {
								float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
								if (dist > 20) continue;
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothLinear(local_angle, vec_calc_target, speed / 10.f);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);

								if (Target != Vector3(0, 0, 0)) {
									float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
									if ((!local_entity.skillcd1 && dist < 20) || dist < 7)
									{
										if (Config::Rage) SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, vec_calc_target);
										else SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, Target);
									}
									if (!local_entity.skillcd1 && in_range(local_angle, vec_calc_target, local_loc, vec, 0.8)) {
										if (detecttoggle && !first) {
											detecttoggle = 0;
											Sleep(50);
											continue;
										}
										SetKeyHold(0x8, 70);
										first = 0;
									}

									if (in_range(local_angle, vec_calc_target, local_loc, vec, 1) && dist < 5) {
										SetKey(0x1);
									}
									if (local_entity.skillcd1 != 0 && !detecttoggle) {
										detecttoggle = 1;
									}
								}
							}
							Sleep(1);
							Config::lastenemy = Config::Targetenemyi;

						}
					}
					if (Config::autoscalefov) {
						auto vec = GetVector3forfov();
						if (vec != Vector3(0, 0, 0)) {
							Vector2 high;
							Vector2 low;
							if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
							{
								Config::Fov = -(high.Y - low.Y) * 4;
								if (Config::Fov > 500) Config::Fov = 500;
								else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
								Config::Fov2 = -(high.Y - low.Y) * 4;
								if (Config::Fov2 > 500) Config::Fov2 = 500;
								else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
							}
							else
							{
								Config::Fov = Config::minFov1;
								Config::Fov2 = Config::minFov2;
							}

						}
						else
						{
							Config::Fov = Config::minFov1;
							Config::Fov2 = Config::minFov2;
						}
					}
					if (Config::AutoMelee) {
						auto vec = GetVector3(false);
						if (vec != Vector3(0, 0, 0) && entities[Config::Targetenemyi].Team) {
							float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
							if (Config::health <= Config::meleehealth && dist <= Config::meleedistance) {
								SetKey(0x800);
								Sleep(1);
							}
						}
					}
					if (Config::AutoShiftGenji) {
						auto vec = GetVector3(false);
						if (vec != Vector3(0, 0, 0)) {
							float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
							if (!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) {
								if (!local_entity.skillcd1 && Config::health <= 50 && dist <= 15 && entities[Config::Targetenemyi].HeroID != 0x16dd && entities[Config::Targetenemyi].HeroID != 0x16ee) {
									auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
									auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
									auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
									auto Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
									auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
									if (in_range(local_angle, vec_calc_target, local_loc, vec, 1)) {
										SetKeyHold(0x8, 40);
									}
								}
								else if (!local_entity.skillcd1 && Config::health <= 80 && dist <= 17 && dist >= 15 && entities[Config::Targetenemyi].HeroID != 0x16dd && entities[Config::Targetenemyi].HeroID != 0x16ee) {
									auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
									auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
									auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
									auto Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
									auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
									if (in_range(local_angle, vec_calc_target, local_loc, vec, 1)) {
										SetKey(0x8);
										Sleep(500);
										SetKey(0x800);
									}
								}
							}
						}
					}

					if (Config::AutoSkill) {
						if (local_entity.PlayerHealth > Config::SkillHealth && Config::skilled) {
							Config::skilled = false;
						}
						else if (local_entity.PlayerHealth < Config::SkillHealth && Config::skilled && local_entity.PlayerHealth < Config::lasthealth && local_entity.HeroID != eHero::HERO_DOOMFIST) {
							Config::skilled = false;
						}
						if (local_entity.PlayerHealth < Config::SkillHealth) {
							if (local_entity.HeroID == eHero::HERO_TRACER && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x10);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_SOMBRA && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x10);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_ROADHOG && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x10);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_TORBJORN && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x10);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_SOLDIER76 && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x10);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_VENTURE && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x10);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_REAPER && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x8);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_MEI && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x8);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_DOOMFIST && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x20);
								Sleep(10);
								SetKey(0x10);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_JUNKERQUEEN && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x8);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_MOIRA && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x8);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_ZARYA && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x8);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_WINSTON && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x20);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_ZENYATTA && local_entity.PlayerHealth != 0 && !Config::skilled) {
								SetKey(0x20);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
						}

					}
					if (Config::AutoShoot && Config::shooted && !(local_entity.HeroID == eHero::HERO_HANJO && !local_entity.skill2act)) {
						int rectime = GetTickCount();
						if (Config::lasttime == 0) Config::lasttime = rectime;
						else {
							int mtime = rectime - Config::lasttime;
							if (mtime >= Config::Shoottime) {
								Config::lasttime = 0;
								Config::shooted = false;
							}
						}
						if (Config::reloading) {
							Config::lasttime = 0;
							Config::shooted = false;
						}
					}
					if (!GetAsyncKeyState(Config::aim_key)) {
						Config::shooted = false;
						Config::lasttime = 0;
						if (Config::reloading) {
							Config::lasttime = 0;
							Config::shooted = false;
						}
						Config::Targetenemyi = -1;
					}
					if (local_entity.HeroID == eHero::HERO_REAPER && Config::reloading) {
						Sleep(300);
						SetKey(0x800);
					}

					if (Config::secondaim) {
						while (GetAsyncKeyState(Config::aim_key2) && !Config::shooted2)
						{
							auto vec = GetVector3aim2(Config::Prediction2 ? true : false);
							if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI)) {
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + offset::view_angle);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								Vector3 Target;
								if (Config::Tracking2)
									Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth2 / 10.f);
								else if (Config::Flick2)
									Target = SmoothAccelerate(local_angle, vec_calc_target, Config::Flick_smooth2 / 10.f, Config::accvalue2);
								if (Config::Rage) Target = vec_calc_target;
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (Config::aiaim) {
									if (Config::Flick2) {
										int x;
										if (rand() % 10 > 5)
											Target.X += (double)(rand()) / RAND_MAX / 300;
										else Target.X -= (double)(rand()) / RAND_MAX / 300;
										if (rand() % 10 > 5)
											Target.Y += (double)(rand()) / RAND_MAX / 300;
										else Target.Y -= (double)(rand()) / RAND_MAX / 300;
										if (rand() % 10 > 5)
											Target.Z += (double)(rand()) / RAND_MAX / 300;
										else Target.Z -= (double)(rand()) / RAND_MAX / 300;
										if (Config::minFov1 > 500)Config::minFov1 = 500;
										if (Config::Fov > 500)Config::Fov = 500;
										if (Config::minFov2 > 500)Config::minFov1 = 500;
										if (Config::Fov2 > 500)Config::Fov = 500;
										if (Config::fov360) Config::fov360 = false;
									}
									else if (Config::Tracking2) {
										int x;
										if (rand() % 10 > 5)
											Target.X += (double)(rand()) / RAND_MAX / 500;
										else Target.X -= (double)(rand()) / RAND_MAX / 500;
										if (rand() % 10 > 5)
											Target.Y += (double)(rand()) / RAND_MAX / 500;
										else Target.Y -= (double)(rand()) / RAND_MAX / 500;
										if (rand() % 10 > 5)
											Target.Z += (double)(rand()) / RAND_MAX / 500;
										else Target.Z -= (double)(rand()) / RAND_MAX / 500;
										if (Config::minFov1 > 500)Config::minFov1 = 500;
										if (Config::Fov > 500)Config::Fov = 500;
										if (Config::minFov2 > 500)Config::minFov1 = 500;
										if (Config::Fov2 > 500)Config::Fov = 500;
										if (Config::fov360) Config::fov360 = false;
									}
								}
								if (Target != Vector3(0, 0, 0)) {
									float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
									if (Config::health <= Config::meleehealth && dist <= Config::meleedistance && Config::AutoMelee) {
										SetKey(0x800);
									}

									SDK->WPM<Vector3>(SDK->g_player_controller + offset::view_angle, Target);
									if (Config::Flick2) {
										if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::hitbox2)) {
											if (Config::togglekey == 0)
												SetKey(0x1);
											else if (Config::togglekey == 1)
												SetKey(0x2);
											else if (Config::togglekey == 2)
												SetKey(0x8);
											else if (Config::togglekey == 3)
												SetKey(0x10);
											else if (Config::togglekey == 4)
												SetKey(0x20);
											Sleep(1);
											Config::shooted2 = true;
										}
									}
								}
								if (local_entity.PlayerHealth < Config::SkillHealth) {
									break;
								}
							}
							Sleep(1);
							if (Config::autoscalefov) {
								auto vec = GetVector3forfov();
								if (vec != Vector3(0, 0, 0)) {
									Vector2 high;
									Vector2 low;
									if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
									{
										Config::Fov = -(high.Y - low.Y) * 4;
										if (Config::Fov > 500) Config::Fov = 500;
										else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
										Config::Fov2 = -(high.Y - low.Y) * 4;
										if (Config::Fov2 > 500) Config::Fov2 = 500;
										else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
									}
									else
									{
										Config::Fov = Config::minFov1;
										Config::Fov2 = Config::minFov2;
									}

								}
								else
								{
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
							}
						}
						if (Config::shooted2 && !GetAsyncKeyState(Config::aim_key2))
							Config::shooted2 = false;
					}
				}
				Sleep(2);
			}
		}
		__except (1) {

		}
	}
	inline void configsavenloadthread() {
		TCHAR bufsave[100];
		if (Config::lastheroid == -2) {
			ImGui::InsertNotification({ ImGuiToastType_Success, 8000, (u8"test Internal Loaded\nWelcome！"), "" });
			Config::lastheroid = 0;
		}
		while (1) {
			if (!Config::Menu) {
				if (Config::lastheroid != local_entity.HeroID) {
					if (Config::lastheroid != 0) {
						_stprintf(bufsave, _T("%d"), Config::highPriority);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("highPriority"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::aiaim);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aiaim"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::hanzoautospeed);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hanzoautospeed"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_hp_pack);
						WritePrivateProfileString(_T("Global"), _T("draw_hp_pack"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::crosscircle);
						WritePrivateProfileString(_T("Global"), _T("crosscircle"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::eyeray);
						WritePrivateProfileString(_T("Global"), _T("eyeray"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::autoscalefov);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autoscalefov"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::lockontarget);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("lockontarget"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::trackcompensate);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("trackc"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::comarea * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("comarea"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::comspeed * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("comspeed"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), (int)Config::Fov);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("FOV"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::hitbox * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitbox"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::missbox * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("missbox"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Tracking_smooth * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking_smooth"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Flick_smooth * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick_smooth"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::Shoottime);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoShootTime"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::predit_level);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predit_level"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::aim_key);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aim_key"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::Gravitypredit);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit"), bufsave, _T(".\\config.ini"));
						int dec = 0;
						if (Config::Tracking) dec = 0;
						else if (Config::Flick) dec = 1;
						else if (Config::hanzo_flick) dec = 2;
						else if (Config::silent) dec = 3;
						else if (Config::triggerbot) dec = 4;
						int autoshoot = 0;
						if (Config::AutoShoot) autoshoot = 1;
						else autoshoot = 0;
						int predictdec = 0;
						if (Config::Prediction) predictdec = 1;
						else predictdec = 0;
						_stprintf(bufsave, _T("%d"), dec);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Aim Mode"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), autoshoot);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autoshootonoff"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), predictdec);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predictdec"), bufsave, _T(".\\config.ini"));
						int dontshot = 0;
						if (Config::dontshot) dontshot = 1;
						else dontshot = 0;
						_stprintf(bufsave, _T("%d"), dontshot);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("dontshot"), bufsave, _T(".\\config.ini"));
						int targetdelay = 0;
						if (Config::targetdelay) targetdelay = 1;
						else targetdelay = 0;
						_stprintf(bufsave, _T("%d"), targetdelay);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("targetdelay"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::targetdelaytime);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("targetdelaytime"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::shotmanydont);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("dontmanyshot"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::hitboxdelayshoot);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitboxdelayshoot"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::hiboxdelaytime);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitboxdelaytime"), bufsave, _T(".\\config.ini"));

						if (Config::lastheroid == eHero::HERO_GENJI) {
							_stprintf(bufsave, _T("%d"), Config::GenjiBlade);
							WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("GenjiBlade"), bufsave, _T(".\\config.ini"));
							_stprintf(bufsave, _T("%d"), Config::AutoShiftGenji);
							WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoShiftGenji"), bufsave, _T(".\\config.ini"));
							_stprintf(bufsave, _T("%d"), (int)(Config::bladespeed * 10000));
							WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("bladespeed"), bufsave, _T(".\\config.ini"));
						}

						if (Config::lastheroid == eHero::HERO_WIDOWMAKER) {
							_stprintf(bufsave, _T("%d"), Config::widowautounscope);
							WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("widowautounscope"), bufsave, _T(".\\config.ini"));
						}

						_stprintf(bufsave, _T("%d"), Config::Bone);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Bone"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::autobone);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autobone"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::Bone2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Bone2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::autobone2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autobone2"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::AutoMelee);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoMelee"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::norecoil);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("norecoil"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::recoilnum * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("recoilnum"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::accvalue * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("accvalue"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::horizonreco);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("horizonreco"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Gravitypredit);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::switch_team);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("switch_team"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::switch_team2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("switch_team2"), bufsave, _T(".\\config.ini"));


						_stprintf(bufsave, _T("%d"), Config::secondaim);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("secondaim"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Tracking2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Flick2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Prediction2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Prediction2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Gravitypredit2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::aim_key2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aim_key2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::togglekey);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("togglekey"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::predit_level2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predit_level2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Tracking_smooth2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking_smooth2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Flick_smooth2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick_smooth2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::accvalue2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("accvalue2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::hitbox2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitbox2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Fov2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Fov2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::enablechangefov);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("enablechangefov"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::CHANGEFOV * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("CHANGEFOV"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::trackback);
						WritePrivateProfileString(_T("Global"), _T("trackback"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_info);
						WritePrivateProfileString(_T("Global"), _T("draw_info"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::drawbattletag);
						WritePrivateProfileString(_T("Global"), _T("drawbattletag"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::drawhealth);
						WritePrivateProfileString(_T("Global"), _T("drawhealth"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::healthbar);
						WritePrivateProfileString(_T("Global"), _T("healthbar"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::dist);
						WritePrivateProfileString(_T("Global"), _T("dist"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::name);
						WritePrivateProfileString(_T("Global"), _T("name"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::ult);
						WritePrivateProfileString(_T("Global"), _T("ult"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_skel);
						WritePrivateProfileString(_T("Global"), _T("draw_skel"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::skillinfo);
						WritePrivateProfileString(_T("Global"), _T("skillinfo"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::outline);
						WritePrivateProfileString(_T("Global"), _T("outline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::externaloutline);
						WritePrivateProfileString(_T("Global"), _T("externaloutline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::teamoutline);
						WritePrivateProfileString(_T("Global"), _T("teamoutline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::healthoutline);
						WritePrivateProfileString(_T("Global"), _T("healthoutline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::rainbowoutline);
						WritePrivateProfileString(_T("Global"), _T("rainbowoutline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_edge);
						WritePrivateProfileString(_T("Global"), _T("draw_edge"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::drawbox3d);
						WritePrivateProfileString(_T("Global"), _T("drawbox3d"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::radar);
						WritePrivateProfileString(_T("Global"), _T("radar"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::radarline);
						WritePrivateProfileString(_T("Global"), _T("radarline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::drawline);
						WritePrivateProfileString(_T("Global"), _T("drawline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_fov);
						WritePrivateProfileString(_T("Global"), _T("draw_fov"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("EnemyColx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("EnemyColy"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("EnemyColz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("EnemyColw"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcolx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcoly"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcolz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcolw"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcol2x"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcol2y"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcol2z"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcol2w"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("invisenargbx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("invisenargby"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("invisenargbz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("invisenargbw"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::enargb.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("enargbx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::enargb.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("enargby"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::enargb.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("enargbz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::enargb.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("enargbw"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargbx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargby"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargbz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargbw"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargb2x"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargb2y"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargb2z"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargb2w"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("allyargbx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("allyargby"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("allyargbz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("allyargbw"), bufsave, _T(".\\config.ini"));


						std::string saveheroname = GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str();
						saveheroname = (u8"Saved:") + saveheroname;
						ImGui::InsertNotification({ ImGuiToastType_Success, 5000,saveheroname.data() , "" });
					}
					Config::Fov = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("FOV"), 200, _T(".\\config.ini"));
					Config::minFov1 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("FOV"), 200, _T(".\\config.ini"));

					Config::comarea = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("comarea"), 100, _T(".\\config.ini"))) / 10000.f;
					Config::comspeed = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("comspeed"), 5000, _T(".\\config.ini"))) / 10000.f;

					Config::hitbox = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hitbox"), 1300, _T(".\\config.ini"))) / 10000.f;
					Config::missbox = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("missbox"), 6000, _T(".\\config.ini"))) / 10000.f;
					Config::Tracking_smooth = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Tracking_smooth"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::Flick_smooth = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Flick_smooth"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::Shoottime = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("AutoShootTime"), 500, _T(".\\config.ini"));
					Config::predit_level = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("predit_level"), 110, _T(".\\config.ini"));
					Config::aim_key = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("aim_key"), 6, _T(".\\config.ini"));
					Config::dontshot = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("dontshot"), 0, _T(".\\config.ini"));
					Config::targetdelay = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("targetdelay"), 0, _T(".\\config.ini"));
					Config::targetdelaytime = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("targetdelaytime"), 0, _T(".\\config.ini"));
					Config::shotmanydont = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("dontmanyshot"), 0, _T(".\\config.ini"));
					Config::hitboxdelayshoot = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hitboxdelayshoot"), 0, _T(".\\config.ini"));
					Config::hiboxdelaytime = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hitboxdelaytime"), 200, _T(".\\config.ini"));
					Config::Bone = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Bone"), 1, _T(".\\config.ini"));
					Config::autobone = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("autobone"), 0, _T(".\\config.ini"));

					Config::Bone2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Bone2"), 1, _T(".\\config.ini"));
					Config::autobone2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("autobone2"), 0, _T(".\\config.ini"));

					Config::AutoMelee = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("AutoMelee"), 0, _T(".\\config.ini"));
					Config::norecoil = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("norecoil"), 0, _T(".\\config.ini"));
					Config::recoilnum = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("recoilnum"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::accvalue = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("accvalue"), 7500, _T(".\\config.ini"))) / 10000.f;
					Config::horizonreco = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("horizonreco"), 0, _T(".\\config.ini"));
					Config::Gravitypredit = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Gravitypredit"), 0, _T(".\\config.ini"));
					Config::switch_team = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("switch_team"), 0, _T(".\\config.ini"));
					Config::draw_hp_pack = GetPrivateProfileInt(_T("Global"), _T("draw_hp_pack"), 0, _T(".\\config.ini"));

					Config::switch_team2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("switch_team2"), 0, _T(".\\config.ini"));
					Config::secondaim = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("secondaim"), 0, _T(".\\config.ini"));
					Config::Tracking2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Tracking2"), 0, _T(".\\config.ini"));
					Config::Flick2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Flick2"), 0, _T(".\\config.ini"));
					Config::Prediction2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Prediction2"), 0, _T(".\\config.ini"));
					Config::Gravitypredit2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Gravitypredit2"), 0, _T(".\\config.ini"));
					Config::predit_level2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("predit_level2"), 1100000, _T(".\\config.ini"))) / 10000.f;
					
					Config::Fov2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Fov2"), 2000000, _T(".\\config.ini"))) / 10000.f;
					Config::minFov2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Fov2"), 2000000, _T(".\\config.ini"))) / 10000.f;
					
					Config::hitbox2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hitbox2"), 1300, _T(".\\config.ini"))) / 10000.f;
					Config::Tracking_smooth2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Tracking_smooth2"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::Flick_smooth2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Flick_smooth2"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::accvalue2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("accvalue2"), 7500, _T(".\\config.ini"))) / 10000.f;

					Config::aim_key2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("aim_key2"), 5, _T(".\\config.ini"));
					Config::togglekey = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("togglekey"), 0, _T(".\\config.ini"));
					Config::Gravitypredit = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Gravitypredit"), 0, _T(".\\config.ini"));

					Config::enablechangefov = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("enablechangefov"), 0, _T(".\\config.ini"));
					Config::CHANGEFOV = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("CHANGEFOV"), 1030000, _T(".\\config.ini"))) / 10000.f;

					Config::trackback = GetPrivateProfileInt(_T("Global"), _T("trackback"), 0, _T(".\\config.ini"));
					Config::draw_info = GetPrivateProfileInt(_T("Global"), _T("draw_info"), 0, _T(".\\config.ini"));
					Config::drawbattletag = GetPrivateProfileInt(_T("Global"), _T("drawbattletag"), 0, _T(".\\config.ini"));
					Config::drawhealth = GetPrivateProfileInt(_T("Global"), _T("drawhealth"), 0, _T(".\\config.ini"));
					Config::healthbar = GetPrivateProfileInt(_T("Global"), _T("healthbar"), 0, _T(".\\config.ini"));
					Config::dist = GetPrivateProfileInt(_T("Global"), _T("dist"), 0, _T(".\\config.ini"));
					Config::name = GetPrivateProfileInt(_T("Global"), _T("name"), 0, _T(".\\config.ini"));
					Config::ult = GetPrivateProfileInt(_T("Global"), _T("ult"), 0, _T(".\\config.ini"));
					Config::draw_skel = GetPrivateProfileInt(_T("Global"), _T("draw_skel"), 0, _T(".\\config.ini"));
					Config::skillinfo = GetPrivateProfileInt(_T("Global"), _T("skillinfo"), 0, _T(".\\config.ini"));
					Config::outline = GetPrivateProfileInt(_T("Global"), _T("outline"), 0, _T(".\\config.ini"));
					Config::radar = GetPrivateProfileInt(_T("Global"), _T("radar"), 0, _T(".\\config.ini"));
					Config::radarline = GetPrivateProfileInt(_T("Global"), _T("radarline"), 0, _T(".\\config.ini"));
					Config::externaloutline = GetPrivateProfileInt(_T("Global"), _T("externaloutline"), 0, _T(".\\config.ini"));
					Config::teamoutline = GetPrivateProfileInt(_T("Global"), _T("teamoutline"), 0, _T(".\\config.ini"));
					Config::healthoutline = GetPrivateProfileInt(_T("Global"), _T("healthoutline"), 0, _T(".\\config.ini"));
					Config::rainbowoutline = GetPrivateProfileInt(_T("Global"), _T("rainbowoutline"), 0, _T(".\\config.ini"));
					Config::draw_edge = GetPrivateProfileInt(_T("Global"), _T("draw_edge"), 0, _T(".\\config.ini"));
					Config::drawbox3d = GetPrivateProfileInt(_T("Global"), _T("drawbox3d"), 0, _T(".\\config.ini"));
					Config::drawline = GetPrivateProfileInt(_T("Global"), _T("drawline"), 0, _T(".\\config.ini"));
					Config::draw_fov = GetPrivateProfileInt(_T("Global"), _T("draw_fov"), 0, _T(".\\config.ini"));

					Config::EnemyCol.x = float(GetPrivateProfileInt(_T("Global"), _T("EnemyColx"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::EnemyCol.y = float(GetPrivateProfileInt(_T("Global"), _T("EnemyColy"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::EnemyCol.z = float(GetPrivateProfileInt(_T("Global"), _T("EnemyColz"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::EnemyCol.w = float(GetPrivateProfileInt(_T("Global"), _T("EnemyColw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::fovcol.x = float(GetPrivateProfileInt(_T("Global"), _T("fovcolx"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol.y = float(GetPrivateProfileInt(_T("Global"), _T("fovcoly"), 9000, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol.z = float(GetPrivateProfileInt(_T("Global"), _T("fovcolz"), 0, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol.w = float(GetPrivateProfileInt(_T("Global"), _T("fovcolw"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol2.x = float(GetPrivateProfileInt(_T("Global"), _T("fovcol2x"), 8550, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol2.y = float(GetPrivateProfileInt(_T("Global"), _T("fovcol2y"), 4390, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol2.z = float(GetPrivateProfileInt(_T("Global"), _T("fovcol2z"), 8390, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol2.w = float(GetPrivateProfileInt(_T("Global"), _T("fovcol2w"), 5000, _T(".\\config.ini"))) / 10000.f;

					Config::invisenargb.x = float(GetPrivateProfileInt(_T("Global"), _T("invisenargbx"), 4000, _T(".\\config.ini"))) / 10000.f;
					Config::invisenargb.y = float(GetPrivateProfileInt(_T("Global"), _T("invisenargby"), 3700, _T(".\\config.ini"))) / 10000.f;
					Config::invisenargb.z = float(GetPrivateProfileInt(_T("Global"), _T("invisenargbz"), 9100, _T(".\\config.ini"))) / 10000.f;
					Config::invisenargb.w = float(GetPrivateProfileInt(_T("Global"), _T("invisenargbw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::enargb.x = float(GetPrivateProfileInt(_T("Global"), _T("enargbx"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::enargb.y = float(GetPrivateProfileInt(_T("Global"), _T("enargby"), 3000, _T(".\\config.ini"))) / 10000.f;
					Config::enargb.z = float(GetPrivateProfileInt(_T("Global"), _T("enargbz"), 0, _T(".\\config.ini"))) / 10000.f;
					Config::enargb.w = float(GetPrivateProfileInt(_T("Global"), _T("enargbw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::targetargb.x = float(GetPrivateProfileInt(_T("Global"), _T("targetargbx"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb.y = float(GetPrivateProfileInt(_T("Global"), _T("targetargby"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb.z = float(GetPrivateProfileInt(_T("Global"), _T("targetargbz"), 0, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb.w = float(GetPrivateProfileInt(_T("Global"), _T("targetargbw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::targetargb2.x = float(GetPrivateProfileInt(_T("Global"), _T("targetargb2x"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb2.y = float(GetPrivateProfileInt(_T("Global"), _T("targetargb2y"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb2.z = float(GetPrivateProfileInt(_T("Global"), _T("targetargb2z"), 4000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb2.w = float(GetPrivateProfileInt(_T("Global"), _T("targetargb2w"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::allyargb.x = float(GetPrivateProfileInt(_T("Global"), _T("allyargbx"), 4000, _T(".\\config.ini"))) / 10000.f;
					Config::allyargb.y = float(GetPrivateProfileInt(_T("Global"), _T("allyargby"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::allyargb.z = float(GetPrivateProfileInt(_T("Global"), _T("allyargbz"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::allyargb.w = float(GetPrivateProfileInt(_T("Global"), _T("allyargbw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::lockontarget = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("lockontarget"), 1, _T(".\\config.ini"));
					Config::trackcompensate = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("trackc"), 0, _T(".\\config.ini"));
					Config::autoscalefov = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("autoscalefov"), 0, _T(".\\config.ini"));
					Config::eyeray = GetPrivateProfileInt(_T("Global"), _T("eyeray"), 0, _T(".\\config.ini"));
					Config::crosscircle = GetPrivateProfileInt(_T("Global"), _T("crosscircle"), 0, _T(".\\config.ini"));
					Config::aiaim = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("aiaim"), 0, _T(".\\config.ini"));
					Config::hanzoautospeed = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hanzoautospeed"), 0, _T(".\\config.ini"));
					Config::highPriority = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("highPriority"), 0, _T(".\\config.ini"));

					switch (Config::aim_key)
					{
					case VK_LBUTTON:
						keys = key_type[0];
						break;
					case VK_RBUTTON:
						keys = key_type[1];
						break;
					case VK_MBUTTON:
						keys = key_type[2];
						break;
					case VK_XBUTTON1:
						keys = key_type[3];
						break;
					case VK_XBUTTON2:
						keys = key_type[4];
						break;
					}

					switch (Config::aim_key2)
					{
					case VK_LBUTTON:
						keys2 = key_type2[0];
						break;
					case VK_RBUTTON:
						keys2 = key_type2[1];
						break;
					case VK_MBUTTON:
						keys2 = key_type2[2];
						break;
					case VK_XBUTTON1:
						keys2 = key_type2[3];
						break;
					case VK_XBUTTON2:
						keys2 = key_type2[4];
						break;
					}

					switch (Config::togglekey)
					{
					case 0:
						keys3 = key_type3[0];
						break;
					case 1:
						keys3 = key_type3[1];
						break;
					case 2:
						keys3 = key_type3[2];
						break;
					case 3:
						keys3 = key_type3[3];
						break;
					case 4:
						keys3 = key_type3[4];
						break;
					}

					int dec1 = 0;
					dec1 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Aim Mode"), 0, _T(".\\config.ini"));
					if (dec1 == 0) {
						Config::Tracking = true;
						Config::Flick = false;
						Config::silent = false;
						Config::triggerbot = false;
						Config::hanzo_flick = false;
					}
					else if (dec1 == 1) {
						Config::Tracking = false;
						Config::Flick = true;
						Config::silent = false;
						Config::triggerbot = false;
						Config::hanzo_flick = false;
					}
					else if (dec1 == 2) {
						Config::Tracking = false;
						Config::Flick = false;
						Config::silent = false;
						Config::triggerbot = false;
						Config::hanzo_flick = true;
					}
					else if (dec1 == 3) {
						Config::Tracking = false;
						Config::Flick = false;
						Config::silent = true;
						Config::triggerbot = false;
						Config::hanzo_flick = false;
					}
					else if (dec1 == 4) {
						Config::Tracking = false;
						Config::Flick = false;
						Config::silent = false;
						Config::triggerbot = true;
						Config::hanzo_flick = false;
					}
					int dec2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("autoshootonoff"), 0, _T(".\\config.ini"));
					if (dec2 == 0) Config::AutoShoot = false;
					else Config::AutoShoot = true;
					int dec3 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("predictdec"), 0, _T(".\\config.ini"));
					if (dec3 == 0) Config::Prediction = false;
					else Config::Prediction = true;

					if (local_entity.HeroID != eHero::HERO_GENJI && (Config::GenjiBlade == true || Config::AutoShiftGenji == true)) {
						Config::GenjiBlade = false;
						Config::AutoShiftGenji = false;
					}
					if (local_entity.HeroID == eHero::HERO_GENJI) {
						Config::GenjiBlade = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("GenjiBlade"), 0, _T(".\\config.ini"));
						Config::AutoShiftGenji = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("AutoShiftGenji"), 0, _T(".\\config.ini"));
						Config::bladespeed = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("bladespeed"), 5000, _T(".\\config.ini"))) / 10000.f;
					}
					if (local_entity.HeroID == eHero::HERO_WIDOWMAKER) {
						Config::widowautounscope = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("widowautounscope"), 0, _T(".\\config.ini"));
					}
					if (local_entity.HeroID != eHero::HERO_WIDOWMAKER && Config::widowautounscope) {
						Config::widowautounscope = false;
					}
					Config::lastheroid = local_entity.HeroID;
					Sleep(2);
					std::string saveheroname = GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str();
					Config::nowhero = (u8"Now using:") + saveheroname;
					saveheroname = (u8"Loaded:") + saveheroname;
					ImGui::InsertNotification({ ImGuiToastType_Success, 5000,saveheroname.data() , "" });
				}
			}
			else if (Config::manualsave == true && Config::lastheroid != 0) {
				_stprintf(bufsave, _T("%d"), Config::highPriority);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("highPriority"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::hanzoautospeed);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hanzoautospeed"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_hp_pack);
				WritePrivateProfileString(_T("Global"), _T("draw_hp_pack"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::aiaim);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aiaim"), bufsave, _T(".\\config.ini"));


				_stprintf(bufsave, _T("%d"), Config::crosscircle);
				WritePrivateProfileString(_T("Global"), _T("crosscircle"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::eyeray);
				WritePrivateProfileString(_T("Global"), _T("eyeray"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::autoscalefov);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autoscalefov"), bufsave, _T(".\\config.ini"));


				_stprintf(bufsave, _T("%d"), Config::lockontarget);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("lockontarget"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::trackcompensate);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("trackc"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::comarea * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("comarea"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::comspeed * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("comspeed"), bufsave, _T(".\\config.ini"));


				_stprintf(bufsave, _T("%d"), (int)Config::Fov);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("FOV"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::hitbox * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitbox"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::missbox * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("missbox"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Tracking_smooth * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking_smooth"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Flick_smooth * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick_smooth"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::Shoottime);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoShootTime"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::predit_level);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predit_level"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::aim_key);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aim_key"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::recoilnum * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("recoilnum"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::Gravitypredit);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit"), bufsave, _T(".\\config.ini"));
				int dec = 0;
				if (Config::Tracking) dec = 0;
				else if (Config::Flick) dec = 1;
				else if (Config::hanzo_flick) dec = 2;
				else if (Config::silent) dec = 3;
				else if (Config::triggerbot) dec = 4;
				int autoshoot = 0;
				if (Config::AutoShoot) autoshoot = 1;
				else autoshoot = 0;
				int predictdec = 0;
				if (Config::Prediction) predictdec = 1;
				else predictdec = 0;
				_stprintf(bufsave, _T("%d"), dec);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Aim Mode"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), autoshoot);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autoshootonoff"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), predictdec);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predictdec"), bufsave, _T(".\\config.ini"));
				int dontshot = 0;
				if (Config::dontshot) dontshot = 1;
				else dontshot = 0;
				_stprintf(bufsave, _T("%d"), dontshot);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("dontshot"), bufsave, _T(".\\config.ini"));
				int targetdelay = 0;
				if (Config::targetdelay) targetdelay = 1;
				else targetdelay = 0;
				_stprintf(bufsave, _T("%d"), targetdelay);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("targetdelay"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::targetdelaytime);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("targetdelaytime"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::shotmanydont);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("dontmanyshot"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::hitboxdelayshoot);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitboxdelayshoot"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::hiboxdelaytime);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitboxdelaytime"), bufsave, _T(".\\config.ini"));
				Config::manualsave = false;
				if (Config::lastheroid == eHero::HERO_GENJI) {
					_stprintf(bufsave, _T("%d"), Config::GenjiBlade);
					WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("GenjiBlade"), bufsave, _T(".\\config.ini"));
					_stprintf(bufsave, _T("%d"), Config::AutoShiftGenji);
					WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoShiftGenji"), bufsave, _T(".\\config.ini"));
					_stprintf(bufsave, _T("%d"), (int)(Config::bladespeed * 10000));
					WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("bladespeed"), bufsave, _T(".\\config.ini"));
				}

				if (Config::lastheroid == eHero::HERO_WIDOWMAKER) {
					_stprintf(bufsave, _T("%d"), Config::widowautounscope);
					WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("widowautounscope"), bufsave, _T(".\\config.ini"));
				}

				_stprintf(bufsave, _T("%d"), Config::Bone);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Bone"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::autobone);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autobone"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::Bone2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Bone2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::autobone2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autobone2"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::AutoMelee);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoMelee"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::norecoil);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("norecoil"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::recoilnum * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("recoilnum"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::accvalue * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("accvalue"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::horizonreco);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("horizonreco"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Gravitypredit);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::switch_team);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("switch_team"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::switch_team2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("switch_team2"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::secondaim);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("secondaim"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Tracking2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Flick2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Prediction2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Prediction2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Gravitypredit2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::aim_key2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aim_key2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::togglekey);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("togglekey"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::predit_level2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predit_level2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Tracking_smooth2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking_smooth2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Flick_smooth2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick_smooth2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::accvalue2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("accvalue2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::hitbox2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitbox2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Fov2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Fov2"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::enablechangefov);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("enablechangefov"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::CHANGEFOV * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("CHANGEFOV"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::trackback);
				WritePrivateProfileString(_T("Global"), _T("trackback"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_info);
				WritePrivateProfileString(_T("Global"), _T("draw_info"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::drawbattletag);
				WritePrivateProfileString(_T("Global"), _T("drawbattletag"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::drawhealth);
				WritePrivateProfileString(_T("Global"), _T("drawhealth"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::healthbar);
				WritePrivateProfileString(_T("Global"), _T("healthbar"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::dist);
				WritePrivateProfileString(_T("Global"), _T("dist"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::name);
				WritePrivateProfileString(_T("Global"), _T("name"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::ult);
				WritePrivateProfileString(_T("Global"), _T("ult"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_skel);
				WritePrivateProfileString(_T("Global"), _T("draw_skel"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::skillinfo);
				WritePrivateProfileString(_T("Global"), _T("skillinfo"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::outline);
				WritePrivateProfileString(_T("Global"), _T("outline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::externaloutline);
				WritePrivateProfileString(_T("Global"), _T("externaloutline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::teamoutline);
				WritePrivateProfileString(_T("Global"), _T("teamoutline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::healthoutline);
				WritePrivateProfileString(_T("Global"), _T("healthoutline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::rainbowoutline);
				WritePrivateProfileString(_T("Global"), _T("rainbowoutline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_edge);
				WritePrivateProfileString(_T("Global"), _T("draw_edge"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::drawbox3d);
				WritePrivateProfileString(_T("Global"), _T("drawbox3d"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::radar);
				WritePrivateProfileString(_T("Global"), _T("radar"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::radarline);
				WritePrivateProfileString(_T("Global"), _T("radarline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::drawline);
				WritePrivateProfileString(_T("Global"), _T("drawline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_fov);
				WritePrivateProfileString(_T("Global"), _T("draw_fov"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("EnemyColx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("EnemyColy"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("EnemyColz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("EnemyColw"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcolx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcoly"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcolz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcolw"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcol2x"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcol2y"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcol2z"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcol2w"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("invisenargbx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("invisenargby"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("invisenargbz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("invisenargbw"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::enargb.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("enargbx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::enargb.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("enargby"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::enargb.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("enargbz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::enargb.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("enargbw"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargbx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargby"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargbz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargbw"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargb2x"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargb2y"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargb2z"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargb2w"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("allyargbx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("allyargby"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("allyargbz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("allyargbw"), bufsave, _T(".\\config.ini"));
				std::string saveheroname = GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str();
				saveheroname = (u8"Saved:") + saveheroname;
				ImGui::InsertNotification({ ImGuiToastType_Success, 5000,saveheroname.data() , "" });
			}
			Sleep(2);
		}
	}

	inline void looprpmthread() {
		while (1) {
			if (entities.size() > 0) {
				if (local_entity.AngleBase && (GetAsyncKeyState(Config::aim_key) || GetAsyncKeyState(Config::aim_key2) || GetAsyncKeyState(0x01) || GetAsyncKeyState(0x02))) {
					if (Config::horizonreco) {
						SDK->WPM<float>(local_entity.AngleBase + 0x1608, 0);
					}
					if (Config::norecoil) {
						SDK->WPM<float>(local_entity.AngleBase + 0x160C, Config::recoilnum);
					}

				}
				if (Config::enablechangefov) {
					SDK->WPM<float>(SDK->dwGameBase + offset::changefov, Config::CHANGEFOV);
				}
			}
			Sleep(10);
		}
	}}