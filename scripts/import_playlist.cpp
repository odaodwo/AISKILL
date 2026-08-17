#include <iostream>
#include "music_memory_common.h"

// 把已经整理好的歌单歌曲导入音乐记忆文件。
// 本程序不负责抓取网页，只负责读取 tracks.json、查重并保存。
int run(const std::vector<std::string>& args)
{
    // 参数依次为：程序名、记忆文件、歌曲列表、平台、链接、歌单名、日期。
    if (args.size() != 7)
    {
        std::cerr << "用法: import_playlist.exe <memory.json> <tracks.json> <平台> <歌单链接> <歌单名> <日期>\n";
        return 2;
    }

    try
    {
        // 先读取已有记忆，再打开临时整理出来的歌曲列表文件。
        Json memory = loadMemory(args[1]);
        std::ifstream tracksInput(args[2]);
        if (!tracksInput)
            throw std::runtime_error("无法打开歌曲列表: " + args[2]);

        Json tracks;
        tracksInput >> tracks;

        // 歌曲列表必须是 [{"music":"...", "artist":"..."}, ...] 形式的数组。
        if (!tracks.is_array())
            throw std::runtime_error("tracks.json 的最外层必须是数组");

        // added 统计新增歌曲，skipped 统计因重复而跳过的歌曲。
        int added = 0;
        int skipped = 0;
        Json& songs = memory["ALLFoundMusic"];

        // 逐首读取歌曲并检查必要字段。
        for (const Json& track : tracks)
        {
            const std::string music = track.value("music", "");
            const std::string artist = track.value("artist", "");
            if (music.empty() || artist.empty())
                throw std::runtime_error("每首歌曲都必须包含 music 和 artist");

            // 已存在相同“歌曲名 + 歌手名”时，不重复写入，也不覆盖旧点评。
            if (findSong(songs, music, artist) != songs.end())
            {
                ++skipped;
                continue;
            }

            // 用户主动提供的歌单歌曲默认视为 4.5 分，并记录来源平台和链接。
            songs.push_back({
                {"music", music},
                {"artist", artist},
                {"advantage", Json::array()},
                {"disadvantage", Json::array()},
                {"rating", 4.5},
                {"comment", "来自用户提供的音乐平台歌单，按规则使用默认评分"},
                {"listened", true},
                {"date", args[6]},
                {"source", {
                    {"type", "playlist_import"},
                    {"platform", args[3]},
                    {"playlist_url", args[4]}
                }}
            });
            ++added;
        }

        // 除了歌曲本身，还要记录这个歌单的导入信息。
        Json& playlists = memory["ImportedPlaylists"];

        // 用歌单链接判断这个歌单以前是否已经导入过。
        auto playlist = std::find_if(playlists.begin(), playlists.end(), [&](const Json& item) {
            return item.value("url", "") == args[4];
        });
        Json record = {
            {"platform", args[3]},
            {"url", args[4]},
            {"name", args[5]},
            {"imported_at", args[6]},
            {"track_count", added}
        };

        // 新歌单追加记录；旧歌单则累加本次真正新增的歌曲数量。
        if (playlist == playlists.end())
            playlists.push_back(record);
        else
        {
            record["track_count"] = playlist->value("track_count", 0) + added;
            *playlist = record;
        }

        // 所有检查完成后才统一写回记忆文件。
        saveMemory(args[1], memory);

        // 输出机器可读的执行结果，供 Skill 判断导入是否成功。
        std::cout << Json{{"added", added}, {"skipped", skipped}}.dump() << '\n';
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

// Windows 使用 wmain 保留中文命令行参数，其他系统使用 main。
#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) { return run(makeArgs(argc, argv)); }
#else
int main(int argc, char* argv[]) { return run(makeArgs(argc, argv)); }
#endif
