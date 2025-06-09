# Kagou - Lightweight HTTP Server

## プロジェクト概要

Kagouは、C言語で実装された軽量なHTTPサーバーです。静的ファイルの配信機能を提供し、様々なMIMEタイプに対応しています。

## 主な機能

- HTTP/1.1プロトコルのサポート
- HTTPS/TLS 1.2+のサポート（--httpsオプション）
- 静的ファイルの配信（HTML、CSS、JavaScript、画像ファイルなど）
- 複数のMIMEタイプのサポート
- エラーハンドリング（404 Not Found、415 Unsupported Media Type、500 Internal Server Error）
- AddressSanitizerによるメモリ安全性の確保

## ディレクトリ構造

```
kagou/
├── src/                    # ソースコード
│   ├── main.c             # メインエントリーポイント
│   ├── request.c/h        # HTTPリクエスト処理
│   ├── request_handler.c/h # リクエストハンドラー
│   ├── response.c/h       # HTTPレスポンス処理
│   └── util.c/h           # ユーティリティ関数
├── test/                  # テスト用ファイル
│   ├── test.html
│   ├── test.css
│   ├── test.js
│   ├── test.json
│   └── test.csv
├── tests/                 # ユニットテスト
│   └── test.c
├── e2e/                   # E2Eテスト
│   └── server.spec.js
├── Makefile              # ビルド設定
├── package.json          # Node.js依存関係（E2Eテスト用）
├── playwright.config.js  # Playwright設定
└── README.md             # プロジェクトREADME

```

## ビルド方法

```bash
# ビルド
make

# クリーンビルド
make clean && make

# テストの実行
make test

# E2Eテストの実行
npm test
```

## 実行方法

```bash
# デフォルトポート（8080）で起動
./kagou

# カスタムポートで起動
./kagou 3000

# 特定のディレクトリをドキュメントルートとして起動
./kagou 8080 /path/to/document/root

# HTTPSを有効にして起動（TLS 1.2+）
./kagou 8443 /path/to/document/root --https
```

## 開発環境

- C言語（C99以降）
- GCC（AddressSanitizerサポート必須）
- Make
- Node.js（E2Eテスト用）

## コーディング規約

- 関数名は`snake_case`を使用
- 構造体名は`PascalCase`を使用
- マクロ定数は`UPPER_SNAKE_CASE`を使用
- インデントは2スペース
- エラーハンドリングは必ず実装する

## 最近の変更

- TLSサポートをTLS 1.0からTLS 1.2以上に更新し、セキュリティを向上
- ヘッダーファイルの`static`変数を`extern`に変更し、リンケージの問題を解決
- `fread`と`realpath`の戻り値チェックを追加し、エラーハンドリングを改善
- メモリリークの防止とエラー時の適切なクリーンアップを実装
