# OurScheme (PAL) — 精簡 Scheme 直譯器

> 本專案實作一個精簡版 **Scheme 直譯器 OurScheme**，分為四個階段（Project 1–4）。
> 最終需同時支援**互動模式**與 **PAL** 測試器的批次 **檔案 I/O** 評測。
> 本文件聚焦於各階段的 **目標 / 方法 / 輸入 / 輸出** 與錯誤處理重點。

---

## 目錄
- [建置與執行](#建置與執行)
- [I/O 與語法總覽](#io-與語法總覽ourscheme-子集合)
- [Project 1- Scanner / Parser / Pretty Printer](#project-1--scanner--parser--pretty-printer)
- [Project 2：求值器與基本語義](#project-2--求值器與基本語義)
- [Project 3：let / lambda / define 與使用者函式](#project-3--let--lambda--define具名函式)
- [Project 4：I/O、錯誤物件、eval、set!](#project-4--io錯誤物件evalset)
- [錯誤處理總覽](#錯誤處理總覽實作指引)
- [測試建議](#測試建議)

---

## 建置與執行

> C++

## I/O 與語法總覽（OurScheme 子集合）

- **Token 與分隔**
  - 分隔字元：空白、`(`、`)`、`'`、`"`、`;`
  - 註解：`;` 至該行結尾（scanner 忽略）
  - 字串：不跨行；支援跳脫 `\" \\ \n \t`
- **原子型別**
  - `INT`、`FLOAT`（輸出固定三位小數，如 `3.000`）、`STRING`、`SYMBOL`
  - 布林/空表：`#t`、`#f`、`nil/()`（系統輸出用 `#t`、`nil`）
- **結構**
  - **List / Dotted Pair**：`(a b)`、`(a . b)`
  - **Quote**：`'x` 等價 `(quote x)`

---

## Project 1 — Scanner / Parser / Pretty Printer

### 目標
- 將字元流 token 化、依文法建立 AST，並以 **Pretty Printer** 輸出「最精簡合法表示」。

### 方法（建議 C++ 模組）
- `Scanner`：最長優先；處理 `.`（小數點 vs DOT）、`#t/#f`、字串跳脫、`;` 註解。
- `Parser`：遞迴下降；支援 list、dotted pair、`quote` 語法糖。
- `Printer`：浮點三位、`#t`、`nil`；`(1 . (2 . nil))` 應印成 `( 1 2 )`。

### 輸入 / 輸出
- **輸入**：互動或檔案中若干 S-exp
- **輸出**：成功 parse 的 S-exp 之 Pretty Print；語法錯誤需含**行/列/字元**資訊，並忽略該行餘下內容

---

## Project 2 — 求值器與基本語義

### 目標
- 在 P1 基礎上加入 **EvalSExp**，支援核心原語、條件、序列、全域 `define` 等。

### 方法（重點）
- **環境（Environment）**：符號 → 值/程序 繫結（可使用 `unordered_map<string, Value>`；日後支援巢狀）
- **原語（節錄）**
  - 建構/列表：`cons`(2), `list`(>=0)；取部：`car`(1), `cdr`(1)
  - 引用：`quote`(1)、`'`
  - 述詞（皆 1 實參）：`atom? pair? list? null? integer? real? number? string? boolean? symbol?`
  - 算術/比較/邏輯：`+ - * /`（各 >=2）、`> >= < <= =`、`not`(1), `and`(>=2), `or`(>=2)
  - 字串：`string-append`、`string>? string<? string=?`（多參數需**全成立**）
  - 等價：`eqv?`(2), `equal?`(2)
  - 序列：`begin`(>=1)（回傳最後一個子表達式）
  - 條件：`if`(2/3), `cond`(>=1)（**短路求值**）
  - 綁定：`define`(2)（**top-level**；不可覆寫系統原語）
  - 清空：`clean-environment`(0)（清除使用者繫結）
- **求值規則**
  - 一般情況**先求值實參**；特殊形式（`quote/if/and/or/cond/define/begin`）按語意僅求需要部分

### 輸入 / 輸出
- **輸入**：表達式
- **輸出**：值（依 Printer 規則）或對應錯誤訊息

---

## Project 3 — `let` / `lambda` / `define`（具名函式）

### 目標
- 支援 **匿名函式**、**區域綁定**、**具名函式**；處理**保留字**在表頭的形式檢查。

### 方法（重點）
- **let**
  - `(let ( (x e1) (y e2) ... ) body1 body2 ... )`
  - 第一參數必為配對清單；至少一個 body；回傳**最後一個 body** 的值
- **lambda**
  - `(lambda (args...) body1 body2 ...)`，`args` 為 0..n 個**符號**
  - 求值結果為**程序物件**（可印 `#<procedure lambda>`）
- **define（具名程序）**
  - `(define (f x y ...) body...)`（**僅 top-level**），或 `(define SYMBOL expr)`
- **「無回傳值」錯誤**
  - 在**會被實際求值**的位置（函式實參、`if/cond/and/or` 被用到的分支/條件）若子式不產生值 → 錯誤  
  - 在**不被取用**的語境（`begin` 前項、未被選中的分支）則不報錯

### 輸入 / 輸出
- **輸入**：含 `let/lambda/define` 的表達式與呼叫
- **輸出**：求值後的值或格式/語義錯誤訊息

---

## Project 4 — I/O、錯誤物件、`eval`、`set!`

### 目標
- 加入 **錯誤物件（ERROR）** 與 I/O/轉換、`eval`、`set!` 等。

### 方法（重點）
- **錯誤物件**
  - `create-error-object`(1)：由**字串**建立錯誤物件
  - `error-object?`(1)：述詞
  - `read`：讀下一個 S-exp；遇掃描/語法錯誤回傳**錯誤物件**（而非直接中止）
  - EOF 時 `read` 應回傳帶有固定訊息的錯誤物件
- **輸出/轉換**
  - `write`(1)：以 Printer 風格印參數，並**回傳參數**
  - `display-string`(1)：只接受 **string/error-object**，不含引號輸出，並回傳參數
  - `newline`(0)：輸出換行並回傳 `nil`
  - `symbol->string`(1), `number->string`(1)：轉字串（數字仍需三位小數規則）
- **求值/指派**
  - `eval`(1)：對其值再 eval 一次（等同呼叫 EvalSExpression）
  - `set!`(2)：`(set! sym expr)`；`sym` **不求值**、`expr` 求值；回傳被設定之值  
    `set!` 可用於非 top-level；`define` 僅允許 top-level

### 輸入 / 輸出
- **輸入**：含 I/O、錯誤物件、`eval`、`set!` 的表達式
- **輸出**：值或錯誤物件/錯誤訊息（依規格）

---

## 錯誤處理總覽（實作指引）

- **語法錯誤（P1 起）**：需包含**行/列/問題 token（或字元）**；錯誤後**忽略整行**，從下一行續讀
- **求值錯誤（P2 起）**：未繫結符號、參數個數/型別錯誤、套用非函式、除以零等 → 依規格字串輸出
- **無回傳值（P3/4）**：只在**會被實際使用到**的位置判為錯誤
- **錯誤物件（P4）**：`create-error-object`/`read` 產生；`error-object?` 判斷；`display-string` 僅收 string/error-object

---

## 測試建議

- **互動 REPL**
  - 多行/同一行多式、`'` 與 `(quote ...)`、字串跳蠅、`;` 註解、`(exit)` 與 EOF 行為
- **批次（PAL）**
  - `./ourscheme < in > out`：重現教材範例與自製測資
  - 覆蓋面：
    - **Scanner**：`.`/`#` 多義、字串跳脫、註解、空白、分隔
    - **Printer**：浮點三位、`#t`/`nil`、list 與 dotted pair 邊界
    - **P2**：短路求值、字串比較多參數、`define` 限制、數學/比較邊界（0、負、浮點）
    - **P3**：`let` 格式、`lambda` 參數表、具名 `define` 僅 top-level、無回傳值規則
    - **P4**：`read/write/display-string/newline`、`symbol->string/number->string`、`eval`、`set!`、錯誤物件傳遞
- **效能/健壯性**
  - 深巢狀 list、長符號/字串、空表、極端數值、混合空白與註解

---
