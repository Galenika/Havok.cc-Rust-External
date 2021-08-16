#pragma once





#pragma region Includes
#include "Imports.hpp"
#include "Driver.hpp"
#include "Utils.hpp"
#include "HeldItem.hpp"
#pragma endregion

#pragma region BPFlags

enum class BCameraMode {
	FirstPerson = 0,
	ThirdPerson = 1,
	Eyes = 2,
	FirstPersonWithArms = 3,
	Last = 3
};

enum class BPlayerFlags {
	Unused1 = 1,
	Unused2 = 2,
	IsAdmin = 4,
	ReceivingSnapshot = 8,
	Sleeping = 16,
	Spectating = 32,
	Wounded = 64,
	IsDeveloper = 128,
	Connected = 256,
	ThirdPersonViewmode = 1024,
	EyesViewmode = 2048,
	ChatMute = 4096,
	NoSprint = 8192,
	Aiming = 16384,
	DisplaySash = 32768,
	Relaxed = 65536,
	SafeZone = 131072,
	ServerFall = 262144,
	Workbench1 = 1048576,
	Workbench2 = 2097152,
	Workbench3 = 4194304,
	DebugCamera = 260
};


enum class BMapNoteType {
	Death = 0,
	PointOfInterest = 1
};

enum class BTimeCategory {
	Wilderness = 1,
	Monument = 2,
	Base = 4,
	Flying = 8,
	Boating = 16,
	Swimming = 32,
	Driving = 64
};

#pragma endregion

#pragma region OffsetStuff

#define ConVar_Graphics_c 0x32732A0

#pragma endregion

#pragma region BList

enum BonesList : int
{
	r_foot = 3,
	l_knee = 2,
	l_hip = 1,
	pelvis = 6,
	l_foot = 15,
	r_knee = 14,
	r_hip = 13,
	spine1 = 18,
	neck = 46,
	head = 47,
	l_upperarm = 24,
	l_forearm = 25,
	l_hand = 26,
	r_upperarm = 55,
	r_forearm = 56,
	r_hand = 57
};

#pragma endregion

#pragma region MSFlags

enum class MStateFlags {
	Ducked = 1,
	Jumped = 2,
	OnGround = 4,
	Sleeping = 8,
	Sprinting = 16,
	OnLadder = 32,
	Flying = 64,
	Aiming = 128,
	Prone = 256,
	Mounted = 512,
	Relaxed = 1024,
};

enum class BaseEntityFlag
{
	stash = 2048,
};

#pragma endregion

#pragma region BPClass

class Utility
{
public:
	std::string wstring_to_string(const std::wstring& wstring);

}; Utility* utils;



std::string Utility::wstring_to_string(const std::wstring& wstring)
{
	int slength = (int)wstring.length() + 1;
	int len = WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), slength, &r[0], len, 0, 0);
	return r;
}
class BaseEntity {
public:

	BaseEntity() {}

	BaseEntity(uintptr_t _ent, uintptr_t _trans, uintptr_t _obj) {

		this->player = Read<uintptr_t>(_ent + 0x28); //Entity
		this->visualState = Read<uintptr_t>(_trans + 0x38);

		this->playerFlags = Read<int32_t>(_ent + 0x658); //public BasePlayer.PlayerFlags playerFlags;
		this->name = ReadNative(_obj + 0x60);
		this->entityFlags = Read<int32_t>(_ent + 0x130);

		this->playerModel = Read<uintptr_t>(this->player + 0x4C0); //BasePlayer -> public PlayerModel playerModel;
		this->modelState = Read<uintptr_t>(this->player + 0x5E8); //0x588 // public ModelState modelState;

		this->position = Read<Vector3>(this->visualState + 0x90); //internal Vector3 position;
		this->health = Read<float>(this->player + 0x224);//private float _health;
	}

public:


	void setViewAngles(Vector3 angles) { // vector 3
		Write<Vector3>(Read<uint64_t>(this->player + 0x4E0) + 0x3C, angles); //public PlayerInput input; | private Vector3 bodyAngles;
	}

	void setViewAngles(Vector2 angles) { // vector 2
		Write<Vector2>(Read<uint64_t>(this->player + 0x4E0) + 0x3C, angles); //public PlayerInput input; | private Vector3 bodyAngles;
	}

	void set_aim_angles(Vector3 aim_angle) {
		auto current = Read<uintptr_t>((uintptr_t)(this + 0x10));
		Read<Vector3>(current + 0x18) = aim_angle;
	}

	void setPlayerFlag(BPlayerFlags flag) {
		Write(this->player + 0x658, flag); //0x5F8 //public BasePlayer.PlayerFlags playerFlags;
	}

