#include <Directory.h>

#include <Windows.h>
#include <string.h>
#include <iostream>
#include <vector>


void* CreateMapFile(const wchar_t* fileName, size_t size) {
	HANDLE hFile = CreateFile(
		fileName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Could not create file %ls : %lu \n", fileName, GetLastError());
		return nullptr;
	}

	HANDLE hMapFile = CreateFileMapping(
		hFile,
		nullptr,
		PAGE_READWRITE,
		0,
		size,
		nullptr);

	if (hMapFile == nullptr) {
		CloseHandle(hFile);
		printf("Could not create file mapping for %ls : %lu \n", fileName, GetLastError());
		return nullptr;
	}

	void* pMap = MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		size);

	if (pMap == nullptr) {
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		printf("Could not create map view of file for %ls : %lu \n", fileName, GetLastError());
		return nullptr;
	}

	CloseHandle(hMapFile);
	CloseHandle(hFile);

	return pMap;
}


void LaunchAndWaitProcesses() {
	const int ProcessCount = 3;
	HANDLE processHandles[ProcessCount]{};
	STARTUPINFOA si[ProcessCount]{};
	PROCESS_INFORMATION pi[ProcessCount]{};
	
	for (int i = 0; i < ProcessCount; ++i) {
		ZeroMemory(&si[i], sizeof(STARTUPINFOA));
		si[i].cb = sizeof(STARTUPINFOA);
		ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
	}

	char processNames[ProcessCount][MAX_PATH];
	strcpy_s(processNames[0], "deposit.exe");
	strcpy_s(processNames[1], "sold.exe");
	strcpy_s(processNames[2], "donate.exe");

	char path[MAX_PATH] = "C:\\Facultate\\CSSO\\Week4";

	for (int i = 0; i < ProcessCount; ++i) {
		char pathFile[MAX_PATH];
		strcpy_s(pathFile, path);
		strcat_s(pathFile, "\\");
		strcat_s(pathFile, processNames[i]);
		if (!CreateProcessA(
			pathFile,
			processNames[i],
			nullptr,
			nullptr,
			FALSE,
			0,
			nullptr,   // Mediul de utilizare
			nullptr,   // Directorul curent
			&si[i], // STARTUPINFO
			&pi[i]  // PROCESS_INFORMATION
		)) {
			printf("Could not start process %s : %lu \n", processNames[i], GetLastError());
			return;
		}
		processHandles[i] = pi[i].hProcess;
	}

	WaitForMultipleObjects(ProcessCount, processHandles, TRUE, INFINITE);

	for (int i = 0; i < ProcessCount; ++i) {
		CloseHandle(pi[i].hProcess);
		CloseHandle(pi[i].hThread);
	}
}

void CreateInitialFile(const wchar_t* filePath) {
	HANDLE hFile = CreateFile(
		filePath,
		GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Could not create file %ls : %lu \n", filePath, GetLastError());
		return;
	}

	DWORD written;
	DWORD value = 0;
	BOOL writeResult = WriteFile(
		hFile,
		&value,
		sizeof(value),
		&written,
		nullptr);

	if (!writeResult) {
		printf("Could not write to file : %lu \n", GetLastError());
	}

	CloseHandle(hFile);
}

bool FileExists(const LPCWSTR fileName) {
	DWORD fileAttr = GetFileAttributes(fileName);
	if (fileAttr == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return true;
}

void ReadAndDisplayFile(const LPCWSTR filePath) {
	HANDLE hFile = CreateFile(
		filePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Could not open file %ls : %lu \n", filePath, GetLastError());
		return;
	}

	DWORD read;
	DWORD value = 0;
	BOOL readResult = ReadFile(
		hFile,
		&value,
		sizeof(value),
		&read,
		nullptr);

	if (!readResult || read != sizeof(value)) {
		printf("Could not read from file %ls : %lu \n", filePath, GetLastError());
		CloseHandle(hFile);
		return;
	}

	printf("Value in %ls : %lu \n", filePath, value);
	CloseHandle(hFile);
}

void ReadAndDisplayTextFile(const LPCWSTR filePath) {
	HANDLE hFile = CreateFile(
		filePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Unable to open file %ls: %lu \n", filePath, GetLastError());
		return;
	}

	const DWORD bufferSize = 1024;
	char buffer[bufferSize]{};
	DWORD bytesRead;

	while (ReadFile(hFile, buffer, bufferSize - 1, &bytesRead, nullptr) && bytesRead > 0) {
		buffer[bytesRead] = '\0';
		std::cout << buffer;
	}

	CloseHandle(hFile);
}

int main() {
	std::wstring path = L"C:\\Facultate\\CSSO\\Week4\\Reports\\Summary";
	auto dir = Directory(path);
	dir.createDirectories();

	const size_t Size = 10000 * sizeof(DWORD);

	auto pMarketShelves = static_cast<DWORD*>(CreateMapFile(L"MarketShelves", Size));
	auto pMarketValability = static_cast<DWORD*>(CreateMapFile(L"MarketValability", Size));
	auto pProductPrices = static_cast<DWORD*>(CreateMapFile(L"ProductPrices", Size));

	if (pMarketShelves == nullptr || pMarketValability == nullptr || pProductPrices == nullptr) {
		printf("Error creating map files: %lu \n", GetLastError());
		return 1;
	}

	CreateInitialFile(L"C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\sold.txt");
	CreateInitialFile(L"C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\donations.txt");

	LaunchAndWaitProcesses();

	if (FileExists(L"C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\errors.txt")) {
		ReadAndDisplayTextFile(L"C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\errors.txt");
	}
	else {
		ReadAndDisplayFile(L"C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\sold.txt");
		ReadAndDisplayFile(L"C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\donations.txt");
	}

	UnmapViewOfFile(pMarketShelves);
	UnmapViewOfFile(pMarketValability);
	UnmapViewOfFile(pProductPrices);

	return 0;

}