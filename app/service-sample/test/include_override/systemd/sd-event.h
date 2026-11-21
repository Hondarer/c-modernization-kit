#ifndef _OVERRIDE_SYSTEMD_SD_EVENT_H
#define _OVERRIDE_SYSTEMD_SD_EVENT_H

/*
 * libsystemd の <systemd/sd-event.h> を取り込み、service-sample が使う関数の呼び出しを mock_sd_* へ置き換える。
 * test/include_override は、テスト対象 (TEST_SRCS) のコンパイルだけに追加される。
 */
#ifndef _WIN32

    #include_next <systemd/sd-event.h>

    #define _IN_OVERRIDE_HEADER_SYSTEMD_SD_EVENT_H
    #include <mock_systemd.h>
    #undef _IN_OVERRIDE_HEADER_SYSTEMD_SD_EVENT_H

#endif // _WIN32

#endif // _OVERRIDE_SYSTEMD_SD_EVENT_H