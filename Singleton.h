#pragma once
#define SINGLETON(T) friend Singleton<T>; private: T() = default;

template <typename T>
class Singleton
{
public:
	Singleton(const Singleton&) = delete;
	void operator=(const Singleton&) = delete;

	static T& Instance()
	{
		static T instance;

		return instance;
	}

protected:
	Singleton() {};

};

