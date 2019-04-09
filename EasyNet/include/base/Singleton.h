#ifndef SINGLETON_H__
#define SINGLETON_H__

#include "EasyNet/include/base/MutexLock.h"


template<class T>
class Singleton
{
public:
	Singleton()
	{

	}
	static T* GetInstance()
	{
		if (m_pInstance == NULL)  //double check
		{
			static base::MutexLock mutex;
            {
                base::LockGuard lk(mutex);
                if (m_pInstance == NULL)
                {
                    m_pInstance = new T();
                }
            }
		}
		return m_pInstance;
	}
private:
	static T* m_pInstance;
};

template<class T>
T* Singleton<T>::m_pInstance = NULL;


#endif
