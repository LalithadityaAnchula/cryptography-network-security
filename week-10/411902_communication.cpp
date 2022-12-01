#include <bits/stdc++.h>
using namespace std;
int main()
{
    vector<int> v1(6, 0), v2(6, 0);
    v1[0] = 0;
    v1[1] = 2;
    v1[2] = 3;
    v1[3] = 3;
    v1[4] = 0;
    v1[5] = 4;
    v1[6] = 4;
    v1[7] = 5;
    v1[8] = 0;
    v2[0] = 0;
    v2[1] = -1;
    v2[2] = -1;
    v2[3] = 1;
    v2[4] = 0;
    v2[5] = -1;
    v2[6] = +1;
    v2[7] = 1;
    v2[8] = 0;
    while (true)
    {
        int a, b;
        cout << "Enter sender's id : ";
        cin >> a;
        cout << "enter receiver's id : ";
        cin >> b;
        int k = 0;
        if (a == 1)
        {
            k = 3;
        }
        if (a == 2)
        {
            k = 6;
        }
        string s, z, y;
        cout << "Enter Plain Text";
        cin >> s;

        for (int i = 0; i < s.length(); i++)
        {
            z = z + char((((s[i] - 65) + v1[k + b]) % 26) + 65);
        }
        cout << "Cipher Text  " << z << endl;
        for (int i = 0; i < 3; i++)
        {
            int j = i;
            if (i == 1)
            {
                j = 3;
            }
            if (i == 2)
            {
                j = 6;
            }
            if (i != a)
            {
                if (i == b)
                {
                    cout << "Text at id" << i << "is" << s << endl;
                }
                else
                {
                    for (int i = 0; i < s.length(); i++)
                    {
                        y = y + char((((z[i] - 65) - v1[j + a] + 26) % 26) + 65);
                    }
                    cout << "Text at id" << i << "is" << y << endl;
                }
            }
        }
    }
    return 0;
}