	void remove_flag(MStateFlags flag)
	{
		int flags = *reinterpret_cast<int*>((uintptr_t)this + 0x24);
		flags &= ~(int)flags;

		*reinterpret_cast<int*>((uintptr_t)this + 0x24) = flags;
	}

	void setModelFlag(MStateFlags flag) {
		Write(this->modelState + 0x24, flag);
	}

	void setBaseFlag(BaseEntityFlag flag) {
		Write(this->entityFlags + 0x130, flag);
	}

	void speedHack(int speed) {
		Write<float>(this->player + 0x714, speed);
	}

public:

	bool IsInView() {
		Vector2 screen_pos;
		if (!Utils::WorldToScreen(this->getPosition(), screen_pos))
			return false;

		return true;
	}


	bool isLocalPlayer() {
		return Read<bool>(this->playerModel + 0x279); //internal bool isLocalPlayer;
	}
	
	bool isVisible()
	{
		return Read<bool>(this->playerModel + 0x268);
	}

	bool iSMenu()
	{
		if (!this) return true;
		DWORD64 Input = Read<DWORD64>(this->player + 0x4E0);
		return !(Read<bool>(Input + 0x94));
	}



	bool isSameTeam(std::unique_ptr<BaseEntity>& localPlayer) {
		auto localTeam = Read<uint32_t>(localPlayer->player + 0x598);
		auto playerTeam = Read<uint32_t>(this->player + 0x598);

		if (localTeam == 0 || playerTeam == 0)
			return false;

		return (localTeam == playerTeam);
	}

	bool isConnected() {
		return this->getPlayerFlag(BPlayerFlags::Connected);
	}

	bool isSleeping() {
		return this->getPlayerFlag(BPlayerFlags::Sleeping);
	}

	bool isWounded() {
		return this->getPlayerFlag(BPlayerFlags::Wounded);
	}

	bool isAiming() {
		return this->getPlayerFlag(BPlayerFlags::Aiming);
	}

	bool isFrozen() {
		if (GetAsyncKeyState(Settings::flyKey))
		{
			return Read<bool>(this->player + 0x4C8); //public bool Frozen;
		}
	}

	bool mounted() {
		return Read<bool>(this->player + 0x2B0);
	}

	bool isDead() {
		return (this->health <= 0);
	}

public:

	uint64_t getUserID()
	{
		return Read<uint64_t>(this->player + 0x6A0); //public ulong userID;
	}

	int getDistance(std::unique_ptr<BaseEntity>& player) {
		return this->getPosition().Distance(player->position);
	}

	std::string getDistanceStr(std::unique_ptr<BaseEntity>& player) {
		return std::to_string(this->getDistance(player));
	}

	uint64_t getMountedEntity() {
		return Read<uint64_t>(this->player + 0x5E8); //0x590
	}

	bool getModelFlag(MStateFlags flag) {
		return (this->playerModel & static_cast<int32_t>(flag)) == static_cast<int32_t>(flag);
	}

	bool getPlayerFlag(BPlayerFlags flag) {
		return (this->playerFlags & static_cast<int32_t>(flag)) == static_cast<int32_t>(flag);
	}

	std::string getASCIIName(std::wstring name) {
		return std::string(name.begin(), name.end());
	}

	Vector3 getNewVelocity() {
		return Read<Vector3>(this->playerModel + 0x21C); //private Vector3 newVelocity;
	}

	Vector3 getPosition() {
		return Read<Vector3>(this->visualState + 0x90);//0x90
	}

	Vector3 getRecoilAngles() {
		return ReadChain<Vector3>(this->player, { (uint64_t)0x4E0, (uint64_t)0x64 }); //public Vector3 recoilAngles
	}

	Vector3 getViewAngles() {
		return ReadChain<Vector3>(this->player, { (uint64_t)0x4E0, (uint64_t)0x3C });// public PlayerInput input;
	}

	std::string getName() {
		return this->getASCIIName(this->getPlayerName());
	}


	void AntiHeavy()
	{
		float Reduction = Read<float>(this->player + 0x724);
		if (Reduction == 0.f) { Write<float>(this->player, -0.1f); }
		else if ((Reduction > 0.15f) && (Reduction < 0.35f))
		{
			Write<float>(this->player + 0x724, 0.15f);
		}
		else if (Reduction > 0.39f)
		{
			Write<float>(this->player + 0x724, 0.15f);
		}
	}

	void AutoShit()
	{
		auto mountable = Read<uint64_t>(this->player + 0x5E8);
		if (mountable)
			Write<bool>(mountable + 0x2B0, true);
	}



