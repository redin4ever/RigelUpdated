#include <Windows.h>
#include "auth.hpp"
#include <string>
#include "utils.hpp"
#include "skStr.h"
#include "includes.hpp"
#include "SDK.hpp"
#include "Overwatch.hpp"
#include <io.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);




void MainThread() {
	using namespace OW;
	std::cout << "Launch in Main Menu and go to Practice Range..\n";
	while (!SDK->Initialize() || !SDK->GetGlobalKey())
	{
		std::cout << "Waiting Overwatch..\n";
		Sleep(2000);
	}


	_beginthread((_beginthread_proc_type)entity_scan_thread, 0, 0);
	Sleep(50);
	_beginthread((_beginthread_proc_type)entity_thread, 0, 0);
	_beginthread((_beginthread_proc_type)viewmatrix_thread, 0, 0);
	Sleep(500);
	_beginthread((_beginthread_proc_type)aimbot_thread, 0, 0);
	_beginthread((_beginthread_proc_type)overlay_thread, 0, 0);
	_beginthread((_beginthread_proc_type)configsavenloadthread, 0, 0);
	Sleep(10);
	_beginthread((_beginthread_proc_type)looprpmthread, 0, 0);

	while (FindWindowA(skCrypt("TankWindowClass"), NULL))
	{
		auto viewMatrixVal = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base) ^ offset::offset_viewmatrix_xor_key;
		viewMatrixVal = SDK->RPM<uint64_t>(viewMatrixVal + 0x20);
		viewMatrixVal = SDK->RPM<uint64_t>(viewMatrixVal + 0x50);
		viewMatrix_xor_ptr = viewMatrixVal + 0x140;
		auto view = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base_test) + offset::offset_viewmatrix_ptr;
		viewMatrixPtr = view;
	}
}

int main()
{

	if (!OW::Config::loginornot) MainThread();
	std::string consoleTitle = (std::string)skCrypt("Loader - Built at:  ") + compilation_date + " " + compilation_time;
	SetConsoleTitleA(consoleTitle.c_str());
	std::cout << skCrypt("\n\nConnecting to server..");
	using namespace OW;

	bool is_login = false;
	std::cout << "Loading functions..\n";
	MainThread();
	Sleep(10000);
	exit(0);
}

std::string tm_to_readable_time(tm ctx) {
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
	auto cv = strtol(timestamp.c_str(), NULL, 10);

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}
