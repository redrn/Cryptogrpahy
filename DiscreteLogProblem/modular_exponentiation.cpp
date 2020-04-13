#include <iostream>
#include <cmath>
using namespace std;

/** Perform fast modular exponentiation using the square method, recursive */
int fast_mod_expo_square(int g, int e, int N)
{
    if(e != 1)
    {
        return (int)nearbyint(pow(fast_mod_expo_square(g, e / 2, N), 2)) % N;
    }
    else
    {
        return g % N;
    }
}

/** Perform fast modular exponentiation using the multiply method */
int fast_mod_expo_multiply()
{

}

// int mod_expo(int g, int e, int N)
// {
    
// }

int main()
{
    cout << "Enter g(base), e(exponent), and N(modulas) separated by white spaces: ";

    int g, e, N;
    cin >> g >> e >> N;

    cout << fast_mod_expo_square(g, e, N) << endl;

    return 0;
}