#pragma once

// Copyright (c) 2023 sodamouse - See LICENSE.md

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
#include <cstdio>

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
constexpr char COMMENT_GLYPHS[]{'#', '%', '[', ']', '(', ')', '/', '$'};

constexpr std::size_t SPLITS_MAX = 2;
std::string SPLITS[SPLITS_MAX];
std::size_t splitIdx = 0;

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

void split_string(const std::string& str, char delimiter)
{
    auto strSize = str.size();
    assert(strSize && "String cannot be empty");

    std::size_t beg = 0;
    std::size_t size = 0;
    bool found = false;
    for (std::size_t i = 0; i <= strSize; ++i)
    {
        if (str[i] == delimiter && !found)
        {
            SPLITS[splitIdx++] = str.substr(beg, size);
            beg = i + 1;
            size = 0;
            found = true;
        }

        if (i == strSize)
        {
            SPLITS[splitIdx++] = str.substr(beg, strSize);
            break;
        }

        ++size;
    }

    splitIdx = 0;
}

std::string strip_string(const std::string& str)
{
    std::size_t firstIndex = 0;
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
    while (std::getline(file, line))
    {
        if (line.size() == 0)
            continue;

        for (std::size_t i = 0; i < CHAR_ARRAY_SIZE(COMMENT_GLYPHS); ++i)
        {
            if (line[0] == COMMENT_GLYPHS[i])
                goto cnt;   // Skip the entire while loop
        }

        split_string(line, '=');
        SPLITS[0] = strip_string(SPLITS[0]);
        SPLITS[1] = strip_string(SPLITS[1]);

        if (SPLITS[0].size() == 0 || SPLITS[1].size() == 0)
            continue;

        for (std::size_t i = 0; i < config_index; ++i)
        {
            if (CONFIGS[i].name == SPLITS[0])
            {
                switch (CONFIGS[i].type)
                {
                case CONFIG_BOOL: {
                    if (SPLITS[1] == "false")
                    {
                        CONFIGS[i].asBool = false;
                    }

                    else if (SPLITS[1] == "true")
                    {
                        CONFIGS[i].asBool = true;
                    }

                    else
                    {
                        fprintf(stderr, "Could not parse value assigned to `%s` as bool.\n", CONFIGS[i].name);
                    }
                }
                break;
                case CONFIG_INT: {
                    try
                    {
                        CONFIGS[i].asInt = std::stoi(SPLITS[1]);
                    }

                    catch (const std::exception& error)
                    {
                        fprintf(stderr, "Could not parse value assigned to `%s` as integer.\n", CONFIGS[i].name);
                        exit(1);
                    }
                }
                break;
                case CONFIG_DOUBLE: {
                    try
                    {
                        CONFIGS[i].asDouble = std::atof(SPLITS[1].c_str());
                    }

                    catch (const std::exception& error)
                    {
                        fprintf(stderr, "Could not parse value assigned to `%s` as double.\n", CONFIGS[i].name);
                        exit(1);
                    }
                }
                break;
                case CONFIG_STR: {
                    // Some tools see this as a memory leak. Those tools are wrong.
                    char* str = new char[SPLITS[1].size() + 1];
                    std::copy(SPLITS[1].begin(), SPLITS[1].end(), str);
                    str[SPLITS[1].size()] = 0;
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
