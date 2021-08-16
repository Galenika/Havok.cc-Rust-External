#include "SDK/Classes.hpp"
#include "Features/ESP/ESP.hpp"
#include "Overlay/Overlay.hpp"
#include "SDK/BasePlayer.hpp"
#include "Features/Aimbot/DoAimbot.hpp"
#include "Features/MISC/DoMisc.hpp"
#include "Entity/EntityLoop.hpp"
#include <xstring>

int main()
{
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	SetConsoleTitleA("Loader");
	HWND hwnd = nullptr;

	try
	{
		CleanPIDDBCacheTable();

		std::cout << safe_str("Launch Rust") << std::endl;

		while (!hwnd)
		{
			hwnd = FindWindowA(safe_str("UnityWndClass"), safe_str("Rust"));
			Sleep(50);
		}

		std::cout << safe_str("Game has been found! Press F1 when game loads in!") << std::endl;

		while (true)
		{
			if (GetAsyncKeyState(VK_F1))
				break;

			Sleep(50);
		}

		Mutex = std::make_unique<_Mutex>();

		Mutex->PlayerSync = std::make_unique<sf::contention_free_shared_mutex<>>();
		Mutex->WriteSync = std::make_unique<sf::contention_free_shared_mutex<>>();

		entityList = std::make_unique<std::vector<BaseEntity>>();
		corpseList = std::make_unique<std::vector<EntityCorpse>>();
		oreList = std::make_unique<std::vector<BaseMiscEntity>>();
		weaponList = std::make_unique<std::vector<BaseWeaponESP>>();
		localPlayer = std::make_unique<LocalPlayer>();

		pId = get_process_id(safe_str("RustClient.exe"));
		gBase = get_module_base_address(safe_str("GameAssembly.dll"));
		uBase = get_module_base_address(safe_str("UnityPlayer.dll"));

		std::thread overlay_thread(Overlay::Loop);
		std::thread entity_thread(Entity::EntityLoop);
		std::thread aimbot_thread(Aimbot::DoAimbot);
		std::thread misc_thread(Misc::DoMisc);
		


		overlay_thread.join();
	}
	catch (std::exception e)
	{
		std::cerr << safe_str("error occurred: ") << e.what() << std::endl;
	}
}