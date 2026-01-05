"""
通用 DLL 路径初始化模块

在使用 MinGW 编译的 .pyd 文件前，导入此模块以自动添加 MinGW DLL 路径。
使用方法：
    import dll_path_init  # 无需其他操作，导入即可
    import pymodule
"""
import os
import sys

# 尝试添加 MinGW DLL 路径
_MINGW_PATHS = [
    "C:/msys64/mingw64/bin",
    "C:/mingw64/bin",
    "D:/msys64/mingw64/bin",
    "D:/mingw64/bin",
]

_added_paths = set()

def add_mingw_dll_path():
    """添加 MinGW DLL 路径到 Python 的 DLL 搜索路径"""
    for mingw_path in _MINGW_PATHS:
        if os.path.exists(mingw_path) and mingw_path not in _added_paths:
            try:
                os.add_dll_directory(mingw_path)
                _added_paths.add(mingw_path)
                if __name__ != "__main__":
                    # 只在非主模块时打印（避免干扰输出）
                    pass
            except (OSError, AttributeError):
                # Python < 3.8 不支持 add_dll_directory，或者路径无效
                pass

# 自动执行初始化
add_mingw_dll_path()

