
//#define USE_X68000
#ifdef USE_X68000
#define CONSOLE_IO // コンソール入出力を使用
#endif

#include <sstream>
#include <string>
#include <iostream>
#include <map>

#ifndef USE_X68000
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#endif

#include "console.h"
#ifndef CONSOLE_IO
#include "form.h"
#endif
#include "CommandInterpreter.h"
#include "PictureBox.h"

PictureBox picturebox; // PictureBoxのインスタンス

void paint(HWND hwnd) {
	picturebox.Paint(hwnd);
}

bool exec_command(int token_count, char** tokens, ConOutput& co_cout, ConOutput& co_cerr) {
    const char* commands[] = {
        "exit", "add", "div", "run"
    };
    const int command_count = sizeof(commands) / sizeof(commands[0]);
    CommandMatchResult result = matchCommand(tokens[0], commands, command_count);

    if (!result.matched) {
        co_cerr << "Unknown command: " << tokens[0] << co_endl;
        return true; // コマンドが不明な場合は処理を続ける
    }
    if (strcmp(result.command, "exit") == 0) {
        return false;
    }
    if (strcmp(result.command, "add") == 0 && token_count == 2) {
        picturebox.AddByDeg(hDrawWindow, std::stoi(tokens[1]));
        //picturebox.print(co_cout);
        std::string result = std::to_string(picturebox.getarea());
        co_cout << "Result: " << result << co_endl;
    }
    else if (strcmp(result.command, "div") == 0 && token_count == 2) {
        //picturebox.Divide(hDrawWindow, std::stoi(tokens[1]), co_cout);
        picturebox.Divide(hDrawWindow, std::stoi(tokens[1]));
        //picturebox.print(co_cout);
        std::string result = std::to_string(picturebox.getarea());
        co_cout << "Result: " << result << co_endl;
    }
    else if (strcmp(result.command, "run") == 0 && token_count == 1) {
    }
    return true;
}

CommandInterpreter command_interpreter;

#ifdef CONSOLE_IO
bool process_line(char* cline) {
    ConOutput co_cout(hOutput); // 出力オブジェクト
    ConOutput co_cerr(hOutput); // エラー出力オブジェクト

    std::string line(cline);
    return command_interpreter.process_command(line, co_cout, co_cerr);
}
#else
bool process_line(const std::wstring& wline) {
    ConOutput co_cout(hOutput); // 出力オブジェクト
    ConOutput co_cerr(hOutput); // エラー出力オブジェクト

    std::string line = WstringToString(wline);
    return command_interpreter.process_command(line, co_cout, co_cerr);
}
#endif

#ifdef CONSOLE_IO
void prompt() {
    // プロンプトを表示
    printf("> ");
}

#ifdef USE_X68000
void xdraw_init() {
    // X68000用の初期化コード
    crtmod(4);  // 画面モードを設定
    gclear();    // 画面をクリア
    apage(0); // グラフィックページを設定
	picturebox.Paint(hDrawWindow); // 初期描画
}
#endif

// メイン関数
int main() {
#ifdef USE_X68000
	xdraw_init();
#endif
    prompt();
    char line[256]; // 入力バッファ（適切なサイズを確保）
    while (fgets(line, sizeof(line), stdin)) {
        if (!process_line(line)) {
            break;
        }
        prompt();
    }
    return 0;
}
#endif
