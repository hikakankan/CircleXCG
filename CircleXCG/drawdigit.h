#pragma once

COLORREF xpalette_to_rgb(int c);

void fillboxx(int x1, int y1, int w, int h, COLORREF color);
void fillellipsex(int x1, int y1, int w, int h, COLORREF color);
void filltrapezoidx(int x1, int y1, int x2, int y2, int w1, int w2, COLORREF color);

inline void DrawHLine(HDC hdc, int x1, int y1, int w, COLORREF color) {
    for (int x = x1; x <= x1 + w; ++x) {
        SetPixel(hdc, x, y1, color);
    }
}
void DirectFillRectangle(HDC hdc, int x1, int y1, int w, int h, COLORREF color) {
    for (int y = y1; y < y1 + h; ++y) {
        DrawHLine(hdc, x1, y, w, color);
    }
}
void DirectFillEllipse(HDC hdc, int x1, int y1, int w, int h, COLORREF color) {
    int cx = x1 + w / 2;
    int cy = y1 + h / 2;
    for (int y = y1; y < y1 + h; ++y) {
        double uy = ((double)y - cy) * 2 / h; // �P�ʉ~��y���W
        int ex = (int)(sqrt(1 - uy * uy) * w / 2); // �ȉ~��x���W
        DrawHLine(hdc, cx - ex, y, ex * 2, color);
    }
}
int get_coordinate(int x1, int y1, int x2, int y2, int y) {
    // �����̕���������x���W���v�Z
    if (y1 == y2) return x1; // �������̏ꍇ
    return x1 + (x2 - x1) * (y - y1) / (y2 - y1);
}
void DirectFillTrapezoid(HDC hdc, int x1, int y1, int x2, int y2, int w1, int w2, COLORREF color) {
    // �h��Ԃ���`��`��
    // ��`�̏�� (x1, y1) ���牺�� (x2, y2) �܂ł̐����ƁA���̕� w1, ����̕� w2 ������
    int x1_left = w1 > 0 ? x1 : x1 + w1;
    int x1_right = w1 > 0 ? x1 + w1 : x1;
    int x2_left = w2 > 0 ? x2 : x2 + w2;
    int x2_right = w2 > 0 ? x2 + w2 : x2;
    for (int y = y1; y <= y2; ++y) {
        int left_x = get_coordinate(x1_left, y1, x2_left, y2, y);
        int right_x = get_coordinate(x1_right, y1, x2_right, y2, y);
        DrawHLine(hdc, left_x, y, right_x - left_x, color);
    }
}

void DirectFillRectangleX(HDC hdc, int x1, int y1, int w, int h, int color) {
#ifdef USE_X68000
    fillboxx(x1, y1, w, h, color);
#else
    DirectFillRectangle(hdc, x1, y1, w + 1, h + 1, xpalette_to_rgb(color));
#endif
}

void DirectFillEllipseX(HDC hdc, int x1, int y1, int w, int h, int color) {
#ifdef USE_X68000
    fillellipsex(x1, y1, w, h, color);
#else
    DirectFillEllipse(hdc, x1, y1, w + 1, h + 1, xpalette_to_rgb(color));
#endif
}

void DirectFillTrapezoidX(HDC hdc, int x1, int y1, int x2, int y2, int w1, int w2, int color) {
#ifdef USE_X68000
    filltrapezoidx(x1, y1, x2, y2, w1, w2, color);
#else
    DirectFillTrapezoid(hdc, x1, y1, x2, y2, w1, w2, xpalette_to_rgb(color));
#endif
}

