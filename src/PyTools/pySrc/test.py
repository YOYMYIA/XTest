import sys
import os

# 获取当前脚本所在目录
script_dir = os.path.dirname(os.path.abspath(__file__))
# 计算 output 目录的绝对路径
output_dir = os.path.join(script_dir, "..", "output")
output_dir = os.path.abspath(output_dir)

# 添加 output 目录到 Python 模块搜索路径
if output_dir not in sys.path:
    sys.path.insert(0, output_dir)

# 添加 DLL 搜索路径（Windows 需要）
if sys.platform == 'win32':
    # 添加 output 目录（.pyd 文件所在位置）
    os.add_dll_directory(output_dir)
    
    # 添加构建输出目录（包含所有依赖的 DLL）
    # 从 pySrc -> PyTools -> src -> 项目根目录 -> build/build-x64-Debug/bin
    build_bin_dir = os.path.join(script_dir, "..", "..", "..", "..", "build", "build-x64-Debug", "bin")
    build_bin_dir = os.path.abspath(build_bin_dir)
    if os.path.exists(build_bin_dir):
        os.add_dll_directory(build_bin_dir)
        print(f"Added DLL directory: {build_bin_dir}")

print(f"Python version: {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}")
print(f"Python executable: {sys.executable}")
print(f"Looking for pymodule in: {output_dir}")

if os.path.exists(output_dir):
    pyd_files = [f for f in os.listdir(output_dir) if f.endswith('.pyd')]
    print(f"Found .pyd files: {pyd_files}")
    if pyd_files:
        # 检查 .pyd 文件的 Python 版本
        for pyd_file in pyd_files:
            import re
            # 匹配 cp314 格式（Python 3.14）或 cp312 格式（Python 3.12）
            match = re.search(r'cp(\d)(\d+)', pyd_file)
            if match:
                pyd_py_major = int(match.group(1))
                pyd_py_minor = int(match.group(2))
                current_major = sys.version_info.major
                current_minor = sys.version_info.minor
                if pyd_py_major != current_major or pyd_py_minor != current_minor:
                    print(f"\n⚠️  WARNING: {pyd_file} is compiled for Python {pyd_py_major}.{pyd_py_minor}, but you're using Python {current_major}.{current_minor}")
                    print(f"   Please rebuild the project to match your Python version.")
else:
    print(f"Output directory not found: {output_dir}")

# 尝试多种导入方法
success = False

# 方法1: 标准 import
print("\n=== Trying standard import ===")
try:
    import pymodule
    result = pymodule.add(1, 2)
    print(f"✅ Success with standard import! Result: {result}")
    success = True
except ImportError as e:
    error_msg = str(e)
    print(f"❌ Standard import failed: {e}")
    
    # 如果是版本不匹配，尝试方法2
    if "No module named" in error_msg:
        print("   Trying alternative import method...")
        
        # 方法2: 使用 importlib 直接加载文件（绕过版本检查）
        print("\n=== Trying importlib direct load ===")
        try:
            import importlib.util
            # 查找任何 .pyd 文件
            pyd_files = [f for f in os.listdir(output_dir) if f.endswith('.pyd')]
            if pyd_files:
                pyd_path = os.path.join(output_dir, pyd_files[0])
                print(f"   Loading: {pyd_files[0]}")
                spec = importlib.util.spec_from_file_location("pymodule", pyd_path)
                if spec:
                    module = importlib.util.module_from_spec(spec)
                    sys.modules['pymodule'] = module
                    spec.loader.exec_module(module)
                    result = module.add(1, 2)
                    print(f"✅ Success with importlib! Result: {result}")
                    success = True
                else:
                    print("   ❌ Failed to create module spec")
        except Exception as e2:
            print(f"   ❌ importlib method also failed: {e2}")
            if "python314" in str(e2).lower() or "DLL" in str(e2):
                print("\n   🔍 Root cause identified:")
                print("      The .pyd file is linked to python314.dll")
                print("      Your Python 3.12 doesn't have this DLL")
                print("      This is a VERSION MISMATCH issue")
    
    if not success:
        print(f"\n{'='*60}")
        print("❌ All import methods failed")
        print(f"{'='*60}")
        print("\n🔍 Diagnosis:")
        print("   The .pyd file was compiled for Python 3.14")
        print("   It requires python314.dll which doesn't exist in Python 3.12")
        print("   Even bypassing the filename check, the DLL dependency fails")
        print("\n✅ Solution:")
        print("   1. The CMakeLists.txt has been updated to auto-detect Python version")
        print("   2. Rebuild the project:")
        print("      cd build/build-x64-Debug")
        print("      cmake ../..")
        print("      ninja  (or make)")
        print("   3. This will create pymodule.cp312-win_amd64.pyd")
        print("      which is linked to python312.dll (the correct DLL)")
        import traceback
        traceback.print_exc()
except Exception as e:
    print(f"\n❌ Unexpected error: {e}")
    import traceback
    traceback.print_exc()