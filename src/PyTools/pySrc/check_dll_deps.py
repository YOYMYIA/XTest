import os
import sys
import subprocess

# 检查 pymodule 的 DLL 依赖
pyd_file = r'D:\gitRoot\XTest\src\PyTools\output\pymodule.cp314-win_amd64.pyd'
build_bin_dir = r'D:\gitRoot\XTest\build\build-x64-Debug\bin'

print("Checking DLL dependencies...")
print(f"PYD file: {pyd_file}")
print(f"Build bin dir: {build_bin_dir}\n")

# 尝试使用 objdump (MinGW) 或 dumpbin (MSVC) 来检查依赖
tools = ['objdump', 'dumpbin']
tool_found = None

for tool in tools:
    try:
        result = subprocess.run([tool, '--version'], capture_output=True, text=True, timeout=2)
        if result.returncode == 0 or 'GNU' in result.stdout or 'GNU' in result.stderr:
            tool_found = tool
            break
    except:
        continue

if tool_found == 'objdump':
    print("Using objdump to check dependencies...")
    try:
        result = subprocess.run(['objdump', '-p', pyd_file], capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            lines = result.stdout.split('\n')
            in_dll_section = False
            dlls = []
            for line in lines:
                if 'DLL Name:' in line:
                    dll_name = line.split('DLL Name:')[1].strip()
                    dlls.append(dll_name)
            
            print(f"\nFound {len(dlls)} DLL dependencies:")
            for dll in dlls:
                print(f"  - {dll}")
                # 检查 DLL 是否存在
                dll_paths = [
                    os.path.join(build_bin_dir, dll),
                    os.path.join(os.path.dirname(pyd_file), dll),
                    os.path.join(sys.prefix, 'DLLs', dll),
                    os.path.join(sys.prefix, 'bin', dll),
                ]
                found = False
                for dll_path in dll_paths:
                    if os.path.exists(dll_path):
                        print(f"    ✅ Found at: {dll_path}")
                        found = True
                        break
                if not found:
                    # 检查系统 PATH
                    import shutil
                    system_dll = shutil.which(dll)
                    if system_dll:
                        print(f"    ✅ Found in system PATH: {system_dll}")
                    else:
                        print(f"    ❌ NOT FOUND - This may be the problem!")
        else:
            print(f"objdump failed: {result.stderr}")
    except Exception as e:
        print(f"Error running objdump: {e}")

# 也检查 build_bin_dir 中的所有 DLL
print(f"\n\nDLLs in build bin directory ({build_bin_dir}):")
if os.path.exists(build_bin_dir):
    dll_files = [f for f in os.listdir(build_bin_dir) if f.endswith('.dll')]
    for dll in sorted(dll_files):
        print(f"  - {dll}")
else:
    print(f"  Directory does not exist!")

# 检查 Python DLL
print(f"\nPython DLL location:")
python_dll = f"python{sys.version_info.major}{sys.version_info.minor}.dll"
python_dll_paths = [
    os.path.join(sys.prefix, 'DLLs', python_dll),
    os.path.join(sys.prefix, 'bin', python_dll),
    os.path.join(os.path.dirname(sys.executable), python_dll),
]
for path in python_dll_paths:
    if os.path.exists(path):
        print(f"  ✅ Python DLL found: {path}")
        break
else:
    print(f"  ❌ Python DLL not found: {python_dll}")

