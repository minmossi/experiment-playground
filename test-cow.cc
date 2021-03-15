#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace std;

int main()
{
    int pid;

    unsigned char *buf = (unsigned char *)malloc(4194304);
    for (int i = 0; i < 4194304; i++)
        buf[i] = rand() % 256;
    unsigned char *buf2 = (unsigned char *)malloc(4194304);
    unsigned char *buf3 = (unsigned char *)malloc(4194304);

    // 2-copy Test
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    memcpy(buf2, buf, 4194304);
    memcpy(buf3, buf2, 4194304);
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
    cout << "2-copy Test: " << sec.count() << "s" << endl;

    unsigned char *buf4 = (unsigned char *)malloc(4194304);
    memcpy(buf4, buf3, 4194304);

    // 2-copy Write Test
    start = std::chrono::system_clock::now();
    memset(buf3, 'a', 4194304);
    sec = std::chrono::system_clock::now() - start;
    cout << "2-copy Write Test: " << sec.count() << "s" << endl;

    buf3[4194303] = 0;
    fprintf(stderr, "%s", (char *)buf3);

    // CoW Test
    start = std::chrono::system_clock::now();
    if (pid = fork()) // 부모
    {
        exit(0);
    }
    else // 자식
    {
    }
    sec = std::chrono::system_clock::now() - start;
    cout << "CoW Test: " << sec.count() << "s" << endl;

    // CoW Write Test
    start = std::chrono::system_clock::now();
    memset(buf4, 'a', 4194304);
    sec = std::chrono::system_clock::now() - start;
    cout << "CoW Write Test: " << sec.count() << "s" << endl;
    buf3[4194303] = 0;
    fprintf(stderr, "%s", (char *)buf4);

    return 0;
}