#pragma once

//#include "resource.h"

#include "console.h"
void paint(HWND hwnd);
bool exec_command(int token_count, char** tokens, ConOutput& co_cout, ConOutput& co_cerr);
