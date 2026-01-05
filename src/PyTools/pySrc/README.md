# Python 版本设置指南

## 问题说明

如果你的 `.pyd` 文件是为特定 Python 版本编译的（如 Python 3.14），你需要使用匹配的 Python 版本运行脚本。

此外，使用 MinGW 编译的 `.pyd` 文件需要 MinGW 运行时 DLL，需要正确配置 DLL 搜索路径。

## 设置 Python 执行版本的方法

### 方法 1: 使用 py launcher（Windows 推荐）

在命令行中使用 `py` launcher 指定 Python 版本：

```bash
# 使用 Python 3.14 运行脚本
py -3.14 test.py

# 或者使用完整版本号
py -3.14.2 test.py
```

### 方法 2: 在 VS Code 中配置 Python 解释器

1. 按 `Ctrl+Shift+P` 打开命令面板
2. 输入 `Python: Select Interpreter`
3. 选择 Python 3.14 解释器（或输入路径：`D:\WorkTools\Python\Python314\python.exe`）

或者在 `.vscode/settings.json` 中添加：

```json
{
    "python.defaultInterpreterPath": "D:\\WorkTools\\Python\\Python314\\python.exe"
}
```

### 方法 3: 在脚本开头添加 shebang（Windows 10+ 支持）

在脚本第一行添加：

```python
#!py -3.14
```

或者使用完整路径：

```python
#!D:\WorkTools\Python\Python314\python.exe
```

### 方法 4: 创建批处理文件

创建 `run_test.bat`：

```batch
@echo off
py -3.14 test.py
```

### 方法 5: 修改系统默认 Python

如果你想让 Python 3.14 成为默认版本，可以：

1. 修改环境变量 `PATH`，将 Python 3.14 的路径放在最前面
2. 或者在 `py.ini` 文件中配置默认版本

## 检查当前 Python 版本

在脚本中或命令行中检查：

```python
import sys
print(f"Python {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}")
```

或者命令行：

```bash
python --version
py -3.14 --version
```

## MinGW DLL 路径配置（通用方法）

如果你的 `.pyd` 文件是用 MinGW 编译的，需要添加 MinGW DLL 路径。我们提供了两种通用的方法：

### 方法 1: 使用 pymodule_wrapper 模块（最简单，推荐）

使用包装器模块，自动处理所有初始化：

```python
from pymodule_wrapper import pymodule

print(pymodule.add(1, 2))  # 直接使用，无需其他配置
```

这个模块会自动：

- 添加 output 目录到 Python 路径
- 初始化 MinGW DLL 路径
- 导入 pymodule

### 方法 2: 使用 dll_path_init 模块

如果只需要 DLL 路径初始化，可以使用：

```python
import sys
import os

# 添加 output 目录
script_dir = os.path.dirname(os.path.abspath(__file__))
output_dir = os.path.join(os.path.dirname(script_dir), "output")
sys.path.insert(0, output_dir)

import dll_path_init  # 自动添加 MinGW DLL 路径
import pymodule      # 现在可以正常导入

print(pymodule.add(1, 2))
```

`dll_path_init` 模块会自动检测常见的 MinGW 安装路径（C:/msys64/mingw64/bin, D:/msys64/mingw64/bin 等）并添加 DLL 搜索路径。

### 方法 3: 使用环境变量和包装脚本

在运行脚本前设置环境变量 `PATH`：

```batch
@echo off
set PATH=C:\msys64\mingw64\bin;%PATH%
py -3.14 test.py
```

### 方法 4: 在项目级别设置（sitecustomize.py）

如果需要全局设置，可以在 Python 的 site-packages 目录创建 `sitecustomize.py`，但这不是推荐方式。

**推荐使用方法 1（pymodule_wrapper 模块）或方法 2（dll_path_init 模块）**，因为它们：

- 自动检测 MinGW 路径
- 支持多个常见的安装位置（C:/msys64/mingw64/bin, D:/msys64/mingw64/bin 等）
- 只在需要时执行
- 不影响其他 Python 脚本
- 代码简洁，易于维护

## 示例文件

- `test.py` - 基础使用示例（使用 dll_path_init）
- `example_usage.py` - 使用 dll_path_init 的完整示例
- `pymodule_wrapper.py` - 包装器模块（推荐方式）
