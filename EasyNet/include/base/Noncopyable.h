#ifndef NONCOPYABLE_H__
#define NONCOPYABLE_H__

class Noncopyable
{
protected:
	Noncopyable(){}
    ~Noncopyable() {}
private:
	Noncopyable(const Noncopyable&);
	Noncopyable& operator = (const Noncopyable&);
};


#endif