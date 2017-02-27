#include <iostream>
#include <thread>
#include <future>
//#include <exception>

using std::thread;
using std::cout;
using std::promise;
using std::future;
using std::exception_ptr;

void f(promise<int> px)
{
	//px.set_value(5);
	try	{	throw 7.7;	}
	catch(double e)
	{
		exception_ptr ep = std::current_exception();
		px.set_exception(ep);
	}
	//px.set_exception(7.7);
}

int main()
{
	promise<int> p;
	future<int> vx = p.get_future();
	thread t(f, std::move(p));

	vx.wait();

	try
	{
		int v = vx.get();
		cout << v << std::endl;
	}
	catch(double e)
	{
		cout << "exception: " << e << std::endl;
	}

	t.join();
}
