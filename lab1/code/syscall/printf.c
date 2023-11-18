#include "mini-lib.h"

int hex2Str(unsigned int num, char *buffer, int count)
{
    char hex[16] = "0123456789abcdef";
    buffer[count++] = '0';
    buffer[count++] = 'x';
    if (num == 0)
    {
        buffer[count++] = '0';
        return count;
    }

    int k = 0, arr[32];
    while (num != 0)
    {
        arr[k++] = num & 0xf;
        num >>= 4;
    }

    for (int i = (k - 1); i >= 0; i--)
    {
        buffer[count++] = hex[arr[i]];
    }

    return count;
}

int dec2Str(int num, char *buffer, int count)
{
    if (num == 0)
    {
        buffer[count++] = '0';
        return count;
    }
    else if (num == (int)-2147483648)
    {
        const char *str = "-2147483648";
        while (*str != 0)
        {
            buffer[count++] = *str;
            str++;
        }
        return count;
    }
    else if (num < 0)
    {
        buffer[count++] = '-';
        num = -num;
    }
    int i = 0, arr[100];
    while (num != 0)
    {
        arr[i++] = num % 10;
        num /= 10;
    }

    char dec[10] = "0123456789";
    for (int j = i - 1; j >= 0; j--)
    {
        buffer[count++] = dec[arr[j]];
    }
    return count;
}
void print_buffer(int *ret, int *count, char *buffer)
{
    buffer[*count] = '\0';
    puts(buffer);
    *ret += *count;
    *count = 0;
}
#include <stdarg.h>
int printf(const char *format, ...)
{
    // Your code here:
    // TODO();
    const int MAX_BUFFER_SIZE = 1024;
    va_list ap;
    va_start(ap, format);
    int i = 0, ret = 0; // format index
    char buffer[MAX_BUFFER_SIZE + 100];
    int count = 0; // buffer index

    while (format[i] != '\0')
    {
        buffer[count++] = format[i];
        if (format[i] == '%')
        {
            count--;
            i++;
            switch (format[i])
            {
            case 'c':
                char character = va_arg(ap, int);
                buffer[count++] = character;
                break;
            case 's':
                print_buffer(&ret, &count, buffer);
                const char *string = va_arg(ap, char *);
                ret += strlen(string);
                puts(string);
                break;
            case 'x':
                unsigned int hexadecimal = va_arg(ap, unsigned int);
                count = hex2Str(hexadecimal, buffer, count);
                break;
            case 'd':
                int decimal = va_arg(ap, int);
                count = dec2Str(decimal, buffer, count);
                break;
            case '%':
                count++;
                break;
            }
        }
        if (count >= MAX_BUFFER_SIZE)
            print_buffer(&ret, &count, buffer);
        i++;
    }

    if (count != 0)
        print_buffer(&ret, &count, buffer);
    va_end(ap);

    return ret;
}
