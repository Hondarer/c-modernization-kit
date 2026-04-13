#ifndef PORTER_TEST_HELPER_H
#define PORTER_TEST_HELPER_H

#include <com_util/base/platform.h>
#include <cstdio>
#include <string>
#include <vector>

#include <com_util/fs/path_max.h>

#if defined(PLATFORM_LINUX)
#include <unistd.h>
#endif /* PLATFORM_LINUX */

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
                                           const std::string& host = "127.0.0.1",
                                           const std::string& encrypt_key = "")
    {
        lines_.push_back("[service." + std::to_string(service_id) + "]");
        lines_.push_back("type      = unicast");
        lines_.push_back("src_addr1 = " + host);
        lines_.push_back("dst_addr1 = " + host);
        lines_.push_back("dst_port  = " + std::to_string(port));
        if (!encrypt_key.empty()) {
            lines_.push_back("encrypt_key = " + encrypt_key);
        }
        lines_.push_back("");
        return *this;
    }

    /** unicast_bidir サービスエントリを追加する。 */
    PorterConfigBuilder& addUnicastBidirService(int64_t service_id, int port,
                                                const std::string& host = "127.0.0.1",
                                                const std::string& encrypt_key = "")
    {
        lines_.push_back("[service." + std::to_string(service_id) + "]");
        lines_.push_back("type      = unicast_bidir");
        lines_.push_back("src_addr1 = " + host);
        lines_.push_back("dst_addr1 = " + host);
        lines_.push_back("dst_port  = " + std::to_string(port));
        if (!encrypt_key.empty()) {
            lines_.push_back("encrypt_key = " + encrypt_key);
        }
        lines_.push_back("");
        return *this;
    }

    /** unicast_bidir_n1 サービスエントリを追加する。 */
    PorterConfigBuilder& addUnicastBidirN1Service(int64_t service_id, int port,
                                                  int max_peers,
                                                  const std::string& bind_addr = "0.0.0.0",
                                                  const std::string& encrypt_key = "")
    {
        lines_.push_back("[service." + std::to_string(service_id) + "]");
        lines_.push_back("type      = unicast_bidir_n1");
        lines_.push_back("dst_addr1 = " + bind_addr);
        lines_.push_back("dst_port  = " + std::to_string(port));
        lines_.push_back("max_peers = " + std::to_string(max_peers));
        if (!encrypt_key.empty()) {
            lines_.push_back("encrypt_key = " + encrypt_key);
        }
        lines_.push_back("");
        return *this;
    }

    /** tcp_bidir サービスエントリを追加する。 */
    PorterConfigBuilder& addTcpBidirService(int64_t service_id, int port,
                                            const std::string& host = "127.0.0.1",
                                            const std::string& encrypt_key = "")
    {
        lines_.push_back("[service." + std::to_string(service_id) + "]");
        lines_.push_back("type      = tcp_bidir");
        lines_.push_back("dst_addr1 = " + host);
        lines_.push_back("dst_port  = " + std::to_string(port));
        if (!encrypt_key.empty()) {
            lines_.push_back("encrypt_key = " + encrypt_key);
        }
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
#if defined(PLATFORM_LINUX)
            char tmpl[] = "/tmp/porter_test_XXXXXX.conf";
            int fd = mkstemps(tmpl, 5); /* ".conf" = 5 文字 */
            if (fd == -1) { return ""; }
            close(fd);
            tmp_path_ = tmpl;
#elif defined(PLATFORM_WINDOWS)
            char tmp_dir[PLATFORM_PATH_MAX] = {};
            GetTempPathA(sizeof(tmp_dir), tmp_dir);
            char tmp_file[PLATFORM_PATH_MAX] = {};
            GetTempFileNameA(tmp_dir, "ptr", 0, tmp_file);
            /* .conf 拡張子に変更 */
            tmp_path_ = std::string(tmp_file) + ".conf";
            /* GetTempFileName が作成した元ファイルを削除して .conf で作り直す */
            DeleteFileA(tmp_file);
#endif /* PLATFORM_ */
        }

        FILE* f = nullptr;
#if defined(PLATFORM_LINUX)
        f = fopen(tmp_path_.c_str(), "w");
#elif defined(PLATFORM_WINDOWS)
        fopen_s(&f, tmp_path_.c_str(), "w");
#endif /* PLATFORM_ */
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
#if defined(PLATFORM_LINUX)
            unlink(tmp_path_.c_str());
#elif defined(PLATFORM_WINDOWS)
            DeleteFileA(tmp_path_.c_str());
#endif /* PLATFORM_ */
        }
    }

    /* コピー禁止 */
    PorterConfigBuilder(const PorterConfigBuilder&)            = delete;
    PorterConfigBuilder& operator=(const PorterConfigBuilder&) = delete;

private:
    std::vector<std::string> lines_;
    std::string              tmp_path_;
};

#endif /* PORTER_TEST_HELPER_H */
