/**
 *******************************************************************************
 *  @file           crypto.h
 *  @brief          データ暗号化・復号モジュールの内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/12
 *  @version        1.0.0
 *
 *  @details
 *  プラットフォームごとに異なる暗号 API を共通インターフェースで抽象化します。\n
 *
 *  | OS      | 使用ライブラリ                                           | アルゴリズム    |
 *  | ------- | -------------------------------------------------------- | --------------- |
 *  | Linux   | OpenSSL (libcrypto) EVP_aes_256_gcm() / EVP_sha256()     | AES-256-GCM / SHA-256 |
 *  | Windows | CNG (BCrypt) BCRYPT_AES_ALGORITHM / BCRYPT_SHA256_ALGORITHM | AES-256-GCM / SHA-256 |
 *
 *  暗号化ペイロードのフォーマット:
 *  @code
 *  [暗号文: src_len バイト] [GCM 認証タグ: POTR_CRYPTO_TAG_SIZE バイト]
 *  @endcode
 *
 *  ノンス構成 (12 バイト):
 *  @code
 *  [session_id: 4B NBO] [seq_num: 4B NBO] [padding: 4B 0x00]
 *  @endcode
 *
 *  追加認証データ (AAD): PotrPacket ヘッダー 32 バイト (NBO ワイヤーフォーマット)。
 *  ヘッダーの改ざんも認証タグで検知される。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#include <porter_const.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *******************************************************************************
 *  @brief          AES-256-GCM でデータを暗号化します。
 *  @param[out]     dst      暗号化後データを格納するバッファ。\n
 *                           内容: [暗号文: src_len バイト][GCM タグ: POTR_CRYPTO_TAG_SIZE バイト]\n
 *                           dst == src の in-place 暗号化も可能。
 *  @param[in,out]  dst_len  入力: dst のバッファサイズ (>= src_len + POTR_CRYPTO_TAG_SIZE)。\n
 *                           出力: 書き込んだバイト数 (= src_len + POTR_CRYPTO_TAG_SIZE)。
 *  @param[in]      src      平文データへのポインタ。
 *  @param[in]      src_len  平文データのバイト数。
 *  @param[in]      key      AES-256 鍵 (POTR_CRYPTO_KEY_SIZE バイト)。
 *  @param[in]      nonce    ノンス (POTR_CRYPTO_NONCE_SIZE バイト)。
 *  @param[in]      aad      追加認証データへのポインタ。NULL の場合は AAD なし。
 *  @param[in]      aad_len  AAD のバイト数。
 *  @return         成功時は 0、失敗時は -1 を返します。
 *******************************************************************************
 */
extern int potr_encrypt(uint8_t       *dst,
                        size_t        *dst_len,
                        const uint8_t *src,
                        size_t         src_len,
                        const uint8_t *key,
                        const uint8_t *nonce,
                        const uint8_t *aad,
                        size_t         aad_len);

/**
 *******************************************************************************
 *  @brief          AES-256-GCM でデータを復号し、認証タグを検証します。
 *  @param[out]     dst      復号後データを格納するバッファ。
 *  @param[in,out]  dst_len  入力: dst のバッファサイズ (>= src_len - POTR_CRYPTO_TAG_SIZE)。\n
 *                           出力: 書き込んだバイト数 (= src_len - POTR_CRYPTO_TAG_SIZE)。
 *  @param[in]      src      暗号化データへのポインタ。\n
 *                           内容: [暗号文: src_len - POTR_CRYPTO_TAG_SIZE バイト][GCM タグ: POTR_CRYPTO_TAG_SIZE バイト]
 *  @param[in]      src_len  暗号化データのバイト数 (タグを含む)。>= POTR_CRYPTO_TAG_SIZE。
 *  @param[in]      key      AES-256 鍵 (POTR_CRYPTO_KEY_SIZE バイト)。
 *  @param[in]      nonce    ノンス (POTR_CRYPTO_NONCE_SIZE バイト)。
 *  @param[in]      aad      追加認証データへのポインタ。NULL の場合は AAD なし。
 *  @param[in]      aad_len  AAD のバイト数。
 *  @return         成功 (認証タグ検証 OK) 時は 0、失敗 (認証タグ不一致含む) 時は -1 を返します。
 *******************************************************************************
 */
extern int potr_decrypt(uint8_t       *dst,
                        size_t        *dst_len,
                        const uint8_t *src,
                        size_t         src_len,
                        const uint8_t *key,
                        const uint8_t *nonce,
                        const uint8_t *aad,
                        size_t         aad_len);

/**
 *******************************************************************************
 *  @brief          任意のパスフレーズを SHA-256 ハッシュにより AES-256 鍵に変換します。
 *  @details
 *  設定ファイルの `encrypt_key` に 64 文字 hex 以外の文字列が指定された場合に使用します。\n
 *  入力バイト列の SHA-256 ダイジェストをそのまま鍵として使用します。\n
 *  送受信の双方で同一のパスフレーズを指定すれば、同一の鍵が導出されます。
 *  @param[out]     key             出力鍵バッファ (POTR_CRYPTO_KEY_SIZE = 32 バイト)。
 *  @param[in]      passphrase      パスフレーズへのポインタ (任意のバイト列)。
 *  @param[in]      passphrase_len  パスフレーズの長さ (バイト)。0 の場合も有効。
 *  @return         成功時は 0、失敗時は -1 を返します。
 *******************************************************************************
 */
extern int potr_passphrase_to_key(uint8_t       *key,
                                  const uint8_t *passphrase,
                                  size_t         passphrase_len);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_H */