	void SetWater()
	{
		Write<float>(this->modelState + 0x14, 0.65f);
	}


#pragma region SkyClass+World
	
	void TODCycle()
	{
		DWORD64 ObjManager = Read<DWORD64>(uBase + 0x17C1F18); if (!ObjManager) return;
		DWORD64 Obj = Read<DWORD64>(ObjManager + 0x8); (Obj && (Obj) != Read<DWORD64>(ObjManager)); Obj = Read<DWORD64>(Obj + 0x8);
		DWORD64 GameObject = Read<DWORD64>(Obj + 0x10); //tagged object
		DWORD64 ObjClass = Read<DWORD64>(GameObject + 0x30);
		DWORD64 Entity1 = Read<DWORD64>(ObjClass + 0x18);
		DWORD64 Dome = Read<DWORD64>(Entity1 + 0x28);
		DWORD64 TodCycle = Read<DWORD64>(Dome + 0x38);


		uint64_t test1 = Read<uint64_t>(GameObject + 0x30);//night
		uint64_t test2 = Read<uint64_t>(test1 + 0x18); //world
		uint64_t test3 = Read<uint64_t>(test2 + 0x28); //day
		uint64_t AtmosphereParameters = Read<uint64_t>(test3 + 0x48); //light
		uint64_t CycleParameters = Read<uint64_t>(test3 + 0x38); //sun
		
		uint64_t TOD_CycleParameters = Read<uint64_t>(test3 + 0x38);
		uint64_t TOD_DayParameters = Read<uint64_t>(test3 + 0x50);
		uint64_t TOD_NightParameters = Read<uint64_t>(test3 + 0x58);
		uint64_t TOD_CloudParamaters = Read<uint64_t>(test3 + 0x78);
		uint64_t TOD_AmbientParameters = Read<uint64_t>(test3 + 0x90);

		if (Settings::nightSky)
		{
			Write<float>(TOD_AmbientParameters + 0x18, 1.f);//AmbientMultiplier
			Write<float>(TOD_NightParameters + 0x50, 6.f);//AmbientMultiplier
			Write<float>(TOD_NightParameters + 0x54, 1.f);//ReflectionMultiplier
			Write<float>(TOD_DayParameters + 0x50, 1.f);//AmbientMultiplier
			Write<float>(TOD_DayParameters + 0x54, 1.f);//ReflectionMultiplier
		}
		if (Settings::skyColorBool)
		Write<float>(AtmosphereParameters + 0x10, Settings::SkyColor);
		if (Settings::night_mode)
		Write<float>(TodCycle + 0x10, Settings::time);
	}
#pragma endregion



	void FixDebug()
	{
		DWORD64 Client = Read<DWORD64>(get_module_base_address(("GameAssembly.dll")) + 0x3274518 + 0xB8);//ConVar_Client_c*
		Write<float>(Client + 0x2C, 1.f);// camspeed
		Write<float>(Client + 0x20, 1.f);// camlerp
	}


	void LongNeck()
	{
		if (GetAsyncKeyState(Settings::LongNeckKey))
		{
			DWORD64 eyes = Read<DWORD64>(this->player + 0x660);
			Write<Vector3>(eyes + 0x38, Vector3(0, (0.8f), 0));
		}
	}


HeldItem getHeldItem()
{
	int active_weapon_id = Read<int>(this->player + 0x5D0); //private uint clActiveItem;

	uint64_t items = ReadChain<uint64_t>(this->player, { (uint64_t)0x668, (uint64_t)0x28, (uint64_t)0x38, 0x10 }); //public PlayerInventory inventory;

	//std::cout << "Held weapon: found :" <<  items << std::endl;


	for (int items_on_belt = 0; items_on_belt <= 6; items_on_belt++)
	{
		uint64_t item = Read<uint64_t>(items + 0x20 + (items_on_belt * 0x8));

		int active_weapon = Read<uint32_t>(item + 0x28);

		if (active_weapon_id == active_weapon)
		{
			HeldItem item_obj = HeldItem(item);

			return item_obj;
		}
	}

	return 0;
}

std::wstring getPlayerName() {
	std::wstring name = ReadUnicode(Read<uint64_t>(this->player + 0x6B8) + 0x14); //BasePlayer -> protected string _displayName

	if (name.find(safe_strW(L"Scientist")) == 0)
		return safe_strW(L"Scientist");

	return name;
}

public:

