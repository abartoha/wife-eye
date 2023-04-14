#include <iostream>
#include <string>
#include <regex>
#include <cstdlib>

int main() {
    std::cout << R"(
             ___       __   ___  ________ _______           _______       ___    ___ _______      
            |\  \     |\  \|\  \|\  _____\\  ___ \         |\  ___ \     |\  \  /  /|\  ___ \     
            \ \  \    \ \  \ \  \ \  \__/\ \   __/|        \ \   __/|    \ \  \/  / | \   __/|    
             \ \  \  __\ \  \ \  \ \   __\\ \  \_|/__       \ \  \_|/__   \ \    / / \ \  \_|/__  
              \ \  \|\__\_\  \ \  \ \  \_| \ \  \_|\ \       \ \  \_|\ \   \/  /  /   \ \  \_|\ \ 
               \ \____________\ \__\ \__\   \ \_______\       \ \_______\__/  / /      \ \_______\
                \|____________|\|__|\|__|    \|_______|        \|_______|\___/ /        \|_______|
                                                                        \|___|/                   
                                                                        
    )" << std::endl;
    std::cout << R"(
                    ____ ____ ____ ___    ____ ___  ___  ___  _  _ ____ ____ ___  ___    
                    |  _\|   ||   ||  \   | . \|  \ | _\ | _\ ||| \|   || . \|  \ | _\   
                    | [ \| . || . || . \  | __/| . \[__ \[__ \||\ /| . ||  <_| . \[__ \  
                    |___/|___/|___/|___/  |/   |/\_/|___/|___/|/\/ |___/|/\_/|___/|___/  )" << std::endl;
    std::cout << R"(    ____ ___  __     _  ____   ____ __   ____ ____ ____   __   ____ ____ _  _ ____ ____ __   _    ___  ___  
    | __\|  \ | \|\  |/ |_ _\  | __\| \|\|_ _\| __\| . \  | \|\|___\|  _\||_|\|_ _\| __\| |  || \ | .\ | _\ 
    | \__| . \|  \|       ||   |  ]_|  \|  || |  ]_|  <_  |  \|| /  | [ \| _ |  || | \__| |__||_|\| .<_[__ \
    |___/|/\_/|/\_/       |/   |___/|/\_/  |/ |___/|/\_/  |/\_/|/   |___/|/ |/  |/ |___/|___/|___/|___/|___/)" << std::endl << std::endl;

    std::cout << "Wifi Passwords: " << std::endl;
    std::string output;
    std::string command = "netsh wlan show profiles";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to execute command" << std::endl;
        return 1;
    }
    char buffer[128];
    while (fgets(buffer, 128, pipe)) {
        output += buffer;
    }
    _pclose(pipe);

    const std::string pattern("All User Profile\\s*: (.+)\r?\n");
    std::regex regex_pattern(pattern);
    std::smatch match;
    std::string wifi_name;
    std::string password_output;
    const std::string password_pattern("Key Content\\s*: (.+)\r?\n");

    for (auto it = std::sregex_iterator(output.cbegin(), output.cend(), regex_pattern); it != std::sregex_iterator(); ++it) {
        match = *it;
        wifi_name = match[1].str();
        std::string password_command = "netsh wlan show profile \"" + wifi_name + "\" key=clear";
        FILE* password_pipe = _popen(password_command.c_str(), "r");
        if (!password_pipe) {
            std::cerr << "\tError: Failed to execute command" << std::endl;
            return 1;
        }
        while (fgets(buffer, 128, password_pipe)) {
            password_output += buffer;
        }
        _pclose(password_pipe);

        std::regex password_regex(password_pattern);
        std::smatch password_match;
        for (auto password_it = std::sregex_iterator(password_output.cbegin(), password_output.cend(), password_regex); password_it != std::sregex_iterator(); ++password_it) {
            password_match = *password_it;
            std::cout << "\t" << wifi_name << ": " << password_match[1].str() << std::endl;
        }
        password_output.clear();
    }
    getchar();
    return 0;
}
