#pragma once
template <typename T> class List {
	T* arr;
	int count;
	int maxCount;
public:
	int Count()
	{
		return count;
	}
	List()
	{
		arr = new T[1];
		maxCount = 1;
		count = 0;
	}
	T Add(T newObject)
	{
		if (subscribersCount == maxSubscribersCount)
		{
			maxSubscribersCount *= 2;
			auto buf = new Action[maxSubscribersCount];
			int i = 0;
			for (; i < subscribersCount; i++)
				buf[i] = funcArray[i];
			for (; i < maxSubscribersCount; i++)
				buf[i] = NULL;
			delete[] funcArray;
			funcArray = buf;
			Subscribe(funcPointer);
		}
		else
		{
			funcArray[subscribersCount] = funcPointer;
			subscribersCount++;
		}
	}
};