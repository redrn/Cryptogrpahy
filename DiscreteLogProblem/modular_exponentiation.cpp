#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

/** Perform fast modular exponentiation using the square method, recursive */
int fast_mod_expo_square(int e, int N, int init_e, int init_val)
{
    if(e != init_e)
    {
        return (int)nearbyint(pow(fast_mod_expo_square(e / 2, N, init_e, init_val), 2)) % N;
    }
    else
    {
        return init_val;
    }
}

/** Perform fast modular exponentiation using the multiply method */
int fast_mod_expo_multiply(int e, int N, int init_e, int init_val)
{
    if(e != init_e)
    {
        return (fast_mod_expo_multiply(e - 1, N, init_e, init_val) * init_val) % N;
    }
    else
    {
        return init_val;
    }
}

/** Data structure to store information of exponentiation operation */
struct Operation
{
    string method;
    int begin, end;
};

vector<Operation> operations;

/** Find the most efficient way of exponentiation using multiply and square method
 *  multiply -> square -> multiply
*/
void find_optimized(int e)
{
    int num_of_operations = -1, lowest_base = 0, square_end = 0, lowest_end = 0, lowest = 0;
    for (int base = 1; base < 100; base+=2)
    {
        num_of_operations = -1;
        for (int num = base; num <= e; num*=2)
        {
            num_of_operations++;
            if(num*2 >= e)
            {
                num_of_operations += (base - 1) + (e - num);
                square_end = num;
            }
        }
        if(lowest == 0 || num_of_operations<lowest)
        {
            lowest = num_of_operations;
            lowest_base = base;
            lowest_end = square_end;
        }
    }

    cout << lowest << endl;
    operations.push_back(Operation{"m", 1, lowest_base});
    operations.push_back(Operation{"s", lowest_base, lowest_end});
    operations.push_back(Operation{"m", lowest_end, e});
}

int main()
{
    cout << "Enter g(base), e(exponent), and N(modulas) separated by white spaces: " << endl;

    int g, e, N;
    //cin >> g >> e >> N;

    find_optimized(1);
    for(auto &x : operations)
    {
        cout << x.method << " " << x.begin << " " << x.end << endl;
    }

    return 0;
}