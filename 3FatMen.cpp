#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <time.h>

std::mutex output_mutex;
void printText(std::wstring s) //вывод текста
{
	output_mutex.lock();
	std::wcout << s << std::endl;
	output_mutex.unlock();
}

std::mutex outpul_mutex;
void printTextNum(std::wstring s, int num) //вывод текста и числа
{
	outpul_mutex.lock();
	std::wcout << s << num << std::endl;
	outpul_mutex.unlock();
}

class Cook
{
public:
	int Cooking() //возвращает коэф-т производительности 
	{
		return efficiency_factor;
	}
	bool getTired() //вышло ли время
	{
		bool current_state;
		tired_mutex.lock();
		current_state = mTired;
		tired_mutex.unlock();
		return current_state;
	}
	void setTired(bool state)
	{
		tired_mutex.lock();
		mTired = state;
		tired_mutex.unlock();
	}
private:
	const int efficiency_factor = 7000;
	bool mState = true;
	bool mTired = false;
	std::mutex tired_mutex;
};

class FatMan
{
public:
	FatMan(int somegluttony, std::wstring name_) //конструктор толстяка
	{
		gluttony = somegluttony;
		getgluttony = somegluttony;
		name = name_;
	}
	std::wstring FatManName() { //возвращает имя толстяка
		return name;
	}
	void FatManDied() //если толстяк взрывается, он перестает есть
	{
		gluttony = 0;
	}
	int getGluttony() { //возвращает коэф-т прожорливости
		return getgluttony;
	}
	void Eating() //поедание
	{
		eating_mutex.lock();
		dish -= gluttony;
		counter += gluttony;
		eating_mutex.unlock();
	}
	int getFood() //сколько еды осталось на тарелке
	{
		int current_dish;
		food_mutex.lock();
		current_dish = dish;
		food_mutex.unlock();
		return current_dish;
	}
	void setFood(int food) //добавление еды в тарелку
	{
		food_mutex.lock();
		dish += food;
		food_mutex.unlock();
	}
	int Counter() //счетчик сколько уже съедено 
	{
		return counter;
	}
	bool getState() //поел ли толстяк
	{
		bool current_state;
		mutex.lock();
		current_state = mState;
		mutex.unlock();
		return current_state;
	}
	void setState(bool state)
	{
		mutex.lock();
		mState = state;
		mutex.unlock();
	}
private:
	int gluttony;
	int getgluttony;
	int dish = 3000;
	int counter = 0;
	bool mState = true;
	std::mutex food_mutex;
	std::mutex mutex;
	std::mutex eating_mutex;
	std::wstring name;
};

std::mutex cook_mutex;
std::mutex fatman_mutex;
Cook cook;
FatMan fatman1(1, L"First FatMan exploded!");
FatMan fatman2(2, L"Second FatMan exploded!");
FatMan fatman3(3, L"Third FatMan exploded!");
int exploded = 0; //сколько тостяков взорвалось
bool fired = false; //уволен ли повар

void CookIsCooking()
{
	clock_t start = clock();
	double seconds = 0;
	while (!cook.getTired()) //пока не прошло 5 сек
	{
		if (fatman1.getState() && fatman2.getState() && fatman3.getState()) //пока все толстяки не поедят, повар не готовит
		{
			cook_mutex.lock();
			fatman1.setFood(cook.Cooking());
			fatman2.setFood(cook.Cooking());
			fatman3.setFood(cook.Cooking());
			//printTextNum(L"dish1 ", fatman1.getFood());
			//printTextNum(L"dish2 ", fatman2.getFood());
			//printTextNum(L"dish3 ", fatman3.getFood());
			//printTextNum(L"FATMAN3", fatman3.Counter());
			fatman1.setState(false);
			fatman2.setState(false);
			fatman3.setState(false);
			cook_mutex.unlock();
		}
		clock_t end = clock();
		seconds = (double)(end - start) / CLOCKS_PER_SEC;
		if (seconds >= 5) //через 5 сек повар устал
		{
			cook.setTired(true);
			printText(L"Cook is tired!");
			printTextNum(L"efficiency_factor: ", cook.Cooking());
			printTextNum(L"First FatMan gluttony:", fatman1.getGluttony());
			printTextNum(L"Second FatMan gluttony:", fatman2.getGluttony());
			printTextNum(L"Third FatMan gluttony:", fatman3.getGluttony());
		}
	}
}

void FatManIsEating(FatMan& fatman)
{
	while (!cook.getTired() && fatman.getFood() > 0 && fatman.Counter() < 10000 && !fired) //пока не прошло 5 сек, не кончилась еда, толстяк не переел, повар не уволен
	{
		if (!fatman.getState()) //если толстяк не ел, он может есть
		{
			fatman_mutex.lock();
			fatman.Eating();
			fatman.setState(true);
			fatman_mutex.unlock();
		}
		if (fatman.Counter() >= 10000 && fatman.getFood() >= 0) //если толстяк переел, но еда на тарелке еще есть, он взрывается
		{
			printText(fatman.FatManName());
			exploded++;
			fatman.FatManDied(); //перестает есть
			if (exploded == 3) //если все толстяки взорвались, то повар не получит зарплату
			{
				printText(L"Cook'll not receive a salary!");
				printTextNum(L"efficiency_factor: ", cook.Cooking());
				printTextNum(L"First FatMan gluttony:", fatman1.getGluttony());
				printTextNum(L"Second FatMan gluttony:", fatman2.getGluttony());
				printTextNum(L"Third FatMan gluttony:", fatman3.getGluttony());
				cook.setTired(true);
			}
		}
		if (fatman.getFood() < 0 && !fired) //если повар не успел накормить толстяков, и он еще не уволеен, его увольняют
		{
			fired = true;
			printText(L"Cook is fired!");
			printTextNum(L"efficiency_factor: ", cook.Cooking());
			printTextNum(L"First FatMan gluttony:", fatman1.getGluttony());
			printTextNum(L"Second FatMan gluttony:", fatman2.getGluttony());
			printTextNum(L"Third FatMan gluttony:", fatman3.getGluttony());
			cook.setTired(true);
		}
	}
}

int main()
{
	std::thread t1(CookIsCooking);
	std::thread t2(FatManIsEating, std::ref(fatman1));
	std::thread t3(FatManIsEating, std::ref(fatman2));
	std::thread t4(FatManIsEating, std::ref(fatman3));
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}