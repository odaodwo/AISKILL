#include <iostream>
#include "music_memory_common.h"

// 保存用户点评。args 中的所有文本都已经统一为 UTF-8。
int run(const std::vector<std::string>& args)
{
    // 前 7 个参数是必需的，优点和缺点是可选的，所以最多有 9 个参数。
    if (args.size() < 7 || args.size() > 9)
    {
        std::cerr << "用法: save_review.exe <memory.json> <歌曲名> <歌手名> <1-5评分> <评论> <日期> [优点] [缺点]\n";
        return 2;
    }

    try
    {
        // 命令行参数本来都是字符串，std::stod() 将评分转换为 double。
        const double rating = std::stod(args[4]);
        if (rating < 1.0 || rating > 5.0)
            throw std::runtime_error("评分必须在 1 到 5 之间");

        // 读取记忆，并用引用直接操作 ALLFoundMusic 数组。
        Json memory = loadMemory(args[1]);
        Json& songs = memory["ALLFoundMusic"];

        // 生成一条完整的用户点评记录。
        // 没有填写优点或缺点时，对应字段保存为空数组，而不是空字符串。
        Json review = {
            {"music", args[2]},
            {"artist", args[3]},
            {"advantage", args.size() >= 8 && !args[7].empty() ? Json::array({args[7]}) : Json::array()},
            {"disadvantage", args.size() >= 9 && !args[8].empty() ? Json::array({args[8]}) : Json::array()},
            {"rating", rating},
            {"comment", args[5]},
            {"listened", true},
            {"date", args[6]},
            {"source", {{"type", "user_review"}}}
        };

        // 使用“歌曲名 + 歌手名”查找旧记录。
        // 如果歌曲来自之前导入的歌单，就用用户的明确点评覆盖默认 4.5 分记录；
        // 如果歌曲从未记录过，则把新点评追加到数组末尾。
        auto existing = findSong(songs, args[2], args[3]);
        const bool updated = existing != songs.end();
        if (updated)
            *existing = review;
        else
            songs.push_back(review);

        // 将修改后的完整 JSON 写回原文件。
        saveMemory(args[1], memory);

        // updated=true 表示覆盖了已有歌曲，false 表示新增了一首歌曲。
        std::cout << Json{{"saved", true}, {"updated", updated}}.dump() << '\n';
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

// Windows 用宽字符入口接收中文参数，再由 makeArgs() 转成 UTF-8。
#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) { return run(makeArgs(argc, argv)); }
#else
int main(int argc, char* argv[]) { return run(makeArgs(argc, argv)); }
#endif