void draw_digit1(HDC hdc, int n, int x, int y, int w, int h, int color) {
    double rw1 = 0.15;
    double rh1 = 0.1;
    double rw2 = 0.15;
    double rh2 = 0.2;
    double rx1 = 0.5;
    double rx3 = 0.1;
    double rx4 = 0.6;
    double rx7 = 0.3;
    double ry4 = 0.7;
    if (n == 0 || n == 2 || n == 3 || n == 6 || n == 8 || n == 9) {
        // ��̉���
        DirectFillTrapezoidX(hdc,
            (int)(x + w * rw1),
            (int)(y),
            (int)(x),
            (int)(y + h * rh1),
            (int)(w - w * rw1 * 2),
            (int)(w),
            color);
    }
    if (n == 0 || n == 3 || n == 5 || n == 6 || n == 8 || n == 9) {
        // ���̉���
        DirectFillTrapezoidX(hdc,
            (int)(x),
            (int)(y + h - h * rh1),
            (int)(x + w * rw1),
            (int)(y + h),
            (int)(w),
            (int)(w - w * rw1 * 2),
            color);
    }
    if (n == 3) {
        // �����̉���
        DirectFillTrapezoidX(hdc,
            (int)(x + w * rx3),
            (int)(y + h / 2 - h * rh1 * 0.5),
            (int)(x + w * rx3),
            (int)(y + h / 2),
            (int)(w - w * rx3),
            (int)(w - w * rx3 - w * rw1),
            color);
        DirectFillTrapezoidX(hdc,
            (int)(x + w * rx3),
            (int)(y + h / 2),
            (int)(x + w * rx3),
            (int)(y + h / 2 + h * rh1 * 0.5),
            (int)(w - w * rx3 - w * rw1),
            (int)(w - w * rx3),
            color);
    }
    if (n == 8) {
        // �����̉���
        DirectFillTrapezoidX(hdc,
            (int)(x),
            (int)(y + h / 2 - h * rh1 * 0.5),
            (int)(x + w * rw1),
            (int)(y + h / 2),
            (int)(w),
            (int)(w - w * rw1 * 2),
            color);
        DirectFillTrapezoidX(hdc,
            (int)(x + w * rw1),
            (int)(y + h / 2),
            (int)(x),
            (int)(y + h / 2 + h * rh1 * 0.5),
            (int)(w - w * rw1 * 2),
            (int)(w),
            color);
    }
    if (n == 5 || n == 6) {
        // �����̉���
        DirectFillTrapezoidX(hdc,
            (int)(x),
            (int)(y + h / 2 - h * rh1 * 0.5),
            (int)(x),
            (int)(y + h / 2 + h * rh1 * 0.5),
            (int)(w - w * rw1),
            (int)(w),
            color);
    }
    if (n == 9) {
        // �����̉���
        DirectFillTrapezoidX(hdc,
            (int)(x),
            (int)(y + h / 2 - h * rh1 * 0.5),
            (int)(x + w * rw1),
            (int)(y + h / 2 + h * rh1 * 0.5),
            (int)(w),
            (int)(w - w * rw1),
            color);
    }
    if (n == 0 || n == 6) {
        // ���̏c��
        DirectFillRectangleX(hdc,
            (int)(x),
            (int)(y + h * rh1),
            (int)(w * rw1),
            (int)(h - h * rh1 * 2),
            color);
    }
    if (n == 0 || n == 9) {
        // �E�̏c��
        DirectFillRectangleX(hdc,
            (int)(x + w - w * rw1),
            (int)(y + h * rh1),
            (int)(w * rw1),
            (int)(h - h * rh1 * 2),
            color);
    }
    if (n == 1) {
        // �c��
        DirectFillTrapezoidX(hdc,
            (int)(x + w * rx1 - w * rw1 / 2),
            (int)(y),
            (int)(x + w * rx1 - w * rw1 / 2 - w * rw2),
            (int)(y + h * rh1),
            (int)(0),
            (int)(w * rw2),
            color);
        DirectFillRectangleX(hdc,
            (int)(x + w * rx1 - w * rw1 / 2),
            (int)(y),
            (int)(w * rw1),
            (int)(h),
            color);
    }
    if (n == 4) {
        // �c��
        DirectFillRectangleX(hdc,
            (int)(x + w * rx4 - w * rw1 / 2),
            (int)(y),
            (int)(w * rw1),
            (int)(h),
            color);
    }
    if (n == 2) {
        // ���̉���
        DirectFillRectangleX(hdc,
            (int)(x),
            (int)(y + h - h * rh1),
            (int)(w),
            (int)(h * rh1),
            color);
    }
    if (n == 4) {
        // ��̉���
        DirectFillRectangleX(hdc,
            (int)(x),
            (int)(y + h * ry4 - h * rh1 * 0.5),
            (int)(w),
            (int)(h * rh1),
            color);
    }
    if (n == 5 || n == 7) {
        // ��̉���
        DirectFillRectangleX(hdc,
            (int)(x),
            (int)(y),
            (int)(w),
            (int)(h * rh1),
            color);
    }
    if (n == 5 || n == 8 || n == 9) {
        // ����̏c��
        DirectFillRectangleX(hdc,
            (int)(x),
            (int)(y + h * rh1),
            (int)(w * rw1),
            (int)(h / 2 - h * rh1 * 1.5),
            color);
    }
    if (n == 3 || n == 8) {
        // �E��̏c��
        DirectFillRectangleX(hdc,
            (int)(x + w - w * rw1),
            (int)(y + h * rh1),
            (int)(w * rw1),
            (int)(h / 2 - h * rh1 * 1.5),
            color);
    }
    if (n == 8) {
        // �����̏c��
        DirectFillRectangleX(hdc,
            (int)(x),
            (int)(y + h / 2 + h * rh1 * 0.5),
            (int)(w * rw1),
            (int)(h / 2 - h * rh1 * 1.5),
            color);
    }
    if (n == 3 || n == 5 || n == 6 || n == 8) {
        // �E���̏c��
        DirectFillRectangleX(hdc,
            (int)(x + w - w * rw1),
            (int)(y + h / 2 + h * rh1 * 0.5),
            (int)(w * rw1),
            (int)(h / 2 - h * rh1 * 1.5),
            color);
    }
    if (n == 2 || n == 3) {
        // ����̏c��
        DirectFillRectangleX(hdc,
            (int)(x),
            (int)(y + h * rh1),
            (int)(w * rw1),
            (int)(h * rh2),
            color);
    }
    if (n == 2 || n == 6 || n == 7) {
        // �E��̏c��
        DirectFillRectangleX(hdc,
            (int)(x + w - w * rw1),
            (int)(y + h * rh1),
            (int)(w * rw1),
            (int)(h * rh2),
            color);
    }
    if (n == 3 || n == 5 || n == 9) {
        // �����̏c��
        DirectFillRectangleX(hdc,
            (int)(x),
            (int)(y + h - h * rh1 - h * rh2),
            (int)(w * rw1),
            (int)(h * rh2),
            color);
    }
    if (n == 2) {
        // �΂߂̐�
        DirectFillTrapezoidX(hdc,
            (int)(x + w - w * rw1),
            (int)(y + h * rh1 + h * rh2),
            (int)(x),
            (int)(y + h - h * rh1),
            (int)(w * rw1),
            (int)(w * rw1),
            color);
    }
    if (n == 4) {
        // �΂߂̐�
        DirectFillTrapezoidX(hdc,
            (int)(x + w * rx4 - w * rw1 * 0.5),
            (int)(y),
            (int)(x),
            (int)(y + h * ry4 - h * rh1 * 0.5),
            (int)(w * rw1),
            (int)(w * rw1),
            color);
    }
    if (n == 7) {
        // �΂߂̐�
        DirectFillTrapezoidX(hdc,
            (int)(x + w - w * rw1),
            (int)(y + h * rh1 + h * rh2),
            (int)(x + w * rx7 - w * rw1 * 0.5),
            (int)(y + h),
            (int)(w * rw1),
            (int)(w * rw1),
            color);
    }
    if (n == 10) {
        // �����_
        DirectFillEllipseX(hdc,
            (int)(x + w * rx1 - w * rw1 / 2),
            (int)(y + h - h * rh1),
            (int)(w * rw1),
            (int)(h * rh1),
            color);
    }
    if (n == 11) {
        // �}�C�i�X�̕���
        DirectFillRectangleX(hdc,
            (int)(x + w * rw1),
            (int)(y + h / 2 - h * rh1 * 0.5),
            (int)(w - w * rw1 * 2),
            (int)(h * rh1),
            color);
    }
}

