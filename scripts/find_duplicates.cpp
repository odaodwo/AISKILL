#include <iostream>
#include "music_memory_common.h"

// 程序的主要逻辑放在 run() 中。
// args 保存已经转换成 UTF-8 的命令行参数，便于正确处理中文歌名和歌手名。
int run(const std::vector<std::string>& args)
{
    // 参数依次为：程序名、记忆文件、歌曲名、歌手名，因此必须正好有 4 个。
    if (args.size() != 4)
    {
        std::cerr << "用法: find_duplicates.exe <memory.json> <歌曲名> <歌手名>\n";
        return 2;
    }

    try
    {
        // 读取整个音乐记忆文件，再取得其中保存歌曲的数组。
        Json memory = loadMemory(args[1]);
        Json& songs = memory["ALLFoundMusic"];

        // findSong() 返回歌曲位置；如果等于 end()，表示没有找到。
        const bool exists = findSong(songs, args[2], args[3]) != songs.end();

        // 使用 JSON 输出，方便 Skill 或其他程序稳定地读取结果。
        std::cout << Json{{"exists", exists}}.dump() << '\n';
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

// Windows 使用 wmain 接收宽字符参数，防止中文经过命令行时乱码。
// 其他系统使用普通 main；两者最终都会调用同一个 run()。
#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) { return run(makeArgs(argc, argv)); }
#else
int main(int argc, char* argv[]) { return run(makeArgs(argc, argv)); }
#endif
