"""
pymodule 包装器

自动处理 DLL 路径初始化，然后导入 pymodule。
使用此模块可以无需手动导入 dll_path_init。

使用方法：
    from pymodule_wrapper import pymodule
    # 或者
    import pymodule_wrapper
    pymodule_wrapper.pymodule.add(1, 2)
"""
import sys
import os

# 添加 output 目录到 Python 路径
_script_dir = os.path.dirname(os.path.abspath(__file__))
_output_dir = os.path.join(os.path.dirname(_script_dir), "output")
if _output_dir not in sys.path:
    sys.path.insert(0, _output_dir)

# 自动导入 DLL 路径初始化
try:
    import dll_path_init
except ImportError:
    # 如果无法导入 dll_path_init，尝试手动添加
    _mingw_bin_path = "C:/msys64/mingw64/bin"
    if os.path.exists(_mingw_bin_path):
        try:
            os.add_dll_directory(_mingw_bin_path)
        except (OSError, AttributeError):
            pass

# 导入 pymodule（现在 DLL 路径已经配置好了）
try:
    import pymodule
except ImportError as e:
    raise ImportError(
        f"无法导入 pymodule: {e}\n"
        "请确保:\n"
        "  1. output 目录中包含 pymodule.cp*.pyd 文件\n"
        "  2. Python 版本与编译时的版本匹配\n"
        "  3. MinGW DLL 路径已正确配置"
    ) from e

# 导出 pymodule，使其可以直接使用
__all__ = ['pymodule']

