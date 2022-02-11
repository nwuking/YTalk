/**
 * @file Singleton.h
 * @author nuwking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_SINGLETON_H
#define YTALK_SINGLETON_H

namespace YTalk
{

namespace base
{

template<typename T>
class Singleton
{
public:
    /**
     * @brief 获取对象的唯一接口
     * 
     * @return T& 
     */
	static T& getInstance()
	{
		if (nullptr == m_value)
		{
			m_value = new T();
		}
		return *m_value;
	}

private:
	Singleton();
	~Singleton() = default;

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static void destroy()
	{
		delete m_value;
	}

private:
	static T*             m_value;
};

template<typename T>
T* Singleton<T>::m_value = nullptr;

}  //namespace base

}   // namespace YTalk

#endif //YTALK_SINGLETON_H