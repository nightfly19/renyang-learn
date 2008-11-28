class PointDemo {
public:
    PointDemo(int);
    ~PointDemo();
 
    void show();
private:

    // Nested Class
    class Point;
 
    Point **_points;
    int _length;
};
