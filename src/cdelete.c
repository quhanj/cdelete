#include "cdelete.h"
#include <dlfcn.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

// 线程安全的一次性初始化控制
static pthread_once_t once_control = PTHREAD_ONCE_INIT;

typedef void (*cdelete_func_t)(void*);

// 类型与函数的映射关系
static struct {
    const char* name;       // 函数名称
    cdelete_func_t func;    // 函数指针
    const char* type_name;  // 类型名称
} cdelete_map[] = {
    {"free", NULL, "CDBUF"},
    {"close", NULL, "CDOPEN"},
    {"fclose", NULL, "CDFOP"},
    {"cJSON_Delete", NULL, "CDJSON"},
    {"PQfinish", NULL, "CDPQC"},
    {"PQclear", NULL, "CDPQR"},
    {"curl_easy_cleanup", NULL, "CDCURL"}
};

// 符号检查函数
static cdelete_func_t symbol_check(const char* func_name) {
    void* handle = dlopen(NULL, RTLD_LAZY | RTLD_NOLOAD);
    if (!handle) return NULL;
    
    dlerror(); // 清除错误
    cdelete_func_t func = (cdelete_func_t)dlsym(handle, func_name);
    const char* error = dlerror();
    dlclose(handle);
    
    return error ? NULL : func;
}

// 一次性初始化函数
static void init_resources() {
    for (uint8_t i = 0; i < sizeof(cdelete_map)/sizeof(cdelete_map[0]); ++i) {
        cdelete_map[i].func = symbol_check(cdelete_map[i].name);
        if (!cdelete_map[i].func) {
            fprintf(stderr, "[CDELETE] 警告: 未找到 %s 的释放函数 %s\n",
                   cdelete_map[i].type_name, cdelete_map[i].name);
        }
    }
}

void cdelete_realfunction(void** p, enum cdeletetype t) {
    // 保证线程安全的初始化
    pthread_once(&once_control, init_resources);

    // 类型范围检查
    if (t < 0 || t >= (sizeof(cdelete_map)/sizeof(cdelete_map[0]))) {
        fprintf(stderr, "[CDELETE] 错误: 无效的资源类型 %d\n", t);
        return;
    }

    // 空指针检查
    if (!*p) return;

    // 执行资源释放
    if (cdelete_map[t].func) {
        cdelete_map[t].func(*p);
        *p = NULL;
    } else {
        fprintf(stderr, "[CDELETE] 错误: 尝试释放未初始化的 %s 类型资源\n",
               cdelete_map[t].type_name);
    }
}
