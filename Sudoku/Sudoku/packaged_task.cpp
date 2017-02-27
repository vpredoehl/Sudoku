//g++ packaged_task.cpp -std=c++11  -o packaged_task

#include <iostream>
#include <thread>
#include <future>
//#include <exception>

using std::thread;
using std::cout;
using std::promise;
using std::future;
using std::exception_ptr;
using std::packaged_task;
using std::async;

auto add(int a, int b) -> int	{	return a+b;	}
auto throw_test(int a, int b) -> int	{	throw std::string("throwing a string");	}
auto square(int i) -> double   {   return i*i; }

void f(promise<int> px)
{
	//px.set_value(5);
	try	{	throw 7.7;	}
	catch(double e)
	{
		exception_ptr ep = std::current_exception();
		px.set_exception(ep);
	}
}

int main()
{
	promise<int> p;
	future<int> vx = p.get_future();
	thread t {f, std::move(p)};

	try
	{
		int v = vx.get();
		cout << v << std::endl;
	}
	catch(double e)
	{
		cout << "exception: " << e << std::endl;
	}

	packaged_task<int(int,int)> pt {/* add */ throw_test};
	future<int> result = pt.get_future();
	thread ptt { move(pt), 3, 5};

	try	{	int v = result.get();	cout << "result: " << v << std::endl;	}
	catch(std::string s)	{	cout << "string: " << s << std::endl;	}
    
    future<double> sqr = async(square, 4);
    cout << "square: " << sqr.get() << std::endl;

	t.join();
	ptt.join();
}
