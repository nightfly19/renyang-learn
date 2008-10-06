
#ifndef __BOUNCYLOGO_H__
#define __BOUNCYLOGO_H__

#include <qcanvas.h>

class BouncyLogo : public QCanvasSprite 
{
public:
    virtual void initPos();
    virtual void initSpeed(); 

    BouncyLogo(QCanvas*);
    void advance(int);
    virtual int rtti() const;
};


#endif

