class PointDemo::Point {
public:
    Point();
    Point(int, int);
    int x() { return _x; }
    int y() { return _y; }
    void x(int x) { _x = x; }
    void y(int y) { _y = y; }
private:
    int _x;
    int _y;
};
