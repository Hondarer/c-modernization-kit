#ifndef _PORTER_TEST_HELPER_H
#define _PORTER_TEST_HELPER_H

#include <cstdio>
#include <string>
#include <vector>

#include <util/fs/path_max.h>

#ifndef _WIN32
#include <unistd.h>
#endif

/**
 * テスト用 porter サービス定義を一時ファイルに書き出すビルダー。
 * デストラクタで一時ファイルを削除する。
 *
 * 使用例:
 *   PorterConfigBuilder cfg;
 *   std::string config_path = cfg.addUnicastService(10, 19010).build();
 */
class PorterConfigBuilder {
public:
    PorterConfigBuilder() = default;

    /** unicast サービスエントリを追加する。 */
    PorterConfigBuilder& addUnicastService(int64_t service_id, int port,
                                           const std::string& host = "127.0.0.1")
    {
        lines_.push_back("[service." + std::to_string(service_id) + "]");
        lines_.push_back("type      = unicast");
        lines_.push_back("src_addr1 = " + host);
        lines_.push_back("dst_addr1 = " + host);
        lines_.push_back("dst_port  = " + std::to_string(port));
        lines_.push_back("");
        return *this;
    }

    /** unicast_bidir サービスエントリを追加する。 */
    PorterConfigBuilder& addUnicastBidirService(int64_t service_id, int port,
                                                const std::string& host = "127.0.0.1")
    {
        lines_.push_back("[service." + std::to_string(service_id) + "]");
        lines_.push_back("type      = unicast_bidir");
        lines_.push_back("src_addr1 = " + host);
        lines_.push_back("dst_addr1 = " + host);
        lines_.push_back("dst_port  = " + std::to_string(port));
        lines_.push_back("");
        return *this;
    }

    /**
     * 一時ファイルを書き出し、そのパスを返す。
     * 2 回目以降の呼び出しでは既存ファイルを上書きする。
     */
    std::string build()
    {
        if (tmp_path_.empty()) {
#ifndef _WIN32
            char tmpl[] = "/tmp/porter_test_XXXXXX.conf";
            int fd = mkstemps(tmpl, 5); /* ".conf" = 5 文字 */
            if (fd == -1) { return ""; }
            close(fd);
            tmp_path_ = tmpl;
#else
            char tmp_dir[PLATFORM_PATH_MAX] = {};
            GetTempPathA(sizeof(tmp_dir), tmp_dir);
            char tmp_file[PLATFORM_PATH_MAX] = {};
            GetTempFileNameA(tmp_dir, "ptr", 0, tmp_file);
            /* .conf 拡張子に変更 */
            tmp_path_ = std::string(tmp_file) + ".conf";
            /* GetTempFileName が作成した元ファイルを削除して .conf で作り直す */
            DeleteFileA(tmp_file);
#endif
        }

        FILE* f = nullptr;
#ifndef _WIN32
        f = fopen(tmp_path_.c_str(), "w");
#else
        fopen_s(&f, tmp_path_.c_str(), "w");
#endif
        if (f == nullptr) { return ""; }

        /* global セクション (テスト向けに短いタイムアウト) */
        fprintf(f, "[global]\n");
        fprintf(f, "window_size        = 16\n");
        fprintf(f, "max_payload        = 1400\n");
        fprintf(f, "health_interval_ms = 1000\n");
        fprintf(f, "health_timeout_ms  = 3000\n");
        fprintf(f, "\n");

        for (const auto& line : lines_) {
            fprintf(f, "%s\n", line.c_str());
        }
        fclose(f);

        return tmp_path_;
    }

    ~PorterConfigBuilder()
    {
        if (!tmp_path_.empty()) {
#ifndef _WIN32
            unlink(tmp_path_.c_str());
#else
            DeleteFileA(tmp_path_.c_str());
#endif
        }
    }

    /* コピー禁止 */
    PorterConfigBuilder(const PorterConfigBuilder&)            = delete;
    PorterConfigBuilder& operator=(const PorterConfigBuilder&) = delete;

private:
    std::vector<std::string> lines_;
    std::string              tmp_path_;
};

#endif // _PORTER_TEST_HELPER_H
