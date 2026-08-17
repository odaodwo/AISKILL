#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

using Json = nlohmann::json;

#ifdef _WIN32
inline std::string toUtf8(const std::wstring& value)
{
    if (value.empty())
        return {};
    const int size = WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()),
                                         nullptr, 0, nullptr, nullptr);
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()),
                        result.data(), size, nullptr, nullptr);
    return result;
}

inline std::vector<std::string> makeArgs(int argc, wchar_t* argv[])
{
    std::vector<std::string> args;
    args.reserve(argc);
    for (int index = 0; index < argc; ++index)
        args.push_back(toUtf8(argv[index]));
    return args;
}
#else
inline std::vector<std::string> makeArgs(int argc, char* argv[])
{
    return {argv, argv + argc};
}
#endif

inline Json loadMemory(const std::string& path)
{
    std::ifstream input(path);
    if (!input)
        throw std::runtime_error("无法打开记忆文件: " + path);

    Json memory;
    input >> memory;
    if (!memory.contains("ALLFoundMusic") || !memory["ALLFoundMusic"].is_array())
        memory["ALLFoundMusic"] = Json::array();
    if (!memory.contains("ImportedPlaylists") || !memory["ImportedPlaylists"].is_array())
        memory["ImportedPlaylists"] = Json::array();
    if (!memory.contains("UserPrefer") || !memory["UserPrefer"].is_array())
        memory["UserPrefer"] = Json::array();
    if (!memory.contains("UserDisLike") || !memory["UserDisLike"].is_array())
        memory["UserDisLike"] = Json::array();
    return memory;
}

inline void saveMemory(const std::string& path, const Json& memory)
{
    std::ofstream output(path, std::ios::trunc);
    if (!output)
        throw std::runtime_error("无法写入记忆文件: " + path);
    output << memory.dump(4) << '\n';
}

inline std::string normalize(std::string value)
{
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), value.end());
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

inline Json::iterator findSong(Json& songs, const std::string& music, const std::string& artist)
{
    const std::string wantedMusic = normalize(music);
    const std::string wantedArtist = normalize(artist);
    return std::find_if(songs.begin(), songs.end(), [&](const Json& song) {
        return normalize(song.value("music", "")) == wantedMusic &&
               normalize(song.value("artist", "")) == wantedArtist;
    });
}