void digital_vbar(HDC hdc, int x, int y, int w, int h, int color) {
    double rh1 = 0.1;
    double rw1 = 0.5;
    DirectFillTrapezoidX(hdc,
        (int)(x + w * rw1),
        (int)(y),
        (int)(x),
        (int)(y + h * rh1),
        (int)(0),
        (int)(w),
        color);
    DirectFillRectangleX(hdc,
        (int)(x),
        (int)(y + h * rh1),
        (int)(w),
        (int)(h - h * rh1 * 2),
        color);
    DirectFillTrapezoidX(hdc,
        (int)(x),
        (int)(y + h - h * rh1),
        (int)(x + w * rw1),
        (int)(y + h),
        (int)(w),
        (int)(0),
        color);
}

void digital_hbar(HDC hdc, int x, int y, int w, int h, int color) {
    double rw1 = 0.1;
    double rh1 = 0.5;
    DirectFillTrapezoidX(hdc,
        (int)(x + w * rw1),
        (int)(y),
        (int)(x),
        (int)(y + h * rh1),
        (int)(w - w * rw1 * 2),
        (int)(w),
        color);
    DirectFillTrapezoidX(hdc,
        (int)(x),
        (int)(y + h * rh1),
        (int)(x + w * rw1),
        (int)(y + h),
        (int)(w),
        (int)(w - w * rw1 * 2),
        color);
}

