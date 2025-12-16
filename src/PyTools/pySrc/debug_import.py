import sys
import os
import importlib.util

# 设置路径
output_dir = r'D:\gitRoot\XTest\src\PyTools\output'
pyd_file = os.path.join(output_dir, 'pymodule.cp314-win_amd64.pyd')

print(f"Python version: {sys.version}")
print(f"Python executable: {sys.executable}")
print(f"\nChecking file:")
print(f"  Output dir exists: {os.path.exists(output_dir)}")
print(f"  PYD file exists: {os.path.exists(pyd_file)}")
if os.path.exists(pyd_file):
    print(f"  PYD file size: {os.path.getsize(pyd_file)} bytes")
    print(f"  PYD file name: {os.path.basename(pyd_file)}")

print(f"\nTrying different import methods:")

# 方法1: 直接 import
print("\n1. Direct import:")
sys.path.insert(0, output_dir)
try:
    import pymodule
    print("   ✅ Success with direct import!")
    print(f"   Module: {pymodule}")
    print(f"   Module file: {pymodule.__file__ if hasattr(pymodule, '__file__') else 'N/A'}")
except Exception as e:
    print(f"   ❌ Failed: {e}")

# 方法2: 使用 importlib 从文件加载
print("\n2. Using importlib.util.spec_from_file_location:")
try:
    spec = importlib.util.spec_from_file_location("pymodule", pyd_file)
    if spec is None:
        print("   ❌ Failed to create spec")
    else:
        print(f"   ✅ Spec created: {spec}")
        print(f"   Spec loader: {spec.loader}")
        module = importlib.util.module_from_spec(spec)
        print(f"   ✅ Module object created: {module}")
        spec.loader.exec_module(module)
        print(f"   ✅ Module loaded successfully!")
        print(f"   Module: {module}")
        if hasattr(module, 'add'):
            result = module.add(1, 2)
            print(f"   ✅ Function works! add(1,2) = {result}")
except Exception as e:
    print(f"   ❌ Failed: {e}")
    import traceback
    traceback.print_exc()

# 方法3: 检查 Python 如何查找模块
print("\n3. Checking what Python looks for:")
print(f"   sys.path: {sys.path[:5]}...")
print(f"   Python looks for: pymodule.pyd or pymodule.cp{sys.version_info.major}{sys.version_info.minor}-*.pyd")

# 方法4: 尝试重命名或创建符号链接
print("\n4. Trying to create a version-agnostic name:")
try:
    # 创建不带版本号的副本名（Python 3.12 会查找 pymodule.cp312-win_amd64.pyd）
    target_name = f"pymodule.cp{sys.version_info.major}{sys.version_info.minor}-win_amd64.pyd"
    target_path = os.path.join(output_dir, target_name)
    
    if os.path.exists(pyd_file) and not os.path.exists(target_path):
        print(f"   Creating copy: {target_name}")
        import shutil
        shutil.copy2(pyd_file, target_path)
        print(f"   ✅ Copy created")
        
        # 现在尝试导入
        try:
            import importlib
            importlib.invalidate_caches()  # 清除导入缓存
            import pymodule
            print(f"   ✅ Import successful after copy!")
            result = pymodule.add(1, 2)
            print(f"   ✅ Function works! add(1,2) = {result}")
        except Exception as e2:
            print(f"   ❌ Still failed: {e2}")
    elif os.path.exists(target_path):
        print(f"   Target already exists: {target_name}")
    else:
        print(f"   Source file not found")
except Exception as e:
    print(f"   ❌ Failed: {e}")
    import traceback
    traceback.print_exc()

