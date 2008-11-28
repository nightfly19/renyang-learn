class Integer {
public:
    Integer(int value) : _value(value) {
    }
 
    int value() {
        return _value;
    }
 
    // 自訂型態轉換
    // 當需要將Integer轉換為int時如何執行 
    operator int() {
        return _value;
    }
 
    int compareTo(Integer);
 
private:
    int _value;
};
