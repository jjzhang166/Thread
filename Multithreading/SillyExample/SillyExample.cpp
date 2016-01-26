// SillyExample.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <process.h>         // needed for _beginthread()

void  silly(void *);   // function prototype

int main()
{
    // Our program's first thread starts in the main() function.

    printf("Now in the main() function.\n");

    // Let's now create our second thread and ask it to start
    // in the silly() function.

    _beginthread(silly, 0, (void*)12);

    // From here on there are two separate threads executing
    // our one program.

    // This main thread can call the silly() function if it wants to.

    silly((void*)-5);

    Sleep(100);
}

void  silly(void *arg)
{
    printf("The silly() function was passed %d\n", (INT_PTR)arg);
}