	uintptr_t player{};
	uintptr_t visualState{};
	std::string name{};
	bool isCalled{};
	std::wstring nameW{};
	Vector3 position{};
	int32_t playerFlags{};
	int entityFlags{};
	uint64_t playerModel{};
	uint64_t modelState{};
	float health{};

}; std::unique_ptr<std::vector<BaseEntity>> entityList;

#pragma endregion

class EntityCorpse {
public:

	EntityCorpse() {}

	EntityCorpse(uintptr_t _ent, uintptr_t _trans, uintptr_t _obj) {
		this->ent = Read<uintptr_t>(_ent + 0x28);
		this->trans = Read<uintptr_t>(_trans + 0x38);

		this->name = ReadNative(_obj + 0x60);
	}

public:

	uintptr_t ent{};
	uintptr_t trans{};
	std::string name{};
}; std::unique_ptr<std::vector<EntityCorpse>> corpseList;

#pragma region PMClass

class EntityMovment : public BaseEntity, EntityCorpse {
public:

	EntityMovment() {}

	EntityMovment(uintptr_t player) {        //0x4D0
		this->playerMovement = Read<uintptr_t>(player + O::BaseMovement); 
	}

	void alwaysShoot() {
		Write<float>(this->playerMovement + 0x48, 1);
	}

	void setFov() {
		auto klass = Read<DWORD64>(gBase + ConVar_Graphics_c); //ConVar.Graphics_TypeInfo
		auto staticFields = Read<DWORD64>(klass + 0xB8);
		Write<float>(staticFields + 0x18, Settings::FovSlider);//0x18 => m_camera
	}

	void zoom()
	{
		if (GetAsyncKeyState(Settings::zoomKey))
		{
			auto klass = Read<DWORD64>(gBase + ConVar_Graphics_c); //ConVar.Graphics_TypeInfo
			auto staticFields = Read<DWORD64>(klass + 0xB8);
			Write<float>(staticFields + 0x18, 20.f);//0x18 => m_camera
		}
	}

	



	void infiniteJump() {
		if (GetAsyncKeyState(Settings::jumpKey))
		{
			Write<float>(this->playerMovement + 0xC0, 0);
			Write<Vector3>(this->playerMovement + 0xBC, Vector3(9999999, 9999999, 9999999));
		}
	}

	void FlyHack()
	{
		if (Settings::flyHackkk)
		{
			if (GetAsyncKeyState(Settings::flyhackKey))
			{
				Write<float>(this->modelState + O::waterLevel, 2);
				Write<float>(this->playerMovement + O::gravityTestRadius, -2);
				Write<float>(this->playerMovement + O::groundAngle, 0);
				Write<float>(this->playerMovement + O::groundAngleNew, 0);
				Write<float>(this->playerMovement + O::capsuleHeight, -300);
				Write<float>(this->playerMovement + O::capsuleCenter, -300);

				if(GetAsyncKeyState(VK_SPACE))
					Write<float>(this->playerMovement + O::gravityMultiplier, Settings::flyhackSpeed);
				else
					Write<float>(this->playerMovement + O::gravityMultiplier, 0.1);
			}
			else
			{
				Write<float>(this->playerMovement + O::groundAngle, 0);
				Write<float>(this->playerMovement + O::groundAngleNew, 50);
				Write<float>(this->playerMovement + O::gravityMultiplier, 2.5f);
				Write<float>(this->playerMovement + O::capsuleHeight, 1.79f);
				Write<float>(this->playerMovement + O::capsuleCenter, 0.899f);
			}
		}
	}

	void spiderClimb() {
		Write<float>(this->playerMovement + O::groundAngle, 0.f);
		Write<float>(this->playerMovement + O::groundAngleNew, 0.f);
	}

	void KillHack()
	{
		Write<bool>(this->playerMovement + O::wasFalling, true);
		Write<Vector3>(this->playerMovement + O::previousVelocity, Vector3(0, -20, 0));
		Write<float>(this->playerMovement + O::groundTime, 0.f);
	}

	void Gravity()
	{
		Write<float>(this->playerMovement + 0x7C, Settings::GravitySlider);
	}

	void walkOnWater() {
		if (GetAsyncKeyState(Settings::walkWaterKEY)) {
			Write<float>(this->playerMovement + O::groundAngle, 0.f);//groundAngle
			Write<float>(this->playerMovement + O::groundAngleNew, 0.f);//groundAngleNew
			Write<float>(this->playerMovement + O::gravityMultiplier, 0.f);//gravityMultiplier
		}
		else {
			Write<float>(this->playerMovement + O::gravityMultiplier, 2.5f);//gravityMultiplier
		}
	}

public:

	uint64_t playerMovement{};
	uint64_t playerModelFlags{};
	uint64_t Mounted{};
};