void draw_digit2(HDC hdc, int n, int x, int y, int w, int h, int color) {
    double rw1 = 0.15;
    double rw2 = 1 - rw1;
    double rx1 = rw1 * 0.5;
    double rxp = 0.5;
    double rh1 = 0.1;
    double rh2 = 0.5 - rh1 * 0.5;
    double ry1 = rh1 * 0.5;
    double ry2 = ry1 + rh2;
    if (n == 0 || n == 2 || n == 3 || n == 5 || n == 6 || n == 7 || n == 8 || n == 9) {
        // ��̉���
        digital_hbar(hdc,
            (int)(x + w * rx1),
            (int)(y),
            (int)(w * rw2),
            (int)(h * rh1),
            color);
    }
    if (n == 2 || n == 3 || n == 4 || n == 5 || n == 6 || n == 8 || n == 9 || n == 11) {
        // �����̉���
        digital_hbar(hdc,
            (int)(x + w * rx1),
            (int)(y + h * rh2),
            (int)(w * rw2),
            (int)(h * rh1),
            color);
    }
    if (n == 0 || n == 2 || n == 3 || n == 5 || n == 6 || n == 8 || n == 9) {
        // ���̉���
        digital_hbar(hdc,
            (int)(x + w * rx1),
            (int)(y + h * rh2 * 2),
            (int)(w * rw2),
            (int)(h * rh1),
            color);
    }
    if (n == 0 || n == 4 || n == 5 || n == 6 || n == 8 || n == 9) {
        // ����̏c��
        digital_vbar(hdc,
            (int)(x),
            (int)(y + h * ry1),
            (int)(w * rw1),
            (int)(h * rh2),
            color);
    }
    if (n == 0 || n == 1 || n == 2 || n == 3 || n == 4 || n == 7 || n == 8 || n == 9) {
        // �E��̏c��
        digital_vbar(hdc,
            (int)(x + w - w * rw1),
            (int)(y + h * ry1),
            (int)(w * rw1),
            (int)(h * rh2),
            color);
    }
    if (n == 0 || n == 2 || n == 6 || n == 8) {
        // �����̏c��
        digital_vbar(hdc,
            (int)(x),
            (int)(y + h * ry2),
            (int)(w * rw1),
            (int)(h * rh2),
            color);
    }
    if (n == 0 || n == 1 || n == 3 || n == 4 || n == 5 || n == 6 || n == 7 || n == 8 || n == 9) {
        // �E���̏c��
        digital_vbar(hdc,
            (int)(x + w - w * rw1),
            (int)(y + h * ry2),
            (int)(w * rw1),
            (int)(h * rh2),
            color);
    }
    if (n == 10) {
        // �����_
        DirectFillRectangleX(hdc,
            (int)(x + w * rxp - w * rw1 / 2),
            (int)(y + h - h * rh1),
            (int)(w * rw1),
            (int)(h * rh1),
            color);
    }
}

void draw_num_str1(HDC hdc, const char* num_str, int x, int y, int w, int h, COLORREF color) {
    double r = 0.9;
    double w2 = w * r;
    double h2 = h * r;
    double xm = w * (1 - r) * 0.5;
    double ym = h * (1 - r) * 0.5;
    for (int i = 0; num_str[i] != '\0'; ++i) {
        char c = num_str[i];
        int n;
        if (c >= '0' && c <= '9') {
            n = c - '0';
        }
        else if (c == '.') {
            n = 10;
        }
        else if (c == '-') {
            n = 11;
        }
        else {
            continue; // �s���ȕ����͖���
        }
        draw_digit1(hdc, n, x + w * i + xm, y + ym, w2, h2, color);
	}
}

void draw_num_str2(HDC hdc, const char* num_str, int x, int y, int w, int h, COLORREF color) {
    double r = 0.9;
    double w2 = w * r;
    double h2 = h * r;
    double xm = w * (1 - r) * 0.5;
    double ym = h * (1 - r) * 0.5;
    for (int i = 0; num_str[i] != '\0'; ++i) {
        char c = num_str[i];
        int n;
        if (c >= '0' && c <= '9') {
            n = c - '0';
        }
        else if (c == '.') {
            n = 10;
        }
        else if (c == '-') {
            n = 11;
        }
        else {
            continue; // �s���ȕ����͖���
        }
        draw_digit2(hdc, n, x + w * i + xm, y + ym, w2, h2, color);
    }
}
