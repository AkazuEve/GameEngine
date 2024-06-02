#include <iostream>
#include <chrono>

#define DEBUGPRINT(x) std::cout << x << std::endl;
#define DEBUGVAR(x) x
#define DEBUGFUNC(x) x
#define TIMEDFUNC(x, y) std::chrono::steady_clock::time_point start; std::chrono::steady_clock::time_point end; start = std::chrono::high_resolution_clock::now(); y end = std::chrono::high_resolution_clock::now(); std::cout << x <<" <" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms><" << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us>" << std::endl;

