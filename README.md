# Bash_in_c

一個用 C 語言實作的輕量級 Shell，支援基本指令執行
## 編譯方法

```bash
gcc -Wall -Wextra -o main main.c -lreadline
```

## 現有功能

- ✅ 基本 shell 功能（fork + execvp）
- ✅ Readline 支援（命令歷史、行編輯）
- ✅ 引號處理 - 支援雙引號包裹參數（如 `grep "^d"`、`echo "Hello World"`）
- ✅ 內建指令：
  - `cd` - 目錄切換（支援 `cd`, `cd ~`, `cd -`, `cd <path>`）
  - `help` - 顯示完整指令說明
  - `exit` - 退出 shell
- ✅ ASCII 藝術指令集：
  - `ascii-table` - ASCII 字元表
  - `ascii-box` - 文字方框
  - `progress` - 進度條動畫
  - `spinner` - 旋轉動畫
  - `matrix` - 矩陣雨效果
  - `clock` - ASCII 藝術時鐘（即時更新顯示）
- ✅ **管道 (Pipe) 支援** - 實作 `|` 運算子，支援 `ls | grep txt`、`ps aux | grep bash` 等多重管道命令

## 未來功能規劃

### 核心功能擴展
- [x] **內建 cd 指令** - 實作目錄切換功能 ✅ 已完成
- [x] **管道 (Pipe) 支援** - 實作 `|` 運算子，支援 `ls | grep txt` ✅ 已完成
- [ ] **重導向 (Redirection)** - 實作 `>`, `>>`, `<` 運算子
- [ ] **背景執行** - 支援 `&` 讓程序在背景執行
- [ ] **環境變數** - 支援 `export`、`$VAR` 變數展開
- [ ] **訊號處理** - 正確處理 Ctrl+C (SIGINT)、Ctrl+Z (SIGTSTP)
- [ ] **工作控制** - 支援 `jobs`、`fg`、`bg` 指令

### 內建指令擴充
- [x] **help 指令** - 更完整的內建指令說明 ✅ 已完成
- [ ] **檔案操作** - `cat`、`touch`、`rm` 等簡化版內建實作
- [ ] **系統資訊** - `sysinfo` 顯示系統資訊（CPU、記憶體使用率）
- [ ] **歷史指令** - `history` 指令查看命令歷史
- [ ] **別名功能** - `alias` 和 `unalias` 指令

### 互動體驗改善
- [ ] **自訂提示符** - 支援顯示當前目錄、使用者名稱、主機名
- [ ] **自動補全** - Tab 鍵自動補全檔案名稱和指令
- [ ] **語法高亮** - 指令輸入時的即時語法標示
- [ ] **提示建議** - 類似 fish shell 的指令建議
- [ ] **配置檔** - 支援 `.bashrc` 或 `.dashrc` 設定檔

### 進階功能
- [ ] **腳本執行** - 支援執行 shell 腳本檔案
- [ ] **條件判斷** - 實作 `if`、`then`、`else`、`fi`
- [ ] **迴圈結構** - 實作 `for`、`while` 迴圈
- [ ] **函數定義** - 支援自訂 shell 函數
- [ ] **命令替換** - 支援 `$(command)` 或 `` `command` ``
- [ ] **萬用字元展開** - 支援 `*`、`?`、`[...]` 等 glob 模式

### 趣味功能
- [x] **時鐘顯示** - ASCII 藝術時鐘 ✅ 已完成
- [ ] **天氣查詢** - `weather [city]` 顯示天氣資訊
- [ ] **計算機** - 簡易計算功能
- [ ] **顏色主題** - 可切換的配色方案
- [ ] **遊戲模式** - 內建簡單的文字遊戲（如貪食蛇、猜數字）

### 效能與品質
- [ ] **記憶體洩漏檢查** - 使用 Valgrind 確保無記憶體洩漏
- [ ] **錯誤處理強化** - 更完善的錯誤訊息和異常處理
- [ ] **單元測試** - 為核心功能添加測試
- [ ] **效能優化** - 減少不必要的記憶體分配
- [ ] **跨平台支援** - 支援 Windows (使用 MinGW)

## 參考資料
- https://github.com/danishprakash/dash/tree/master  
- https://danishpraka.sh/posts/write-a-shell/

