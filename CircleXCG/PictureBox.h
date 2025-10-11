#pragma once
#include "wintox.h"
#include "ClassFigureList.h"
#include <string>

#ifdef USE_X68000
#include "drawasm.h"
#include "xdraw.h"
#endif

class PictureBox
{
private:
    int p_r = 2;
    ClassFigureList flist;
    std::wstring text;

public:
    void Line(HDC hdc, double sx, double sy, double ex, double ey) {
        // 直線の描画
#ifdef USE_X68000
        drawlinex((int)sx, (int)sy, (int)ex, (int)ey, 1);
#else
        MoveToEx(hdc, sx, sy, NULL);
        LineTo(hdc, ex, ey);
#endif
    }

    void Circle(HDC hdc, double x, double y, double r) {
		//printf("Circle: x=%f, y=%f, r=%f\n", x, y, r);
#ifdef USE_X68000
        drawellipsex((int)(x - r), (int)(y - r), (int)(r * 2), (int)(r * 2), 2);
#else
        Ellipse(hdc, x - r, y - r, x + r, y + r);
#endif
    }

    void Cls(HWND hWnd, HDC hdc) {
        // クライアント領域のサイズを取得
        RECT rect;
        GetClientRect(hWnd, &rect);

#ifdef USE_X68000
        //wipe();
        fillboxx((int)(rect.left), (int)(rect.top), (int)(rect.right - rect.left), (int)(rect.bottom - rect.top), 3);
        //fillboxx((int)(rect.left), (int)(rect.top), (int)((rect.right - rect.left) / 2), (int)((rect.bottom - rect.top) / 2), 4);
#else
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
#endif
    }

    void Paint(HWND hWnd)
    {
        // クライアント領域のサイズを取得
        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        long w = width;
        long h = height;
        long ox = w / 2;
        long oy = h / 2;
        double csize = (ox < oy) ? ox * 0.8 : oy * 0.8;
		//printf("w=%d, h=%d, ox=%d, oy=%d, csize=%f\n", w, h, ox, oy, csize);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // ペンとブラシの作成
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        SelectObject(hdc, hPen);
        SelectObject(hdc, hBrush);

        Cls(hWnd, hdc);

        // 塗りつぶしなしに戻す
        HBRUSH hNoBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        SelectObject(hdc, hNoBrush);

        Circle(hdc, ox, oy, csize);

        int cnt = flist.GetCount();
        for (int i = 0; i < cnt; i++) {
            double x1 = flist.getX(i);
            double y1 = flist.getY(i);
            double cx1 = x1 * csize;
            double cy1 = y1 * csize;

            Circle(hdc, ox + cx1, oy - cy1, p_r);

            if (i + 1 < cnt) {
                double x2 = flist.getX(i + 1);
                double y2 = flist.getY(i + 1);
                double cx2 = x2 * csize;
                double cy2 = y2 * csize;
                Line(hdc, ox + cx1, oy - cy1, ox + cx2, oy - cy2);
            }
            else if (cnt >= 3) {
                double x2 = flist.getX(0);
                double y2 = flist.getY(0);
                double cx2 = x2 * csize;
                double cy2 = y2 * csize;
                Line(hdc, ox + cx1, oy - cy1, ox + cx2, oy - cy2);
            }
        }

        // テキストの描画
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT); // 背景を透明に
        TextOut(hdc, 10, 10, text.c_str(), wcslen(text.c_str()));

        // リソースの解放
        DeleteObject(hPen);
        DeleteObject(hBrush);

        EndPaint(hWnd, &ps);
    }

    double triarea(double x1, double y1, double x2, double y2) {
        return -(x1 * y2 - x2 * y1) / 2;
    }

    double getarea() {
        int cnt = flist.GetCount();
        double a = 0;
        for (int i = 0; i < cnt; i++) {
            double x1 = flist.getX(i);
            double y1 = flist.getY(i);
            if (i + 1 < cnt) {
                double x2 = flist.getX(i + 1);
                double y2 = flist.getY(i + 1);
                a += triarea(x1, y1, x2, y2);
            }
            else if (cnt >= 3) {
                double x2 = flist.getX(0);
                double y2 = flist.getY(0);
                a += triarea(x1, y1, x2, y2);
            }
        }
        return a;
    }

#ifdef USE_X68000
    void InvalidateRect(HWND hWnd, const RECT* lpRect, BOOL bErase) {
        Paint(hWnd);
    }
#endif

    void MouseDown(HWND hWnd, int x, int y)
    {
        // クライアント領域のサイズを取得
        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        long w = width;
        long h = height;
        long ox = w / 2;
        long oy = h / 2;
        double csize = (ox < oy) ? ox * 0.8 : oy * 0.8;

        double r = sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy));
        if (fabs(r - csize) < 10) {
            flist.AddPoint((x - ox) / r, -(y - oy) / r);
            text = std::to_wstring(getarea());
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }

    void AddByDeg(HWND hWnd, int deg) {
        // 指定された角度で点を追加
        double rad = deg * 3.14159265358979323846 / 180.0; // 角度をラジアンに変換
        double x = cos(rad);
        double y = sin(rad);
        flist.AddPoint(x, y);
        text = std::to_wstring(getarea());
        InvalidateRect(hWnd, NULL, TRUE);
    }

    void Divide(HWND hWnd, int n) {
        // 指定された個数に分割
        flist.Divide(n);
        text = std::to_wstring(getarea());
        InvalidateRect(hWnd, NULL, TRUE);
    }
};

