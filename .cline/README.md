# Cline Development Guide for Kagou

このドキュメントは、ClineでKagouプロジェクトを効率的に開発するためのガイドです。

## プロジェクト概要

Kagouは軽量なHTTPサーバーで、C言語で実装されています。詳細は`PROJECT.md`を参照してください。

## 重要なファイル

### ドキュメント
- `PROJECT.md` - プロジェクトの概要と構造
- `TODO.md` - タスクリストと既知の問題
- `README.md` - ユーザー向けドキュメント
- `.cline/README.md` - このファイル（開発者向けガイド）

### ソースコード
- `src/main.c` - エントリーポイント
- `src/request.c/h` - HTTPリクエスト処理
- `src/request_handler.c/h` - リクエストハンドリングロジック
- `src/response.c/h` - HTTPレスポンス生成
- `src/util.c/h` - ユーティリティ関数

### ビルド・テスト
- `Makefile` - ビルド設定
- `tests/test.c` - ユニットテスト
- `e2e/server.spec.js` - E2Eテスト

## 開発フロー

### 1. ビルド
```bash
make clean && make
```

### 2. テスト実行
```bash
# ユニットテスト
make test

# E2Eテスト
yarn e2e
```

### 3. サーバー起動
```bash
./kagou [port] [document_root]
```

### 4. Format

```bash
make format
```

## コーディング規約

### 命名規則
- **関数**: `snake_case` (例: `load_text_file`)
- **構造体**: `PascalCase` (例: `Request`, `Response`)
- **マクロ/定数**: `UPPER_SNAKE_CASE` (例: `HEADER_BUFFER`)
- **変数**: `snake_case` (例: `file_name`)

### スタイル
- インデント: 2スペース
- 行の最大長: 80文字（推奨）
- 中括弧: K&Rスタイル

### メモリ管理
- すべての`malloc`/`calloc`に対応する`free`を必ず実装
- エラー時のクリーンアップを忘れない
- AddressSanitizerでメモリリークをチェック

### エラーハンドリング
- システムコールの戻り値は必ずチェック
- エラー時は適切なエラーメッセージを出力
- リソースのクリーンアップを確実に実行

## よくある作業

### 新しいMIMEタイプの追加
`src/request_handler.c`の`MIME_TYPES`配列に追加：
```c
{".webp", "image/webp", FILE_BINARY},
```

### エラーページのカスタマイズ
`src/request_handler.c`の定数を編集：
- `NOT_FOUND_BODY`
- `UNSUPPORTED_MEDIA`
- `BAD_REQUEST`

### デバッグ
AddressSanitizerが有効なので、メモリ関連のバグは実行時に検出されます：
```bash
./kagou [port] [document_root]
# メモリエラーがあれば詳細なスタックトレースが表示される
```

## 注意事項

1. **ヘッダーファイルの変数定義**
   - `static`変数をヘッダーに定義しない
   - グローバル変数は`extern`宣言 + .cファイルで定義

2. **戻り値チェック**
   - `fread`, `fwrite`, `realpath`などの戻り値は必ずチェック

3. **メモリ確保**
   - 大きなファイルを扱う際はメモリ使用量に注意
   - 現在はファイル全体をメモリに読み込む実装

4. **セキュリティ**
   - パストラバーサル攻撃への対策として`realpath`を使用
   - バッファオーバーフローに注意

## トラブルシューティング

### コンパイルエラー
- GCCのバージョンを確認（AddressSanitizerサポート必須）
- ヘッダーファイルのインクルード順序を確認

### 実行時エラー
- AddressSanitizerの出力を確認
- `strace`でシステムコールをトレース
- `gdb`でデバッグ

### テスト失敗
- ポート8080が使用されていないか確認
- Node.jsの依存関係をインストール: `npm install`

## 今後の開発方針

`TODO.md`を参照してください。主な優先事項：
- HTTPSサポート
- Keep-Alive接続
- メモリ使用量の最適化
- より詳細なログ機能
