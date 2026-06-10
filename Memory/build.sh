#!/bin/bash
#
## 创建编译目录
mkdir -p build
cd build || exit 1

## cmake 生成 Makefile
cmake ..

## 多线程编译（加速）
make -j$(nproc)

## 提示完成
echo -e "\n✅ 编译完成！可执行文件在: build/memory_crash_simulator"
