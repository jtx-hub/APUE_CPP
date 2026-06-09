#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <utility> // 必须包含，为了使用 std::move
#include <string.h>
#include "FileDescriptor.h"
#include <Windows.h>

// ==========================================
// 场景 1：测试独占所有权与移动语义 (Move Semantics)
// ==========================================
void test_move_and_copy() {
    std::cout << "\n>>> [场景 1] 开启移动语义测试..." << std::endl;
    FileDescriptor file("test_move.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    file.Write("1. Hello from original file!\n");

    /* --- 极度危险的拷贝测试 (请取消注释尝试编译) --- */
    // 如果你的 FileDescriptor 写得够好，下面两行代码【必须导致编译失败】！
    // 因为两个 fd 析构时会 close 两次，造成 Double Free 灾难。
    // FileDescriptor file_copy = file;            // 报错：使用了被删除的函数
    // FileDescriptor file_assign; file_assign = file; // 报错：使用了被删除的函数

    /* --- 安全的所有权转移 (移动语义) --- */
    // 使用 std::move 剥夺原 file 的 fd，交给 file_moved
    FileDescriptor file_moved = std::move(file);
    file_moved.Write("2. Hello from moved file!\n");

    // 严苛测试：此时原 file 的 fd 应该被你置为 -1 了。
    // 如果这里尝试用原 file 写入，应该抛出异常，而不是偷偷把数据写坏。
    try {
        file.Write("3. Zombie write!\n");
    } catch (const std::exception& e) {
        std::cout << "    [成功拦截僵尸写入] " << e.what() << std::endl;
    }
}

// ==========================================
// 场景 2：测试构造函数中的致命异常 (如文件无权限/路径不存在)
// ==========================================
void test_open_failure() {
    std::cout << "\n>>> [场景 2] 开启打开失败异常测试..." << std::endl;
    // 故意写一个绝对不存在的乱码目录
    FileDescriptor file("/invalid_dir_name_888/test.txt", O_CREAT | O_WRONLY, 0644);
    
    // 如果构造函数没有抛出异常，下面这行就会执行，说明代码不及格！
    std::cout << "    [❌ 错误] 文件打开失败，但没有抛出异常！" << std::endl;
}

// ==========================================
// 场景 3：测试栈展开 (Stack Unwinding) 与内存安全
// ==========================================
void test_stack_unwinding() {
    std::cout << "\n>>> [场景 3] 开启异常栈展开测试..." << std::endl;
    FileDescriptor file("test_unwind.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    file.Write("Before crash...\n");

    std::cout << "    业务执行到一半，突然抛出致命异常！" << std::endl;
    throw std::runtime_error("模拟业务逻辑除以零或内存爆炸！");

    // 这行永远不会执行
    file.Write("After crash...\n"); 
}

// ==========================================
// 主函数：调度所有测试并捕获异常
// ==========================================
int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "============= FileDescriptor 工业级压力测试 =============" << std::endl;

    // 运行场景 1
    try {
        test_move_and_copy();
    } catch (const std::exception& e) {
        std::cout << "  [场景1捕获异常]: " << e.what() << std::endl;
    }

    // 运行场景 2
    try {
        test_open_failure();
    } catch (const std::exception& e) {
        std::cout << "  [场景2完美捕获预期异常]: " << e.what() << std::endl;
    }

    // 运行场景 3
    try {
        test_stack_unwinding();
    } catch (const std::exception& e) {
        std::cout << "  [场景3捕获业务异常]: " << e.what() << std::endl;
    }

    std::cout << "\n============= 测试结束 =============" << std::endl;
    std::cout << "请检查当前目录下是否生成了 test_move.txt 和 test_unwind.txt，" << std::endl;
    std::cout << "并且确认析构函数中的 close() 都被正确打印调用了！" << std::endl;

    return 0;
}