#pragma once

// The MIT License (MIT)

// Copyright (c) 2023 sodamouse

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

namespace Comfyg {

bool* config_bool(const char* name, bool defaultValue);
int* config_int(const char* name, int defaultValue);
double* config_double(const char* name, double defaultValue);
const char** config_str(const char* name, const char* defaultValue);
void load_config_file(const char* path);

} // namespace Comfyg

#ifdef COMFYG_IMPL

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

#define CHAR_ARRAY_SIZE(x) (sizeof(x) / sizeof(char))

namespace Comfyg {

enum ConfigType {
    CONFIG_BOOL,
    CONFIG_INT,
    CONFIG_DOUBLE,
    CONFIG_STR,
};

struct Config
{
    ConfigType type;
    const char* name;
    union {
        bool asBool;
        int asInt;
        double asDouble;
        const char* asStr;
    };
};

constexpr std::size_t CONFIG_MAX = 99;
Config CONFIGS[CONFIG_MAX]{};
std::size_t config_index = 0;
char COMMENT_GLYPHS[]{'#', '%', '[', ']', '(', ')', '/', '$'};

Config* config_new(ConfigType type, const char* name)
{
    auto* config = &CONFIGS[config_index++];
    config->type = type;
    config->name = name;

    return config;
}

bool* config_bool(const char* name, bool defaultValue)
{
    auto* config = config_new(CONFIG_BOOL, name);
    config->asBool = defaultValue;

    return &config->asBool;
}

int* config_int(const char* name, int defaultValue)
{
    auto* config = config_new(CONFIG_INT, name);
    config->asInt = defaultValue;

    return &config->asInt;
}

double* config_double(const char* name, double defaultValue)
{
    auto* config = config_new(CONFIG_DOUBLE, name);
    config->asDouble = defaultValue;

    return &config->asDouble;
}

const char** config_str(const char* name, const char* defaultValue)
{
    auto* config = config_new(CONFIG_STR, name);
    config->asStr = defaultValue;

    return &config->asStr;
}

std::vector<std::string> split_string(const std::string& str, char delimiter)
{
    auto strSize = str.size();
    assert(strSize && "String cannot be empty");
    std::vector<std::string> splits;

    std::size_t beg = 0;
    std::size_t size = 0;
    for (std::size_t i = 0; i <= strSize; ++i)
    {
        if (str[i] == delimiter)
        {
            splits.emplace_back(str.substr(beg, size));
            beg = i + 1;
            size = 0;
        }

        if (i == strSize)
        {
            splits.emplace_back(str.substr(beg, strSize));
            break;
        }

        ++size;
    }

    return splits;
}

std::string strip_string(const std::string& str)
{
    auto firstIndex = 0;
    for (; str[firstIndex] == ' '; ++firstIndex)
        ;

    auto lastIndex = str.size() - 1;
    for (; str[lastIndex] == ' '; --lastIndex)
        ;

    return str.substr(firstIndex, lastIndex - firstIndex + 1);
}

void load_config_file(const char* path)
{
    std::fstream file(path, std::ios::in);
    std::string line;

    // Stores a pair of strings, corresponding to key/value of a single config.
    std::vector<std::string> kv;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
            continue;

        for (std::size_t i = 0; i < CHAR_ARRAY_SIZE(COMMENT_GLYPHS); ++i)
        {
            if (line[0] == COMMENT_GLYPHS[i])
                goto cnt;
        }

        kv = split_string(line, '=');
        kv[0] = strip_string(kv[0]);
        kv[1] = strip_string(kv[1]);

        if (kv[0].size() == 0 || kv[1].size() == 0)
            continue;

        for (std::size_t i = 0; i < config_index; ++i)
        {
            if (CONFIGS[i].name == kv[0])
            {
                switch (CONFIGS[i].type)
                {
                case CONFIG_BOOL: {
                    if (kv[1] == "false")
                    {
                        CONFIGS[i].asBool = false;
                    }

                    else if (kv[1] == "true")
                    {
                        CONFIGS[i].asBool = true;
                    }

                    else
                    {
                        std::cout << "Option `" << CONFIGS[i].name
                                  << "` expects `true` or `false` but `" << kv[1]
                                  << "` was provided.\n";
                        exit(1);
                    }
                }
                break;
                case CONFIG_INT: {
                    try
                    {
                        CONFIGS[i].asInt = std::stoi(kv[1]);
                    }

                    catch (const std::exception& error)
                    {
                        std::cerr << "Could not parse `" << kv[1] << "` as integer.\n";
                        exit(1);
                    }
                }
                break;
                case CONFIG_DOUBLE: {
                    try
                    {
                        CONFIGS[i].asDouble = std::atof(kv[1].c_str());
                    }

                    catch (const std::exception& error)
                    {
                        std::cerr << "Could not parse `" << kv[1] << "` as double.\n";
                        exit(1);
                    }
                }
                break;
                case CONFIG_STR: {
                    // Some tools see this as a memory leak. Those tools are wrong.
                    char* str = new char[kv[1].size() + 1];
                    std::copy(kv[1].begin(), kv[1].end(), str);
                    str[kv[1].size()] = 0;
                    CONFIGS[i].asStr = str;
                }
                break;
                }
            }
        }
    cnt:;
    }
}

} // namespace Comfyg
#endif
