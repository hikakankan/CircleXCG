#include "CommandInterpreter.h"
#include "CircleXCG.h"

bool CommandInterpreter::process_command(const std::string& sline, ConOutput& co_cout, ConOutput& co_cerr) {
    char line[256]; // ���̓o�b�t�@�i�K�؂ȃT�C�Y���m�ہj
    std::string tsline = DNString(sline).Trim();
#ifdef USE_X68000
    strncpy(line, tsline.c_str(), sizeof(line));
#else
    strcpy_s(line, sizeof(line), tsline.c_str());
#endif
    line[strcspn(line, "\n")] = '\0'; // ���s�������폜
    char* tokens[MAX_TOKENS];
    int token_count = split(line, ' ', tokens, MAX_TOKENS);
    if (token_count == 0) {
        return true; // ��s�̏ꍇ�͏����𑱂���
    }
    return exec_command(token_count, tokens, co_cout, co_cerr);
}
