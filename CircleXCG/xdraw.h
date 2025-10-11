#pragma once
#include <cstdint>
#include <stdio.h>

// �C�����C���A�Z���u���Ŏg�����߂̃O���[�o���ϐ��̒�`
#define MAX_DRAW_TEXT 1024 // �ő啶����
#define MAX_XARGS 8 // �ő������
int16_t g_color;
int32_t g_x1, g_y1, g_x2, g_y2, g_x3, g_y3, g_w, g_h, g_w2, g_rx, g_ry;
int32_t g_start_angle, g_sweep_angle;
int32_t g_r, g_hbyw, g_end_angle, g_sc1, g_sc2, g_sc3, g_sc4, g_page;
uint8_t g_mx, g_my, g_font_size, g_rotation;
char g_text[MAX_DRAW_TEXT];
int32_t g_gsize = 0;        // �O���t�B�b�N�X�̈�̕� 0: 512x512, 1: 1024x1024
int32_t g_gaddr = 0xc00000; // �O���t�B�b�N�X�p�������[�̐擪�̃A�h���X
uint16_t g_xargs[MAX_XARGS];
void global_copy_args(uint16_t* args, int argsize) {
    for (int i = 0; i < argsize; ++i) {
        g_xargs[i] = args[i];
    }
}
void global_copy_args(int16_t* args, int argsize) {
    for (int i = 0; i < argsize; ++i) {
        g_xargs[i] = args[i];
    }
}
uint16_t global_get_arg(int index) {
    if (index < 0 || index >= MAX_XARGS) {
        return 0; // �͈͊O�̃C���f�b�N�X��0��Ԃ�
    }
    return g_xargs[index];
}
void global_set_crtmode(int mode) {
    if (mode >= 4 && mode <= 15) {
        g_gsize = 0;
    }
    else {
        g_gsize = 1;
    }
}
void global_set_apage(int page) {
    g_gaddr = 0xc00000 + (page * 0x80000); // �y�[�W���ƂɃA�h���X��ݒ�
}
#ifdef PACKED_STRUCT
struct __attribute__((packed)) PaintStruct {
#else
struct PaintStruct {
#endif
    uint16_t x;
    uint16_t y;
    uint16_t color;
    char* start_work;
    char* end_work;
};
PaintStruct g_paint = { 0, 0, 0, nullptr, nullptr };
PaintStruct* p_paint = &g_paint;
#ifdef PACKED_STRUCT
struct __attribute__((packed)) DrawStringStruct {
#else
struct DrawStringStruct {
#endif
    uint16_t x;
    uint16_t y;
    const char* str;
    uint8_t mx;
    uint8_t my;
    uint16_t color;
    uint8_t font_size;
    uint8_t rotation;
};
DrawStringStruct g_draw_string = { 0, 0, nullptr, 0, 0, 0, 0, 0 };
DrawStringStruct* p_draw_string = &g_draw_string;

// IOCS�R�[��
// ��ʃ��[�h��ݒ�
void crtmod(int mode)
{
    global_set_crtmode(mode); // �O���[�o���ϐ��ɉ�ʃ��[�h��ݒ�
    asm(
        "moveq  #0x10, %%d0\n\t" // _CRTMOD
        "move.w %0, %%d1\n\t"
        "trap   #15\n\t"
        :
    : "r"(mode)              // ����
        : "d0", "d1"             // �g�p���郌�W�X�^
        );
}
int get_crtmod()
{
    int mode;
    asm(
        "moveq  #0x10, %%d0\n\t" // _CRTMOD
        "move.w #-1, %%d1\n\t"
        "trap   #15\n\t"
        "move.l %%d0, %0\n\t"
        : "=r"(mode)
        :                        // ����
        : "d0", "d1"             // �g�p���郌�W�X�^
    );
    return mode;
}
// �O���t�B�b�N��ʂ̃y�[�W���N���A
void gclear()
{
    asm(
        "move.l #0x90, %%d0\n\t" // _G_CLR_ON
        "trap   #15\n\t"
        :
    :                        // ���͂Ȃ�
        : "d0"                   // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂ̏������݃y�[�W��ݒ�
void apage(int page)
{
    global_set_apage(page); // �O���[�o���ϐ��Ƀy�[�W��ݒ�
    asm(
        "move.l #0xb1, %%d0\n\t" // _APAGE
        "move.l %0, %%d1\n\t"
        "trap   #15\n\t"
        :
    : "r"(page)              // ����
        : "d0", "d1"             // �g�p���郌�W�X�^
        );
}
int get_apage()
{
    int page;
    asm(
        "move.l #0xb1, %%d0\n\t" // _APAGE
        "move.l #-1, %%d1\n\t"
        "trap   #15\n\t"
        "move.l %%d0, %0\n\t"
        : "=r"(page)
        :                        // ����
        : "d0", "d1"             // �g�p���郌�W�X�^
    );
    return page;
}
// �O���t�B�b�N��ʂ̕\���y�[�W��ݒ�
void vpage(int pages)
{
    asm(
        "move.l #0xb2, %%d0\n\t" // _VPAGE
        "move.l %0, %%d1\n\t"
        "trap   #15\n\t"
        :
    : "r"(pages)             // ����
        : "d0", "d1"             // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂ̃E�B���h�E��ݒ�
void window(int x1, int y1, int x2, int y2)
{
    asm(
        "move.l #0xb4, %%d0\n\t" // _WINDOW
        "move.l %0, %%d1\n\t"
        "move.l %1, %%d2\n\t"
        "move.l %2, %%d3\n\t"
        "move.l %3, %%d4\n\t"
        "trap   #15\n\t"
        :
    : "r"(x1), "r"(y1), "r"(x2), "r"(y2) // ����
        : "d0", "d1", "d2", "d3", "d4"       // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂ��N���A
void wipe()
{
    asm(
        "move.l #0xb5, %%d0\n\t" // _WIPE
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0"                   // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂɓ_��`��
void pset(uint16_t* data)
{
    global_copy_args(data, 3); // �������O���[�o���ϐ��ɃR�s�[
    asm(
        "move.l #0xb6, %%d0\n\t" // _PSET
        "lea    g_xargs, %%a1\n\t" // a1 = [x, y, color]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
}
void psetx(int x1, int y1, int color)
{
    asm(
        "move.l   %0, %%d1\n\t"
        "move.l   %1, %%d2\n\t"
        "move.l   %2, %%d3\n\t"
        "clr.l	  -(%%sp)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h��)
        "move.l   %%d0, (%%sp)\n\t"
        "movea.l  %%d1, %%a0\n\t"
        "adda.l   %%d1, %%a0\n\t"  // a0 = x1 * 2
        "moveq.l  #10, %%d0\n\t"
        "add.l    g_gsize, %d0\n\t"
        "lsl.l    %%d0, %%d2\n\t"  // d2 = y1 * (1024 << g_gsize)
        "adda.l   %%d2, %%a0\n\t"  // a0 = x1 * 2 + y1 * (1024 << g_gsize)
        "adda.l   g_gaddr, %%a0\n\t" // a0 = g_gaddr + x1 * 2 + y1 * (1024 << g_gsize)
        "move.w   %%d3, (%%a0)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h����߂�)
        "addq.l   #4,%%sp\n\t"
        :
    : "r"(x1), "r"(y1), "r"(color) // ����
        : "d0", "d1", "d2", "d3", "a0" // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂɓ_�̐F��Ԃ�
void point(uint16_t* data)
{
    global_copy_args(data, 2); // �������O���[�o���ϐ��ɃR�s�[
    asm(
        "move.l #0xb7, %%d0\n\t" // _POINT
        "lea    g_xargs, %%a1\n\t" // a1 = [x, y, color]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
    data[2] = global_get_arg(2); // ���ʂ�data�Ɋi�[
}
int pointx(int x1, int y1)
{
    uint16_t color = 0;
    asm(
        "move.l   %1, %%d1\n\t"
        "move.l   %2, %%d2\n\t"
        "clr.l	  -(%%sp)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h��)
        "move.l   %%d0, (%%sp)\n\t"
        "movea.l  %%d1, %%a0\n\t"
        "adda.l   %%d1, %%a0\n\t"  // a0 = x1 * 2
        "moveq.l  #10, %%d0\n\t"
        "add.l    g_gsize, %d0\n\t"
        "lsl.l    %%d0, %%d2\n\t"  // d2 = y1 * (1024 << g_gsize)
        "adda.l   %%d2, %%a0\n\t"  // a0 = x1 * 2 + y1 * (1024 << g_gsize)
        "adda.l   g_gaddr, %%a0\n\t" // a0 = g_gaddr + x1 * 2 + y1 * (1024 << g_gsize)
        "move.w   (%%a0), %%d3\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h����߂�)
        "addq.l   #4,%%sp\n\t"
        "move.l   %%d3, %0\n\t"
        : "=r"(color)
        : "r"(x1), "r"(y1) // ����
        : "d0", "d1", "d2", "d3", "a0" // �g�p���郌�W�X�^
    );
    return color; // ���ʂ�Ԃ�
}
// �O���t�B�b�N��ʂɐ���`��
void drawline(uint16_t* data)
{
    global_copy_args(data, 6); // �������O���[�o���ϐ��ɃR�s�[
    asm(
        "move.l #0xb8, %%d0\n\t"   // _LINE
        "lea    g_xargs, %%a1\n\t" // a1 = [x1, y1, x2, y2, color, style]
        "trap   #15\n\t"
        :
    :                          // ����
        : "d0", "a1"               // �g�p���郌�W�X�^
        );
}
void drawlinex(int x1, int y1, int x2, int y2, COLORREF color)
{
	g_xargs[0] = x1;
	g_xargs[1] = y1;
	g_xargs[2] = x2;
	g_xargs[3] = y2;
	g_xargs[4] = color;
    asm(
        "move.l #0xb8, %%d0\n\t"   // _LINE
        "lea    g_xargs, %%a1\n\t" // a1 = [x1, y1, x2, y2, color, style]
        "trap   #15\n\t"
        :
    :                          // ����
        : "d0", "a1"               // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂɒ����`��`��
void drawbox(uint16_t* data)
{
    global_copy_args(data, 6); // �������O���[�o���ϐ��ɃR�s�[
    asm(
        "move.l #0xb9, %%d0\n\t" // _BOX
        "lea    g_xargs, %%a1\n\t" // a1 = [x1, y1, x2, y2, color, style]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
}
void drawboxx(int x1, int y1, int w, int h, COLORREF color)
{
    asm(
        "move.l   %0, %%d1\n\t"
        "move.l   %1, %%d2\n\t"
        "move.l   %2, %%d3\n\t"
        "move.l   %3, %%d4\n\t"
        "move.l   %4, %%d5\n\t"
        "clr.l	  -(%%sp)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h��)
        "move.l   %%d0, (%%sp)\n\t"

        "movea.l  %%d1, %%a0\n\t"
        "adda.l   %%d1, %%a0\n\t"  // a0 = x1 * 2
        "moveq.l  #10, %%d0\n\t"
        "add.l    g_gsize, %%d0\n\t"
        "lsl.l    %%d0, %%d2\n\t"  // d2 = y1 * (1024 << g_gsize)
        "adda.l   %%d2, %%a0\n\t"  // a0 = x1 * 2 + y1 * (1024 << g_gsize)
        "adda.l   g_gaddr, %%a0\n\t" // a0 = g_gaddr + x1 * 2 + y1 * (1024 << g_gsize)
        "move.l   %%a0, %%a1\n\t"

        "move.l   %%a0, -(%%sp)\n\t"

        "move.l   %%d4, %%d2\n\t"
        "lsl.l    %%d0, %%d2\n\t"  // d2 = h * (1024 << g_gsize)
        "adda.l   %%d2, %%a1\n\t"  // a1 = x1 * 2 + (y1 + h) * (1024 << g_gsize)

        "move.l   %%d3, %%d6\n\t"  // d6 = w (x�����̃J�E���^�[)
        "boxxxlp:\n\t"
        "move.w   %%d5, (%%a0)+\n\t"
        "move.w   %%d5, (%%a1)+\n\t"
        "dbra     %%d6, boxxxlp\n\t" // x�����̃��[�v

        "move.l   (%%sp)+, %%a0\n\t" // a0 = g_gaddr + x1 * 2 + y1 * (1024 << g_gsize)
        "move.l   %%a0, %%a1\n\t"
        "adda.l   %%d3, %%a1\n\t"
        "adda.l   %%d3, %%a1\n\t"  // a1 = g_gaddr + (x1 + w) * 2 + y1 * (1024 << g_gsize)

        "moveq.l  #1, %%d2\n\t"
        "lsl.l    %%d0, %%d2\n\t"  // d2 = (1024 << g_gsize) (y�����̍���)

        "move.l   %%d4, %%d7\n\t"  // d7 = h (y�����̃J�E���^�[)
        "boxxylp:\n\t"
        "move.w   %%d5, (%%a0)\n\t"
        "adda.l   %%d2, %%a0\n\t"
        "move.w   %%d5, (%%a1)\n\t"
        "adda.l   %%d2, %%a1\n\t"
        "dbra     %%d7, boxxylp\n\t" // y�����̃��[�v

        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h����߂�)
        "addq.l   #4,%%sp\n\t"
        :
    : "r"(x1), "r"(y1), "r"(w), "r"(h), "r"(color) // ����
        : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "a0", "a1" // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂ̒����`��h��Ԃ�
void fillbox(uint16_t* data)
{
    global_copy_args(data, 5); // �������O���[�o���ϐ��ɃR�s�[
    asm(
        "move.l #0xba, %%d0\n\t" // _FILL
        "lea    g_xargs, %%a1\n\t" // a1 = [x1, y1, x2, y2, color]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
}
void fillboxx(int x1, int y1, int w, int h, COLORREF color)
{
    asm(
        "move.l   %0, %%d1\n\t"
        "move.l   %1, %%d2\n\t"
        "move.l   %2, %%d3\n\t"
        "move.l   %3, %%d4\n\t"
        "move.l   %4, %%d5\n\t"

        "clr.l	  -(%%sp)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h��)
        "move.l   %%d0, (%%sp)\n\t"

        "movea.l  %%d1, %%a0\n\t"
        "adda.l   %%d1, %%a0\n\t"  // a0 = x1 * 2
        "moveq.l  #10, %%d0\n\t"
        "add.l    g_gsize, %%d0\n\t"
        "lsl.l    %%d0, %%d2\n\t"  // d2 = y1 * (1024 << g_gsize)
        "adda.l   %%d2, %%a0\n\t"  // a0 = x1 * 2 + y1 * (1024 << g_gsize)
        "adda.l   g_gaddr, %%a0\n\t" // a0 = g_gaddr + x1 * 2 + y1 * (1024 << g_gsize)
        "move.l   %%a0, %%a1\n\t"

        "moveq.l  #1, %%d2\n\t"
        "lsl.l    %%d0, %%d2\n\t"  // d2 = (1024 << g_gsize) (y�����̍���)

        "move.l   %%d4, %%d7\n\t"  // d7 = h (y�����̃J�E���^�[)
        "fillxylp:\n\t"
        "move.l   %%d3, %%d6\n\t"  // d6 = w (x�����̃J�E���^�[)
        "fillxxlp:\n\t"

        "move.w   %%d5, (%%a0)+\n\t"

        "dbra     %%d6, fillxxlp\n\t" // x�����̃��[�v
        "adda.l   %%d2, %%a1\n\t"
        "move.l   %%a1, %%a0\n\t"
        "dbra     %%d7, fillxylp\n\t" // y�����̃��[�v

        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h����߂�)
        "addq.l   #4,%%sp\n\t"
        :
    : "r"(x1), "r"(y1), "r"(w), "r"(h), "r"(color) // ����
        : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "a0", "a1" // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂɑȉ~��`��
void drawcircle(int16_t* data)
{
    global_copy_args(data, 7); // �������O���[�o���ϐ��ɃR�s�[
    asm(
        "move.l #0xbb, %%d0\n\t" // _CIRCLE
        "lea    g_xargs, %%a1\n\t"    // a1 = [cx, cy, r, color, start_angle, end_angle, aspect_ratio]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂ̎w�肵���ꏊ��h��Ԃ�
void paint(PaintStruct* data)
{
    g_paint.x = data->x;
    g_paint.y = data->y;
    g_paint.color = data->color;
    g_paint.start_work = data->start_work;
    g_paint.end_work = data->end_work;
    asm(
        "move.l #0xbc, %%d0\n\t" // _PAINT
        "lea    p_paint, %%a1\n\t"    // a1 = [x, y, color, start_work, end_work]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
}
// �O���t�B�b�N��ʂɕ������\������
void drawstring(DrawStringStruct* data)
{
    g_draw_string.x = data->x;
    g_draw_string.y = data->y;
    g_draw_string.str = data->str;
    g_draw_string.mx = data->mx;
    g_draw_string.my = data->my;
    g_draw_string.color = data->color;
    g_draw_string.font_size = data->font_size;
    g_draw_string.rotation = data->rotation;
    asm(
        "move.l #0xbd, %%d0\n\t" // _SYMBOL
        "lea    p_draw_string, %%a1\n\t"    // a1 = [x, y, str, mx, my, color, font_size, rotation]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
}

void drawarcx(int x1, int y1, int w, int h, int start_angle, int sweep_angle, COLORREF color) {
	g_xargs[0] = x1 + w / 2;
	g_xargs[1] = y1 + h / 2;
	g_xargs[2] = h / 2;
	g_xargs[3] = color;
	g_xargs[4] = start_angle;
	g_xargs[5] = start_angle + sweep_angle;
	g_xargs[6] = h * 256 / w; // aspect_ratio = h / w * 256
    asm(
        "move.l #0xbb, %%d0\n\t" // _CIRCLE
        "lea    g_xargs, %%a1\n\t"    // a1 = [cx, cy, r, color, start_angle, end_angle, aspect_ratio]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
}
void drawpiex(int x1, int y1, int w, int h, int start_angle, int sweep_angle, COLORREF color) {
    g_xargs[0] = x1 + w / 2;
    g_xargs[1] = y1 + h / 2;
    g_xargs[2] = h / 2;
    g_xargs[3] = color;
    g_xargs[4] = -start_angle;
    g_xargs[5] = -start_angle - sweep_angle;
    g_xargs[6] = h * 256 / w; // aspect_ratio = h / w * 256
    asm(
        "move.l #0xbb, %%d0\n\t" // _CIRCLE
        "lea    g_xargs, %%a1\n\t"    // a1 = [cx, cy, r, color, start_angle, end_angle, aspect_ratio]
        "trap   #15\n\t"
        :
    :                        // ����
        : "d0", "a1"             // �g�p���郌�W�X�^
        );
}
void drawroundedrectanglex(int x1, int y1, int w, int h, int rx, int ry, COLORREF color) {
    int dx = rx * 2;
    int dy = ry * 2;
    drawlinex(x1 + rx, y1, x1 + w - rx, y1, color);
    drawlinex(x1 + rx, y1 + h, x1 + w - rx, y1 + h, color);
    drawlinex(x1, y1 + ry, x1, y1 + h - ry, color);
    drawlinex(x1 + w, y1 + ry, x1 + w, y1 + h - ry, color);
    drawarcx(x1, y1, dx, dy, 180, 90, color);
    drawarcx(x1 + w - dx, y1, dx, dy, 270, 90, color);
    drawarcx(x1, y1 + h - dy, dx, dy, 90, 90, color);
    drawarcx(x1 + w - dx, y1 + h - dy, dx, dy, 0, 90, color);
}
void drawellipsex(int x1, int y1, int w, int h, COLORREF color) {
    //printf("drawellipsex: x1=%d, y1=%d, w=%d, h=%d, color=%06X\n", x1, y1, w, h, color);
    drawarcx(x1, y1, w, h, 0, 360, color);
}

void fillellipsex(int x1, int y1, int w, int h, COLORREF color) {
    asm(
        "move.l   %0, g_x1\n\t"
        "move.l   %1, g_y1\n\t"
        "move.l   %2, g_w\n\t"
        "move.l   %3, g_h\n\t"
        "move.l   %4, g_color\n\t"

        "clr.l	  -(%%sp)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h��)
        "move.l   %%d0, (%%sp)\n\t"

        "move.l   g_color, -(%%sp)\n\t"
        "move.l   g_h, -(%%sp)\n\t"
        "move.l   g_w, -(%%sp)\n\t"
        "move.l   g_y1, -(%%sp)\n\t"
        "move.l   g_x1, -(%%sp)\n\t"
        "move.l   #0, -(%%sp)\n\t"
        //"jsr      fillellipsex_a\n\t"
        "jsr      fillellipse_asm\n\t"
        "add.l    #24,%%sp\n\t"

        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h����߂�)
        "addq.l   #4, %%sp\n\t"
        :                        // �o��
    : "r"(x1), "r"(y1), "r"(w), "r"(h), "r"(color) // ����
        : "d0"                   // �g�p���郌�W�X�^
        );
}
void fillroundedrectanglex(int x1, int y1, int w, int h, int rx, int ry, COLORREF color) {
    asm(
        "move.l   %0, g_x1\n\t"
        "move.l   %1, g_y1\n\t"
        "move.l   %2, g_w\n\t"
        "move.l   %3, g_h\n\t"
        "move.l   %4, g_rx\n\t"
        "move.l   %5, g_ry\n\t"
        "move.l   %6, g_color\n\t"

        "clr.l	  -(%%sp)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h��)
        "move.l   %%d0, (%%sp)\n\t"

        "move.l   g_color, -(%%sp)\n\t"
        "move.l   g_ry, -(%%sp)\n\t"
        "move.l   g_rx, -(%%sp)\n\t"
        "move.l   g_h, -(%%sp)\n\t"
        "move.l   g_w, -(%%sp)\n\t"
        "move.l   g_y1, -(%%sp)\n\t"
        "move.l   g_x1, -(%%sp)\n\t"
        "move.l   #0, -(%%sp)\n\t"
        //"jsr      fillroundedrectanglex_a\n\t"
        "jsr      fillroundedrectangle_asm\n\t"
        "add.l    #32,%%sp\n\t"

        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h����߂�)
        "addq.l   #4, %%sp\n\t"
        :                        // �o��
    : "r"(x1), "r"(y1), "r"(w), "r"(h), "r"(rx), "r"(ry), "r"(color) // ����
        : "d0"                   // �g�p���郌�W�X�^
        );
}
void filltrapezoidx(int x1, int y1, int x2, int y2, int w1, int w2, COLORREF color) {
    asm(
        "move.l   %0, g_x1\n\t"
        "move.l   %1, g_y1\n\t"
        "move.l   %2, g_x2\n\t"
        "move.l   %3, g_y2\n\t"
        "move.l   %4, g_w\n\t"
        "move.l   %5, g_w2\n\t"
        "move.l   %6, g_color\n\t"

        "clr.l	  -(%%sp)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h��)
        "move.l   %%d0, (%%sp)\n\t"

        "move.l   g_color, -(%%sp)\n\t"
        "move.l   g_w2, -(%%sp)\n\t"
        "move.l   g_w, -(%%sp)\n\t"
        "move.l   g_y2, -(%%sp)\n\t"
        "move.l   g_x2, -(%%sp)\n\t"
        "move.l   g_y1, -(%%sp)\n\t"
        "move.l   g_x1, -(%%sp)\n\t"
        "move.l   #0, -(%%sp)\n\t"
        "jsr      filltrapezoid_asm\n\t"
        "add.l    #32,%%sp\n\t"

        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h����߂�)
        "addq.l   #4, %%sp\n\t"
        :                        // �o��
    : "r"(x1), "r"(y1), "r"(x2), "r"(y2), "r"(w1), "r"(w2), "r"(color) // ����
        : "d0"                   // �g�p���郌�W�X�^
        );
}
void filltrianglex(int x1, int y1, int x2, int y2, int x3, int y3, COLORREF color) {
    asm(
        "move.l   %0, g_x1\n\t"
        "move.l   %1, g_y1\n\t"
        "move.l   %2, g_x2\n\t"
        "move.l   %3, g_y2\n\t"
        "move.l   %4, g_x3\n\t"
        "move.l   %5, g_y3\n\t"
        "move.l   %6, g_color\n\t"

        "clr.l	  -(%%sp)\n\t"
        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h��)
        "move.l   %%d0, (%%sp)\n\t"

        "move.l   g_color, -(%%sp)\n\t"
        "move.l   g_y3, -(%%sp)\n\t"
        "move.l   g_x3, -(%%sp)\n\t"
        "move.l   g_y2, -(%%sp)\n\t"
        "move.l   g_x2, -(%%sp)\n\t"
        "move.l   g_y1, -(%%sp)\n\t"
        "move.l   g_x1, -(%%sp)\n\t"
        "move.l   #0, -(%%sp)\n\t"
        "jsr      filltriangle_asm\n\t"
        "add.l    #32,%%sp\n\t"

        "dc.w	  0xff20\n\t"      // _SUPER (�X�[�p�[�o�C�U�[���[�h����߂�)
        "addq.l   #4, %%sp\n\t"
        :                        // �o��
    : "r"(x1), "r"(y1), "r"(x2), "r"(y2), "r"(x3), "r"(y3), "r"(color) // ����
        : "d0"                   // �g�p���郌�W�X�^
        );
}
