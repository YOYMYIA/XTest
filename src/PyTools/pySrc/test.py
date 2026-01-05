import sys
import os

# 获取脚本文件所在的目录
script_dir = os.path.dirname(os.path.abspath(__file__))
# 构建 output 目录的绝对路径
output_dir = os.path.join(os.path.dirname(script_dir), "output")

# 检查 output 目录是否存在
if not os.path.exists(output_dir):
    print(f"错误: 找不到输出目录: {output_dir}")
    sys.exit(1)

# 导入 DLL 路径初始化模块（自动添加 MinGW DLL 路径）
try:
    import dll_path_init
except ImportError:
    # 如果无法导入，回退到手动添加（兼容性处理）
    mingw_bin_path = "C:/msys64/mingw64/bin"
    if os.path.exists(mingw_bin_path):
        os.add_dll_directory(mingw_bin_path)

# 添加到 sys.path
sys.path.insert(0, output_dir)

try:
    import pymodule
    print(pymodule.add(1, 2))
except ImportError as e:
    print(f"导入错误: {e}")
    print(f"\n提示:")
    print(f"  - 当前 Python 版本: {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}")
    print(f"  - 输出目录: {output_dir}")
    if os.path.exists(output_dir):
        files = os.listdir(output_dir)
        print(f"  - 输出目录中的文件: {files}")
        pyd_files = [f for f in files if f.endswith('.pyd')]
        if pyd_files:
            print(f"\n  找到 .pyd 文件: {pyd_files}")
            print(f"  请确保 Python 版本与编译 .pyd 文件时使用的版本匹配")
    sys.exit(1)