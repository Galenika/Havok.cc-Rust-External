#pragma once
#include "Imports.hpp"
#include "Driver.hpp"
#include "Utils.hpp"

namespace O {
	auto BaseMovement = 0x4E8;// public BaseMovement movement;
	auto waterLevel = 0x24;// public float waterLevel
	auto gravityTestRadius = 0x80;// public float gravityTestRadius
	auto groundAngle = 0xC8;// groundAngle
	auto groundAngleNew = 0xC4;// groundAngleNew
	auto capsuleHeight = 0x68;// public float capsuleHeight
	auto capsuleCenter = 0x6C;// public float capsuleCenter
	auto gravityMultiplier = 0x84;// public float gravityMultiplier;
	auto wasFalling = 0x14B;// private bool wasFalling
	auto previousVelocity = 0xE4;// private Vector3 previousVelocity
	auto groundTime = 0xCC;// private float groundTime
	auto clothingMoveSpeedReduction = 0x72C;// public float clothingMoveSpeedReduction;
	auto heldEntity = 0x98;// private EntityRef heldEntity;
	auto recoil = 0x2D8;// public RecoilProperties recoil;
	auto primaryMagazine = 0x2B8;//0x2A0 = public BaseProjectile.Magazine primaryMagazine;

	auto aimSway = 0x2D0;// public float aimSway;
	auto aimSwaySpeed = 0x2D4;// public float aimSwaySpeed;
	auto aimCone = 0x2E8;// public float aimCone;
	auto hipAimCone = 0x2EC;// public float hipAimCone;
	auto aimconePenaltyPerShot = 0x2F0;// public float aimconePenaltyPerShot;
	auto aimConePenaltyMax = 0x2F4;// public float aimConePenaltyMax;
}

class HeldItem {
public:

	HeldItem() {}

	HeldItem(uint64_t _ent) {
		this->ent = _ent;

		this->name = GetItemName();

		this->bp = Read<uintptr_t>(this->ent + O::heldEntity);

		this->recoil_properties = Read<uintptr_t>(this->bp + O::recoil);

		this->primary_mag = Read<uint64_t>(this->bp + O::primaryMagazine);
	}

	std::string GetItemName()
	{
		uint64_t info = Read<uint64_t>(this->ent + 0x20);

		uint64_t display_name = Read<uint64_t>(info + 0x20);

		std::wstring wide_name = ReadUnicode(display_name + 0x14);

		if (!wide_name.empty())
			return std::string(wide_name.begin(), wide_name.end()).c_str();

		//std::cout <<  "Display name " << display_name << std::endl;

		return safe_str("No Item");
	}

	std::string GetActiveWeaponName(uint64_t myWeapon)
	{
		uint64_t itemDefinition = Read<uint64_t>(myWeapon + 0x20);
		uint64_t display_name = Read<uint64_t>(itemDefinition + 0x28);
		uint64_t english = Read<uint64_t>(display_name + 0x18);
		if (!english)
			return std::string();
		else
			return safe_str("nothing");
	}

	void setNoAimCone() {
		Write<float>(this->bp + O::aimCone, 0.f);
		Write<float>(this->bp + O::aimConePenaltyMax, 0.f);
		Write<float>(this->bp + O::aimconePenaltyPerShot, 0.f);
		Write<float>(this->bp + O::aimSway, 0.f);
		Write<float>(this->bp + O::aimSwaySpeed, 0.f);
		Write<float>(this->bp + O::hipAimCone, 0.f);
	}

	void AntiSpread()
	{
		Write<float>(this->bp + 0x31C, 0.f);
		Write<float>(this->bp + 0x320, 0.f);
		Write<float>(this->bp + 0x2E8, 0.f);
		Write<float>(this->bp + 0x2EC, 0.f);
		Write<float>(this->bp + 0x2F0, 0.f);
	}

	void setNoSway() {
		Write<float>(this->bp + 0x2B8, 0.f);
		Write<float>(this->bp + 0x2BC, 0.f);
	}


	void RunHit() {
		Write<int>(this->bp + 0x299, 0);
	}

	uintptr_t getRecoilProp() {
		const auto recoil_properties = Read<uintptr_t>(this->bp + O::recoil);
		if (recoil_properties)
			return recoil_properties;
	}

	void fatBullet()
	{
		auto List = Read<DWORD64>(this->bp + 0x358); //private List<Projectile> createdProjectiles;
		List = Read<DWORD64>(List + 0x10);
		for (int i = 0; i < 8; ++i)
		{
			UINT64 Item = Read<UINT64>(List + 0x20 + (i * 0x8));
			Write<float>(Item + 0x2C, 1.5f);
		}
	}
	
	void rapidFire() {
			Write<float>(this->bp + 0x1F4, Settings::rapidfirevalue);
			Write<bool>(this->bp + 0x298, true); //this is automatic fire
	}

	void instantCompound() {
		if (this->name.find(safe_str("bow.compound")) != std::string::npos)
		{
			Write<float>(bp + 0x378, 0);//stringHoldDurationMax
			Write<float>(bp + 0x37C, 1000000);//stringBonusDamage
			Write<float>(bp + 0x388, 1000000);//movementPenaltyRampUpTime
		}
	}

	void setLonghit(float input)
	{
		Write(this->bp + 0x290, input);
	}

	void FastMed(float input)
	{
		Write(this->bp + 0x280, input);
	}

	void fastSwitch() {
			Write<float>(bp + 0x1F0, 0);//deployDelay | instant deploy > AttackEntity
	}


	//causes a crash if switched to another weapon (i will fix - johnn)
	void instantEoka() {
		if (this->name.find(safe_str("eoka")) != std::string::npos)
		{
			Write<bool>(this->bp + 0x370, true);//_didSparkThisFrame
			Write<float>(this->bp + 0x360, 100.f);//successFraction
		}
	}


	bool IsWeapon()
	{
		if (this->name.find(safe_str("rifle")) != std::string::npos)
		{
			return true;
		}
		if (this->name.find(safe_str("pistol")) != std::string::npos)
		{
			return true;
		}
		if (this->name.find(safe_str("bow")) != std::string::npos)
		{
			return true;
		}
		if (this->name.find(safe_str("lmg")) != std::string::npos)
		{
			return true;
		}
		if (this->name.find(safe_str("shotgun")) != std::string::npos)
		{
			return true;
		}
		if (this->name.find(safe_str("smg")) != std::string::npos)
		{
			return true;
		}
		else
			return false;
	}

	std::string GetAmmoType()
	{
		uint64_t item_def = Read<uint64_t>(this->primary_mag + 0x20);
		uint64_t short_name = Read<uint64_t>(item_def + 0x20);

		std::wstring wide_name = ReadUnicode(short_name + 0x14);

		if (!wide_name.empty())
			return std::string(wide_name.begin(), wide_name.end()).c_str();

		return safe_str("No Ammo");
	}

public:
	uint64_t ent;
	uintptr_t bp;
	uint64_t playerMovement2{};
	uintptr_t recoil_properties;
	uint64_t primary_mag;
	std::string name;
};
