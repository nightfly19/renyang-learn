class Point2D { 
public: 
    Point2D();
    Point2D(int, int);
    int x() {return _x;} 
    int y() {return _y;} 
    Point2D operator+(const Point2D&); // 重載+運算子 
    Point2D operator-(const Point2D&); // 重載-運算子 
    Point2D& operator++(); // 重載++前置，例如 ++p 
    Point2D operator++(int); // 重載++後置，例如 p++
    Point2D& operator--(); // 重載--前置，例如 --p 
    Point2D operator--(int); // 重載--後置，例如 p--
 
private:
    int _x;
    int _y; 
};
