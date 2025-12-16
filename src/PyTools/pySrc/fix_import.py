import sys
import os
import shutil
import importlib.util

# 配置路径
script_dir = os.path.dirname(os.path.abspath(__file__))
output_dir = os.path.join(script_dir, "..", "output")
output_dir = os.path.abspath(output_dir)

# 原始文件
source_pyd = os.path.join(output_dir, "pymodule.cp314-win_amd64.pyd")
# 目标文件（匹配当前 Python 版本）
target_pyd = os.path.join(output_dir, f"pymodule.cp{sys.version_info.major}{sys.version_info.minor}-win_amd64.pyd")

print(f"Python version: {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}")
print(f"Source PYD: {source_pyd}")
print(f"Target PYD: {target_pyd}\n")

# 添加 DLL 搜索路径
if sys.platform == 'win32':
    os.add_dll_directory(output_dir)
    build_bin_dir = os.path.join(script_dir, "..", "..", "..", "..", "build", "build-x64-Debug", "bin")
    build_bin_dir = os.path.abspath(build_bin_dir)
    if os.path.exists(build_bin_dir):
        os.add_dll_directory(build_bin_dir)
        print(f"Added DLL directory: {build_bin_dir}")

# 方法1: 尝试直接使用 importlib 加载原始文件
print("\n=== Method 1: Load with importlib (bypass version check) ===")
try:
    spec = importlib.util.spec_from_file_location("pymodule", source_pyd)
    if spec:
        module = importlib.util.module_from_spec(spec)
        # 在加载前，尝试设置环境变量来帮助找到 DLL
        sys.modules['pymodule'] = module
        spec.loader.exec_module(module)
        result = module.add(1, 2)
        print(f"✅ Success! Result: {result}")
        print(f"   Module loaded from: {source_pyd}")
    else:
        print("❌ Failed to create spec")
except Exception as e:
    print(f"❌ Failed: {e}")
    error_str = str(e)
    if "python314.dll" in error_str or "python314" in error_str.lower():
        print("\n   Problem: Module requires python314.dll but you have Python 3.12")
        print("   Solution: You need to rebuild the module for Python 3.12")
    elif "DLL" in error_str or "找不到" in error_str or "not found" in error_str.lower():
        print("\n   Problem: Missing DLL dependencies")
        print("   The module needs DLLs that are not in the search path")
    import traceback
    traceback.print_exc()

# 方法2: 如果文件不存在，创建副本（虽然版本不匹配，但可以测试）
print("\n=== Method 2: Try importing with version-matched filename ===")
if os.path.exists(source_pyd) and not os.path.exists(target_pyd):
    print(f"Creating copy: {os.path.basename(target_pyd)}")
    try:
        shutil.copy2(source_pyd, target_pyd)
        print("✅ Copy created")
    except Exception as e:
        print(f"❌ Failed to create copy: {e}")

if os.path.exists(target_pyd):
    sys.path.insert(0, output_dir)
    importlib.invalidate_caches()
    try:
        import pymodule
        result = pymodule.add(1, 2)
        print(f"✅ Success with copied file! Result: {result}")
    except Exception as e:
        print(f"❌ Still failed: {e}")
        if "python314" in str(e).lower():
            print("   The .pyd file is still linked to python314.dll")
            print("   You MUST rebuild the project for Python 3.12")

print("\n=== Summary ===")
print("The root cause is:")
print("  1. The .pyd file was compiled for Python 3.14")
print("  2. It's linked against python314.dll")
print("  3. You're running Python 3.12 which has python312.dll")
print("  4. These are incompatible - you MUST rebuild the project")
print("\nTo fix:")
print("  1. The CMakeLists.txt has been updated to auto-detect Python version")
print("  2. Rebuild the project: cd build/build-x64-Debug && cmake ../.. && ninja/make")
print("  3. This will create pymodule.cp312-win_amd64.pyd linked to python312.dll")

