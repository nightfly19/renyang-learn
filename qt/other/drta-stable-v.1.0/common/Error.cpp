#include "Error.h"

Error::Error(QString s)
	: text(s)
{
}

Error::Error(char *s)
	: text(s)
{
}

Error::Error(error_type type)
{
	switch (type)
	{
		case DRTA_ERR_MEMORY:
			text = QString("not enough memory!");
			break;
		default:
			text = QString("Unkown error");
			break;
	}
}

Error::~Error()
{
}

QString& Error::getText(void)
{
	return text;
}
