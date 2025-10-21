#pragma once
#include "wintox.h"
#include "ClassFigureList.h"
#include <string>

#ifdef USE_X68000
#include "drawasm.h"
#include "xdraw.h"
#endif

#include "drawdigit.h"

COLORREF xpalette_to_rgb(int c) {
    switch (c) {
    case 0: return RGB(0, 0, 0);       // 黒
    case 1: return RGB(128, 128, 128); // 灰色
    case 2: return RGB(0, 0, 128);     // 青
    case 3: return RGB(0, 0, 255);     // 明るい青
    case 4: return RGB(128, 0, 0);     // 赤
    case 5: return RGB(255, 0, 0);     // 明るい赤
    case 6: return RGB(128, 0, 128);   // 紫
    case 7: return RGB(255, 0, 255);   // 明るい紫
    case 8: return RGB(0, 128, 0);     // 緑
    case 9: return RGB(0, 255, 0);     // 明るい緑
    case 10: return RGB(0, 128, 128);  // 水色
    case 11: return RGB(0, 255, 255);  // 明るい水色
    case 12: return RGB(128, 128, 0);  // 黄
    case 13: return RGB(255, 255, 0);  // 明るい黄
    case 14: return RGB(192, 192, 192);// 白
    case 15: return RGB(255, 255, 255);// 明るい白
    default: return RGB(0, 0, 0);      // 黒（デフォルト）
    }
}

class PictureBox
{
private:
    int p_r = 3;
    ClassFigureList flist;
    std::string number_text;
    std::string number_text_old;
    int text_color = 3;
    int circle_color = 9;
    int point_color = 7;
    int polygon_color = 11;

public:
    std::string get_number_text() {
        return number_text;
    }
    void set_number_text(std::string text) {
        number_text_old = number_text;
        number_text = text;
	}
    bool is_number_text_changed() {
        return number_text != number_text_old;
	}

public:
    void Line(HDC hdc, double sx, double sy, double ex, double ey) {
        // 直線の描画
#ifdef USE_X68000
        drawlinex((int)sx, (int)sy, (int)ex, (int)ey, polygon_color);
#else
        HPEN hPen = CreatePen(PS_SOLID, 1, xpalette_to_rgb(polygon_color));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        MoveToEx(hdc, sx, sy, NULL);
        LineTo(hdc, ex, ey);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
#endif
    }

    void Circle(HDC hdc, double x, double y, double r) {
#ifdef USE_X68000
        // X68000 で円になるように補正
        double rx = r / 1.5;
        drawellipsex((int)(x - rx), (int)(y - r), (int)(rx * 2), (int)(r * 2), circle_color);
#else
        HPEN hPen = CreatePen(PS_SOLID, 1, xpalette_to_rgb(circle_color));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        Ellipse(hdc, x - r, y - r, x + r, y + r);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
#endif
    }

    void DrawPoint(HDC hdc, double x, double y, double r) {
#ifdef USE_X68000
        // X68000 で円になるように補正
        double rx = r / 1.5;
        fillellipsex((int)(x - rx), (int)(y - r), (int)(rx * 2), (int)(r * 2), point_color);
#else
        HPEN hPen = CreatePen(PS_SOLID, 1, xpalette_to_rgb(point_color));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        HBRUSH hBrush = CreateSolidBrush(xpalette_to_rgb(point_color));
        HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);
        Ellipse(hdc, x - r, y - r, x + r, y + r);
        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hBrush);
        DeleteObject(hPen);
#endif
    }

    void Cls(HWND hWnd, HDC hdc) {
        // クライアント領域のサイズを取得
        RECT rect;
        GetClientRect(hWnd, &rect);

#ifdef USE_X68000
        wipe();
#else
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
#endif
    }

    void FillPolygon(HDC hdc, POINT* pts, int count) {
        // 直線の描画
#ifdef USE_X68000
		if (count < 3) return;
        // 三角形の重心を求める
        int n = count / 3;
        int x = (pts[0].x + pts[n].x + pts[n * 2].x) / 3;
		int y = (pts[0].y + pts[n].y + pts[n * 2].y) / 3;
		paintx(x, y, polygon_color);
#else
        HPEN hPen = CreatePen(PS_SOLID, 1, xpalette_to_rgb(polygon_color));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        HBRUSH hBrush = CreateSolidBrush(xpalette_to_rgb(polygon_color));
        HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);
        Polygon(hdc, pts, count);
        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hBrush);
        DeleteObject(hPen);
#endif
    }

    void DrawText(HDC hdc, std::string text, int x, int y, int w, int h, int color) {
		//std::string text = WstringToString(wtext);
		draw_num_str2(hdc, text.c_str(), x, y, w, h, color);
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
#ifdef USE_X68000
		double xsize = csize / 1.5;
#else
		double xsize = csize;
#endif
		double ysize = csize;

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
		POINT* pts = new POINT[cnt];
        for (int i = 0; i < cnt; i++) {
            double x1 = flist.getX(i);
            double y1 = flist.getY(i);
            double cx1 = x1 * xsize;
            double cy1 = y1 * ysize;

            pts[i].x = (LONG)(ox + cx1);
            pts[i].y = (LONG)(oy - cy1);

            if (i + 1 < cnt) {
                double x2 = flist.getX(i + 1);
                double y2 = flist.getY(i + 1);
                double cx2 = x2 * xsize;
                double cy2 = y2 * ysize;
#ifdef USE_X68000
                Line(hdc, ox + cx1, oy - cy1, ox + cx2, oy - cy2);
#endif
            }
            else if (cnt >= 3) {
                double x2 = flist.getX(0);
                double y2 = flist.getY(0);
                double cx2 = x2 * xsize;
                double cy2 = y2 * ysize;
#ifdef USE_X68000
                Line(hdc, ox + cx1, oy - cy1, ox + cx2, oy - cy2);
#endif
            }
        }

		FillPolygon(hdc, pts, cnt);
		delete[] pts;

        for (int i = 0; i < cnt; i++) {
            double x1 = flist.getX(i);
            double y1 = flist.getY(i);
            double cx1 = x1 * xsize;
            double cy1 = y1 * ysize;

            DrawPoint(hdc, ox + cx1, oy - cy1, p_r);
        }

        // テキストの描画
        //SetTextColor(hdc, RGB(0, 0, 0));
        //SetBkMode(hdc, TRANSPARENT); // 背景を透明に
        //TextOut(hdc, 10, 10, text.c_str(), wcslen(text.c_str()));
        DrawText(hdc, get_number_text(), 10, 10, 16, 32, text_color);

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

    bool MouseDown(HWND hWnd, int x, int y)
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

#ifdef USE_X68000
        x = ox + (x - ox) * 1.5;
#endif

        double r = sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy));
        if (fabs(r - csize) < 10) {
            flist.AddPoint((x - ox) / r, -(y - oy) / r);
            set_number_text(std::to_string(getarea()));
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (fabs(r - csize) > csize / 8) {
			// クリック領域外のときは終了
            return false;
        }
		return true;
    }

    void AddByDeg(HWND hWnd, int deg) {
        // 指定された角度で点を追加
        double rad = deg * 3.14159265358979323846 / 180.0; // 角度をラジアンに変換
        double x = cos(rad);
        double y = sin(rad);
        flist.AddPoint(x, y);
        set_number_text(std::to_string(getarea()));
        InvalidateRect(hWnd, NULL, TRUE);
    }

    void Divide(HWND hWnd, int n) {
        // 指定された個数に分割
        flist.Divide(n);
        set_number_text(std::to_string(getarea()));
        InvalidateRect(hWnd, NULL, TRUE);
    }
};

