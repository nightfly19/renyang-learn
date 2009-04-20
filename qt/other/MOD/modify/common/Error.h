
#ifndef ERROR_H
#define ERROR_H

#include <qstring.h>

class Error {
public:
	enum error_type { DRTA_ERR_MEMORY = 1 }; 
	
	Error(QString);
	Error(char*);
	Error(error_type);
	~Error();
	QString& getText(void);

private:
	QString text;
};

#endif

