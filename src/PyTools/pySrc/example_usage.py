"""
示例：如何使用 dll_path_init 模块

这个文件展示了如何在其他脚本中使用通用的 DLL 路径初始化。
"""

import sys
import os

# 添加 output 目录到 Python 路径
script_dir = os.path.dirname(os.path.abspath(__file__))
output_dir = os.path.join(os.path.dirname(script_dir), "output")
sys.path.insert(0, output_dir)

# 导入 DLL 路径初始化模块（只需一行）
import dll_path_init

# 现在可以正常导入和使用 pymodule
import pymodule

if __name__ == "__main__":
    result = pymodule.add(10, 20)
    print(f"pymodule.add(10, 20) = {result}")

