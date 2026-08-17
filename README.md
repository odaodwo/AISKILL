# AIMusicSkill

一个带长期偏好记忆的音乐推荐 Codex Skill。它可以根据公开歌单、歌曲列表、文字偏好和历史点评分析用户口味，并在后续推荐中避开重复歌曲、逐步调整推荐方向。

## 功能

- 分析公开音乐平台歌单或用户提供的歌曲列表
- 根据喜欢、不喜欢和历史评分生成相似歌曲推荐
- 使用本地 JSON 保存长期音乐偏好，不上传私人数据
- 导入歌单时自动查重
- 记录歌曲评分、优点、缺点和原始评论
- 支持酷狗、QQ 音乐、网易云音乐、Spotify、Apple Music、YouTube Music 等公开链接

## 项目结构

```text
AIMusicSkill/
├── SKILL.md                         # Skill 的触发条件与完整工作流
├── preferMemory.json                # 本地音乐偏好与历史记录
└── scripts/
    ├── music_memory_common.h        # JSON 读写、文本标准化和查重逻辑
    ├── find_duplicates.cpp          # 检查歌曲是否已经记录
    ├── import_playlist.cpp          # 导入整理后的歌单
    └── save_review.cpp              # 保存或更新用户点评
```

## 使用方式

将整个目录放入 Codex 的 Skills 目录中，并重新启动或刷新 Codex。之后可以直接提出类似请求：

```text
分析这个公开歌单，并推荐 5 首相似歌曲：<歌单链接>
我喜欢氛围感强、节奏舒缓的女声歌曲，帮我推荐一些。
记录我对《歌曲名》的评价：4.5 分，编曲很好，但副歌有点重复。
```

Skill 会在需要时读取 `preferMemory.json`，并调用 `scripts/` 中的工具完成查重和保存。

## 构建本地工具

仓库目前提供 C++ 源码，不包含预编译的 Windows 可执行文件。构建时需要：

- 支持 C++17 的编译器，例如 MinGW-w64 `g++`
- [nlohmann/json](https://github.com/nlohmann/json) 头文件

在项目根目录运行：

```powershell
g++ scripts/find_duplicates.cpp -std=c++17 -municode -o scripts/find_duplicates.exe
g++ scripts/save_review.cpp -std=c++17 -municode -o scripts/save_review.exe
g++ scripts/import_playlist.cpp -std=c++17 -municode -o scripts/import_playlist.exe
```

生成的 `.exe` 仅用于本机运行，不纳入源码版本控制。若面向不具备 C++ 环境的普通用户分发，建议在 GitHub Releases 中提供经过验证的预编译版本。

## 数据说明

`preferMemory.json` 默认只包含空数组：

```json
{
  "ALLFoundMusic": [],
  "ImportedPlaylists": [],
  "UserPrefer": [],
  "UserDisLike": []
}
```

使用过程中产生的歌单记录和个人偏好会写入此文件。公开分享仓库前，请检查其中是否包含不希望公开的个人数据。

## 开发提示

- 修改 C++ 源码后重新编译对应工具。
- 工具输出采用 JSON，调用方应同时检查输出内容和退出码。
- `scripts/` 只负责本地数据处理，不负责登录音乐平台或绕过访问限制。
- 提交代码时不要包含 `.exe`、临时歌单或本地测试文件。

