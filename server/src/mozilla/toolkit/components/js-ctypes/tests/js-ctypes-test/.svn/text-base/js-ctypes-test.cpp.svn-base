#include "js-ctypes-test.h"
#include <string.h>
#include <wchar.h>
#include <math.h>

void
test_v()
{
  // do nothing
  return;
}

short
test_s()
{
  return 12345;
}

short
test_s_s(short number)
{
  return number;
}

short
test_s_ss(short number1, short number2)
{
  return number1 + number2;
}

short*
test_s_echo(short *number)
{
  return number;
}

int
test_i()
{
  return 123456789;
}

int
test_i_i(int number)
{
  return number;
}

int
test_i_ii(int number1, int number2)
{
  return number1 + number2;
}

int*
test_i_echo(int *number)
{
  return number;
}

float
test_f()
{
  return 123456.5f;
}

float
test_f_f(float number)
{
  return number;
}

float
test_f_ff(float number1, float number2)
{
  return (number1 + number2);
}

float*
test_f_echo(float *number)
{
  return number;
}

double
test_d()
{
  return 123456789.5;
}

double
test_d_d(double number)
{
  return number;
}

double
test_d_dd(double number1, double number2)
{
  return (number1 + number2);
}

double*
test_d_echo(double *number)
{
  return number;
}

int
test_strlen(const char* string)
{
  return strlen(string);
}

int
test_wcslen(const wchar_t* string)
{
  return wcslen(string);
}

char gDummyString[] = "success";

char *
test_strret()
{
  return gDummyString;
}

char *
test_str_echo(const char* string)
{
  return (char*)string;
}

int
test_i_if_floor(int number1, float number2)
{
  return floor(float(number1) + number2);
}

int
test_pt_in_rect(RECT rc, POINT pt)
{
  if (pt.x < rc.left || pt.x > rc.right)
    return 0;
  if (pt.y < rc.top || pt.y > rc.bottom)
    return 0;
  return 1;
}

void
test_init_pt(POINT* pt, int x, int y)
{
  pt->x = x;
  pt->y = y;
}
