#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc >= 3) {
        int start = std::stoi(argv[1]);
        int end = std::stoi(argv[2]);

        for (int i = start; i <= end; i++) {
            if (isPrime(i)) {
                std::cout << i << " ";
            }
        }
        return 0;
    }

    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    std::cout << "Procesul principal cauta numere prime cu 10 procese fiu...\n\n";

    for (int i = 0; i < 10; i++) {
        int start = i * 1000 + 1;
        int end = (i + 1) * 1000;
        if (i == 0) start = 1;

        HANDLE hReadPipe, hWritePipe;
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;
        si.dwFlags |= STARTF_USESTDHANDLES;
        ZeroMemory(&pi, sizeof(pi));

        std::string cmd = std::string(path) + " " + std::to_string(start) + " " + std::to_string(end);
        char* cmdLine = const_cast<char*>(cmd.c_str());

        if (!CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            std::cerr << "Eroare la crearea procesului " << i + 1 << "\n";
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            continue;
        }

        CloseHandle(hWritePipe);

        std::cout << "[Fiu " << (i + 1) << " | Interval " << start << "-" << end << "]: ";

        char buffer[128];
        DWORD bytesRead;
        while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << buffer;
        }
        std::cout << "\n\n";

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hReadPipe);
    }

    std::cout << "Toate procesele au terminat cautarea!\n";
    return 0;
}