#pragma endregion

class BaseMiscEntity {
public:
	BaseMiscEntity() {}

	BaseMiscEntity(uintptr_t _ent, uintptr_t _trans, uintptr_t _obj) {
		this->ent = Read<uintptr_t>(_ent + 0x28);
		this->trans = Read<uintptr_t>(_trans + 0x38);
		this->name = ReadNative(_obj + 0x60);

		if (this->name.find(safe_str("stone-ore")) != std::string::npos)
			this->name = safe_str("STONE");
		else if (this->name.find(safe_str("metal-ore")) != std::string::npos)
			this->name = safe_str("METAL");
		else if (this->name.find(safe_str("sulfur-ore")) != std::string::npos)
			this->name = safe_str("SULFUR");
		else if (this->name.find(safe_str("stone-collectable")) != std::string::npos)
			this->name = safe_str("STONE");
		else if (this->name.find(safe_str("metal-collectable")) != std::string::npos)
			this->name = safe_str("METAL");
		else if (this->name.find(safe_str("sulfur-collectable")) != std::string::npos)
			this->name = safe_str("SULFUR");
		else if (this->name.find(safe_str("wood-collectable")) != std::string::npos)
			this->name = safe_str("WOOD");
		else if (this->name.find(safe_str("hemp-collectable")) != std::string::npos || this->name.find(safe_str("hemp.entity.prefab")) != std::string::npos)
			this->name = safe_str("HEMP");
		else if (this->name.find(safe_str("small_stash_deployed")) != std::string::npos)
			this->name = safe_str("STASH");
		else if (this->name.find(safe_str("loot_barrel_1.prefab")) != std::string::npos || this->name.find(safe_str("loot_barrel_2.prefab")) != std::string::npos || this->name.find(safe_str("loot-barrel-1.prefab")) != std::string::npos || this->name.find(safe_str("loot-barrel-2.prefab")) != std::string::npos)
			this->name = safe_str("BARREL");
		else if (this->name.find(safe_str("oil_barrel.prefab")) != std::string::npos)
			this->name = safe_str("OIL BARREL");
		else if (this->name.find(safe_str("crate_elite.prefab")) != std::string::npos)
			this->name = safe_str("ELITE CRATE");
		else if (this->name.find(safe_str("crate_normal.prefab")) != std::string::npos)
			this->name = safe_str("MILITARY CRATE");
		else if (this->name.find(safe_str("crate_normal_2_medical.prefab")) != std::string::npos)
			this->name = safe_str("MEDICAL CRATE");
		else if (this->name.find(safe_str("crate_normal_2.prefab")) != std::string::npos)
			this->name = safe_str("REGULAR CRATE");
		else if (this->name.find(safe_str("crate_normal_2_food.prefab")) != std::string::npos)
			this->name = safe_str("FOOD CRATE");
		else if (this->name.find(safe_str("crate_tools.prefab")) != std::string::npos)
			this->name = safe_str("TOOL CRATE");
		else if (this->name.find(safe_str("rowboat.prefab")) != std::string::npos)
			this->name = safe_str("BOAT");
		else if (this->name.find(safe_str("rhib.prefab")) != std::string::npos)
			this->name = safe_str("RHIB");
		else if (this->name.find(safe_str("kayak.prefab")) != std::string::npos)
			this->name = safe_str("KAYAK");
		else if (this->name.find(safe_str("minicopter.entity.prefab")) != std::string::npos)
			this->name = safe_str("MINICOPTER");
		else if (this->name.find(safe_str("bradleyapc.prefab")) != std::string::npos)
			this->name = safe_str("BRADLEY");

		else
			this->name = safe_str("Error");

	}

public:
	uintptr_t ent{};
	uintptr_t trans{};
	std::string name{};
}; std::unique_ptr<std::vector<BaseMiscEntity>> oreList;//scans for ores




class BaseWeaponESP {
public:
	BaseWeaponESP() {}

	BaseWeaponESP(uintptr_t _ent, uintptr_t _trans, std::string _name) {
		this->ent = Read<uintptr_t>(_ent + 0x28);
		this->trans = Read<uintptr_t>(_trans + 0x38);
		this->name = _name;
	}

public:
	uintptr_t ent{};
	uintptr_t trans{};
	std::string name{};
}; std::unique_ptr<std::vector<BaseWeaponESP>> weaponList;

#pragma region LPClass

class LocalPlayer {
public:
	std::unique_ptr<BaseEntity> Player;
	std::unique_ptr<EntityMovment> Movement;
}; std::unique_ptr<LocalPlayer> localPlayer;

#pragma endregion