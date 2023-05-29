# Comfyg
The comfy config file parser library.

## Usage
``` c++
#define COMFYG_IMPL
#include "comfyg.hpp"

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
```

## Description
Comfyg is a single-file, header-only library for quickly setting up loading configruation parameters from a file. Its fast, light-weight, and very easy to extend.

Please see the accompanied config.conf file for parameter syntax.
