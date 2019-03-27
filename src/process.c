#include <caml/memory.h>
#include <caml/mlvalues.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* This fix inspired by:
 * https://github.com/facebook/flow/blob/276bb7f60e3f2fe2646cc7c77badf7086463e761/hack/utils/sysinfo.c
 */
value rench_pid(value handle) {
    CAMLparam1(handle);

#ifdef _WIN32
    HANDLE hnd = (HANDLE)Long_val(handle);
    CAMLreturn(Val_int(GetProcessId(hnd)));
#else
    CAMLreturn(handle);    
#endif
}
