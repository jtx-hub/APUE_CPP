#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <Windows.h>

using namespace std;

// ==========================================
// 架构师的“遗言函数”：专门拦截操作系统的死亡信号
// ==========================================
void crash_handler(int signum) {
    cout << "\n====================================================" << endl;
    cout << "🚨 [致命拦截] 程序遭遇到系统级崩溃！" << endl;
    cout << "🚨 捕获到的信号编号: " << signum << endl;
    
    if (signum == SIGSEGV) {
        cout << "🚨 错误类型: Segmentation fault (段错误)！" << endl;
        cout << "👉 原因分析: 你可能访问了越界的数组、使用了野指针，或者爆栈了。" << endl;
    } 
    else if (signum == SIGABRT) {
        cout << "🚨 错误类型: Aborted (异常中止)！" << endl;
        cout << "👉 原因分析: glibc 内存管家发现了 Double Free 或堆内存被踩烂了！" << endl;
    }
    
    cout << "====================================================" << endl;
    
    // 打印完遗言后，必须手动结束程序，否则会陷入死循环
    exit(signum); 
}

// ==========================================
// 车祸 1：栈溢出 (Stack Overflow) - 递归黑洞
// ==========================================
void crash_stack_overflow(int depth) {
// 每次下潜分配 64 KB（小步快跑，让 OS 慢慢扩展栈空间）
    char small_array[64 * 1024]; 
    memset(small_array, 0x1, sizeof(small_array)); 
    
    // 打印当前消耗了多少 KB 的栈内存
    cout << "当前下潜深度: " << depth 
         << " (总计消耗约: " << (depth * 64) << " KB). 栈指针地址: " << (void*)small_array << endl;
    
    // 递归
    crash_stack_overflow(depth + 1);
}

// ==========================================
// 车祸 2：野指针 / 释放后使用 (Use-After-Free) - 幽灵读写
// ==========================================
void crash_use_after_free() {
    cout << ">>> 正在堆上分配内存..." << endl;
    int* ptr = new int(42);
    cout << "分配的值: " << *ptr << "，地址: " << ptr << endl;
    
    cout << ">>> 立刻释放内存 (delete)..." << endl;
    delete ptr;
    
    cout << ">>> 强行对已释放的内存赋值为 9999！" << endl;
    // 极其危险：此时 ptr 变成了野指针，但这块物理内存可能还没还给 OS！
    *ptr = 9999; 
    cout << "幽灵读取: " << *ptr << " (你看，有时它居然不报错，这就是最可怕的隐患！)" << endl;
    
    // 强行制造二次分配，引发真正的系统雪崩
    cout << ">>> 再次分配新内存，观察幽灵写入如何破坏系统..." << endl;
    int* ptr2 = new int(100); 
    cout << "新内存分配完毕。如果你能看到这行，说明 C++ 运行库正在强撑。" << endl;
}

// ==========================================
// 车祸 3：堆越界写入 (Heap Buffer Overflow) - 踩坏邻居
// ==========================================
void crash_heap_overflow() {
    cout << ">>> 申请大小为 5 的数组..." << endl;
    int* arr = new int[5];
    
    cout << ">>> 强行写入第 6 个元素（越界踩坏了动态分配器的元数据！）" << endl;
    arr[5] = 888; // 越界写入！
    
    cout << ">>> 准备 delete[] 释放内存..." << endl;
    // 崩溃往往不发生在写入时，而是发生在这里！因为 free() 发现内存块的头部信息被你踩坏了。
    delete[] arr; 
    cout << ">>> delete[] 释放内存完成..." << endl;
}

// ==========================================
// 车祸 4：重复释放 (Double Free) - 瞬间毙命
// ==========================================
void crash_double_free() {
    int* ptr = new int(10);
    cout << "第一次 delete..." << endl;
    delete ptr;
    cout << "第二次 delete (准备迎接 Abort 信号)..." << endl;
    delete ptr; 
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    signal(SIGSEGV, crash_handler);
    signal(SIGABRT, crash_handler);

    int choice;
    cout << "================ C++ 内存车祸模拟靶场 ================" << endl;
    cout << "1. 触发【栈溢出】(Stack Overflow)" << endl;
    cout << "2. 触发【释放后使用】(Use-After-Free / 野指针)" << endl;
    cout << "3. 触发【堆越界写入】(Heap Buffer Overflow)" << endl;
    cout << "4. 触发【重复释放】(Double Free)" << endl;
    cout << "请选择要制造的车祸编号 (1-4): ";
    cin >> choice;

    switch (choice) {
        case 1: crash_stack_overflow(1); break;
        case 2: crash_use_after_free(); break;
        case 3: crash_heap_overflow(); break;
        case 4: crash_double_free(); break;
        default: cout << "安全退出。" << endl;
    }

    return 0;
}