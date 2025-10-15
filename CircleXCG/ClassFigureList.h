#pragma once
#include <vector>

class ClassFigure
{
public:
    double x, y;
};

class ClassFigureList
{
private:
    std::vector<ClassFigure> list;

public:
    int GetCount() const {
        return static_cast<int>(list.size());
    }

    int FindInsIndex(double x, double y) {
        size_t i;
        for (i = 0; i < list.size(); i++) {
            const ClassFigure& e = list[i];
            if (y <= 0) {
                // top half
                if (e.y > 0) {
                    return static_cast<int>(i);
                }
                else if (e.x == x) {
                    return -1;
                }
                else if (e.x < x) {
                    return static_cast<int>(i);
                }
            }
            else {
                // bottom half
                if (e.y <= 0) {
                    continue;
                }
                else if (e.x == x) {
                    return -1;
                }
                else if (e.x > x) {
                    return static_cast<int>(i);
                }
            }
        }
        return static_cast<int>(i);
    }

    void AddPoint(double x, double y) {
        ClassFigure e;
        e.x = x;
        e.y = y;
        int index = FindInsIndex(x, y);
        if (index != -1) {
            list.insert(list.begin() + index, e);
        }
    }

    void Clear() {
        list.clear();
    }

    double getX(int i) const {
        return list[i].x;
    }

    double getY(int i) const {
        return list[i].y;
    }

    void Divide(int n) {
        // w’è‚³‚ê‚½ŒÂ”‚É•ªŠ„
        std::vector<ClassFigure> add_list;
        for (int i = 0; i < list.size(); i++) {
			double b = atan2(list[i].y, list[i].x);
			int j = (i + 1) % list.size();
            double a = atan2(list[j].y, list[j].x);
			if (b <= a) {
				b += 2 * 3.14159265358979323846; // Šp“x‚ğ³‹K‰»
			}
            for (int k = 1; k < n; k++) {
                double angle = a + (b - a) * k / n;
                double x = cos(angle);
                double y = sin(angle);
				add_list.push_back(ClassFigure{ x, y });
			}
        }
        for (const auto& e : add_list) {
            AddPoint(e.x, e.y);
		}
    }
};
