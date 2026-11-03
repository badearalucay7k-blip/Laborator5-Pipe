#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

int main() {
    std::cout << "Procesul principal cauta numere prime cu 10 procese fiu (Linux)...\n\n";

    for (int i = 0; i < 10; i++) {
        int start = i * 1000 + 1;
        int end = (i + 1) * 1000;
        if (i == 0) start = 1;

        int fd[2];
        if (pipe(fd) == -1) {
            std::cerr << "Eroare la crearea pipe-ului\n";
            return 1;
        }

        pid_t pid = fork();

        if (pid < 0) {
            std::cerr << "Eroare la crearea procesului fiu\n";
            return 1;
        }

        if (pid == 0) {
            close(fd[0]);

            std::string primes = "";
            for (int num = start; num <= end; num++) {
                if (isPrime(num)) {
                    primes += std::to_string(num) + " ";
                }
            }

            write(fd[1], primes.c_str(), primes.length());
            close(fd[1]);
            _exit(0);
        } 
        else {
            close(fd[1]);

            std::cout << "[Fiu " << (i + 1) << " | Interval " << start << "-" << end << "]: ";

            char buffer[4096];
            ssize_t bytesRead = read(fd[0], buffer, sizeof(buffer) - 1);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::cout << buffer;
            }
            
            close(fd[0]);
            std::cout << "\n\n";

            wait(NULL);
        }
    }

    std::cout << "Toate procesele au terminat cautarea!\n";
    return 0;
}
