#include "nscore.h"

NS_EXTERN_C
{
  NS_EXPORT void test_v();

  NS_EXPORT short test_s();
  NS_EXPORT short test_s_s(short);
  NS_EXPORT short test_s_ss(short, short);
  NS_EXPORT short* test_s_echo(short*);

  NS_EXPORT int test_i();
  NS_EXPORT int test_i_i(int);
  NS_EXPORT int test_i_ii(int, int);
  NS_EXPORT int* test_i_echo(int*);

  NS_EXPORT float test_f();
  NS_EXPORT float test_f_f(float);
  NS_EXPORT float test_f_ff(float, float);
  NS_EXPORT float* test_f_echo(float*);

  NS_EXPORT double test_d();
  NS_EXPORT double test_d_d(double);
  NS_EXPORT double test_d_dd(double, double);
  NS_EXPORT double* test_d_echo(double*);

  NS_EXPORT int test_strlen(const char*);
  NS_EXPORT int test_wcslen(const wchar_t*);
  NS_EXPORT char* test_strret();
  NS_EXPORT char* test_str_echo(const char*);

  NS_EXPORT int test_i_if_floor(int, float);

  struct POINT {
    int x;
    int y;
  };

  struct RECT {
    int top;
    int left;
    int bottom;
    int right;
  };

  NS_EXPORT int test_pt_in_rect(RECT, POINT);
  NS_EXPORT void test_init_pt(POINT* pt, int x, int y);
}
