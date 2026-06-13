# ADVcchi — コンセプトメモ

M5Stack Cardputer ADV で動く開発者たまごっち。Claude Code の hook 経由で開発行為がエサになる。
「物理的に存在する罪悪感」が差別化の核。

## 当面のスコープ（2026-06-13 壁打ちで縮小）

フルのたまごっち（HP・死亡・進化）はいきなり作らない。まず:

1. **ADV 上で動く Claude Code のカニみたいなキャラ**
   - 各物理ボタン（キーボード）でリアクション & 音
2. **Claude Code を繋いだら bash 等のアクションでリアクションする**
   - PostToolUse hook → curl → Cardputer HTTP サーバ

ここまでが第一弾。HP/死亡/機嫌/進化は後のフェーズ。

## 壁打ちで確定した設計ポイント

- **hook は stdin JSON 方式**（`$1 $2` 引数では来ない）。`tool_name` / `tool_input.command` / `tool_response` を jq で読む
- **イベント分類は Mac 側 shell でやる**。ファームウェアは受け取った event を表示するだけ（変更コストの非対称性）
- **curl は `--max-time 2` + バックグラウンド必須**。デバイス不在時に開発体験を壊さない
- **mDNS（`advcchi.local`）** で IP ハードコード回避
- **AsyncWebServer 不要**。M5Unified ループ + 標準 WebServer の `handleClient()` で足りる見込み
- **test pass/fail 判定は後回し**（tool_response の解析が必要なため）
- **hook 置き場は `~/.claude/settings.json`**（ユーザーレベル、全プロジェクトの行為がエサ）

## 将来フェーズ（フルたまごっち化）の論点

- HP 減衰「1分 -1」は進化（数日生存前提）と矛盾 → **睡眠モード案（最終イベント60分で減衰停止）を採用方向**
- お経は SD カードの WAV 再生が現実的
- LAN 内認証なし HTTP → 固定トークン 1 個で十分
- yotta 環境固有ネタ: omamori ブロックで「怒られた…」、compaction で記憶喪失演出

## ハードウェア

- M5Stack Cardputer ADV（接続済み）
- 画面 240×135、物理キーボード、内蔵スピーカー、SD カード
- Arduino / C++（M5Unified / M5GFX）
