class Ball;

int compare(Ball&, Ball&);

class Ball { 
public: 
    Ball(double, char*); 
 
    double radius() {
        return _radius;
    }
 
    char* name() {
        return _name; 
    }
 
    void radius(double radius) {
        _radius = radius;
    } 
 
    void name(char *name) {
        _name = name;
    }
 
    // 宣告朋友函式 
    friend int compare(Ball&, Ball&);
 
private:
    double _radius; // 半徑 
    char *_name; // 名稱 
};
