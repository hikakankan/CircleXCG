
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

#ifdef USE_X68000
#include "mouse.h"
#endif

#ifndef USE_X68000
#include <conio.h>  // _kbhit, _getch
#endif

PictureBox picturebox; // PictureBoxのインスタンス

typedef struct {
    int x;
    int y;
    bool bl;
    bool br;
} MouseState;

#ifndef USE_X68000
void get_mouse_state(MouseState* state)
{
    POINT pt; // マウス座標を格納する構造体
    if (GetCursorPos(&pt)) {
        state->x = pt.x;
        state->y = pt.y;
    }
    else {
        state->x = -1;
        state->y = -1;
    }

    // マウスボタンの状態を取得
    state->bl = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    state->br = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
}
#else
// マウスの状態を取得する関数
void get_mouse_state(MouseState* state)
{
    int x, y, x2, y2, bl, br;

    mspos(&x, &y);
    msstat(&x2, &y2, &bl, &br);

    state->x = x;
    state->y = y;
    state->bl = bl != 0;
    state->br = br != 0;
}
#endif

#ifndef USE_X68000
bool window_or_mouse_init(HWND& hwnd) {
    // ウィンドウを初期化
    return true;
}
#else
typedef int HWND; // X68000ではウィンドウは不要なのでダミー定義
bool window_or_mouse_init(HWND& hwnd) {
    mouse_init();
    return true; // X68000ではウィンドウは不要なので常に成功
}
#endif

bool is_key_pressed() {
#ifndef USE_X68000
    return _kbhit();
#else
    int key = 0;
    keysns(&key);
    return (key & 0xff) != 0; // キーが押されているかどうかをチェック
#endif
}

int key_input() {
#ifndef USE_X68000
    if (_kbhit()) {
        return _getch(); // キー入力を取得
    }
    return 0; // 入力がない場合は0を返す
#else
    int key = 0;
    keyinp(&key); // IOCSコールでキー入力を取得
    return key & 0xff; // キーコードを返す
#endif
}

void run(ConOutput& co_cout) {
    co_cout << "Mouse Input Mode. Click inside the drawing area. Click out of click area to quit." << co_endl;

    MouseState state;

	HWND hwnd = hDrawWindow; // 描画ウィンドウのハンドルを使用
    if (!window_or_mouse_init(hwnd)) {
        // ウィンドウの初期化に失敗した場合は終了
        return;
    }

    while (true) {
#ifndef USE_X68000
        MSG msg;
        // メッセージがあれば処理
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
#endif

        get_mouse_state(&state);

        if (state.bl) {
            POINT pt;
			pt.x = state.x;
			pt.y = state.y;
#ifndef USE_X68000
            ScreenToClient(hwnd, &pt);
#endif
            if (!picturebox.MouseDown(hwnd, pt.x, pt.y)) {
				// クリック領域外なら終了
                return;
            }
        }

#ifndef USE_X68000
        // CPU負荷を減らすために少し待つ
        Sleep(50);
#endif

#ifdef USE_X68000
        if (is_key_pressed()) {
            int key = key_input();
            if (key == 'q') {
                // 'q'キーが押されたら終了
                break;
            }
        }
#endif
    }
}

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
        std::string result = std::to_string(picturebox.getarea());
        co_cout << "Area: " << result << co_endl;
    }
    else if (strcmp(result.command, "div") == 0 && token_count == 2) {
        picturebox.Divide(hDrawWindow, std::stoi(tokens[1]));
        std::string result = std::to_string(picturebox.getarea());
        co_cout << "Area: " << result << co_endl;
    }
    else if (strcmp(result.command, "run") == 0 && token_count == 1) {
        run(co_cout);
        co_cout << "Done Mouse Input Mode" << co_endl;
        //co_cout << co_endl;
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
