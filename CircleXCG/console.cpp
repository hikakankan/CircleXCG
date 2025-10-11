#include "console.h"
//#include "form.h"
//#include "AsmInterpreter.h"
//#include "Program.h"
//#include "CommandInterpreter.h"
#ifdef USE_X68000
#include <cstring>
#endif

ConOutputModifier co_endl(ConOutputModifier::ENDL);
ConOutputModifier co_flush(ConOutputModifier::FLUSH);
ConOutputModifier co_hex(ConOutputModifier::HEX);

//#define USE_X68000

#ifndef USE_X68000
HWND hInput, hOutput;
HWND hSource;
HWND hDrawWindow;
#else
HWND hOutput;
HWND hDrawWindow;
#endif

// �}�b�`���O�֐�
CommandMatchResult matchCommand(const char* input, const char* const* commands, int command_count) {
    // ���S��v�̊m�F
    for (int i = 0; i < command_count; ++i) {
        if (strcmp(commands[i], input) == 0) {
            CommandMatchResult result = { true, commands[i] };
            return result;
        }
    }

    // �v���t�B�b�N�X��v�̊m�F
    const char* match = nullptr;
    int matchCount = 0;
    size_t input_len = strlen(input);

    for (int i = 0; i < command_count; ++i) {
        if (strncmp(commands[i], input, input_len) == 0) {
            match = commands[i];
            matchCount++;
        }
    }

    if (matchCount == 1) {
        CommandMatchResult result = { true, match };
        return result;
    }

    // ��ӂłȂ��A�܂��͈�v���Ȃ��ꍇ
    CommandMatchResult result = { false, nullptr };
    return result;
}

// split �֐�
// line: ���͕�����i�ύX����܂��j
// delim: ��؂蕶��
// tokens: ���ʂ̃|�C���^�z��
// �߂�l: �������ꂽ�g�[�N����
int split(char* line, char delim, char* tokens[], int max_tokens) {
    int count = 0;
    char* p = line;

    while (*p != '\0' && count < max_tokens) {
        // �g�[�N���̊J�n�ʒu���L�^
        tokens[count++] = p;

        // ��؂蕶���܂��͏I�[�܂Ői��
        while (*p != '\0' && *p != delim) {
            p++;
        }

        if (*p == delim) {
            // ��؂蕶�����I�[�ɒu������
            *p = '\0';
            p++; // ���̃g�[�N���̐擪��
        }
    }

    return count;
}

#ifndef USE_X68000
// https://qiita.com/tsuchinokoman/items/869a30e02e6ddb5786b3 ����R�[�h�����p
std::wstring StringToWstring(const std::string& str)
{
    std::wstring ret;
    //��x�ڂ̌Ăяo���͕����񐔂�m�邽��
    auto result = MultiByteToWideChar(CP_UTF8,
        0,
        str.c_str(),//���͕�����
        (int)str.length(),
        nullptr,
        0);
    ret.resize(result);//�m�ۂ���
    //��x�ڂ̌Ăяo���͕ϊ�
    result = MultiByteToWideChar(CP_UTF8,
        0,
        str.c_str(),//���͕�����
        (int)str.length(),
        ret.data(),
        (int)ret.size());
    return ret;
}

// https://qiita.com/tsuchinokoman/items/869a30e02e6ddb5786b3 ����R�[�h�����p
std::string WstringToString(const std::wstring& wstr)
{
    std::string ret;
    //��x�ڂ̌Ăяo���͕����񐔂�m�邽��
    auto result = WideCharToMultiByte(
        CP_ACP,
        0,
        wstr.c_str(),//���͕�����
        (int)wstr.length(),
        nullptr,
        0,
        nullptr,
        nullptr);
    ret.resize(result);//�m�ۂ���
    //��x�ڂ̌Ăяo���͕ϊ�
    result = WideCharToMultiByte(
        CP_ACP,
        0,
        wstr.c_str(),//���͕�����
        (int)wstr.length(),
        ret.data(),
        (int)ret.size(),
        nullptr,
        nullptr);
    return ret;
}
#endif
