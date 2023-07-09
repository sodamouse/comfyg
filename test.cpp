#define COMFYG_IMPL
#include "comfyg.hpp"
#include <iostream>

int main()
{
    const char** stringOption = Comfyg::config_str("string_option", {});
    bool* boolOption = Comfyg::config_bool("boolean option", {});
    int* intOption = Comfyg::config_int("intOption", {});
    double* doubleOption = Comfyg::config_double("DoubleOption", {});
    Comfyg::load_config_file("config.conf");

    std::cout << *stringOption << '\n';
    std::cout << *boolOption << '\n';
    std::cout << *intOption << '\n';
    std::cout << *doubleOption << '\n';

    return 0;
}
