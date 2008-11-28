
class Point2D {
	public:
		Point2D();
		Point2D(int,int);

		friend Point2D operator+(const Point2D &,int);
		friend Point2D operator+(int, const Point2D &);
		friend Point2D operator++(Point2D &);
		friend Point2D operator++(Point2D &,int);

		const int get_x() {
			return _x;
		}

		const int get_y() {
			return _y;
		}

	private:
		int _x;
		int _y;
};

