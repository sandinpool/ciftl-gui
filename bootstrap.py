import os
import subprocess
import sys

# 检查当前目录是否包含 "env" 目录
env_dir = os.path.join(os.getcwd(), "env")
deps_dir = os.path.join(os.getcwd(), "deps")
if not os.path.exists(env_dir):
    print("Creating 'env' directory...")
    os.makedirs(env_dir)
else:
    print("'env' directory already exists.")

# 检查 "env" 目录是否包含 "vcpkg" 目录
vcpkg_dir = os.path.join(env_dir, "vcpkg")
if not os.path.exists(vcpkg_dir):
    print("Cloning vcpkg from GitHub...")
    try:
        subprocess.run(["git", "clone", "https://github.com/microsoft/vcpkg.git", vcpkg_dir], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error cloning vcpkg: {e}")
        sys.exit(1)

    print("Initializing vcpkg...")
    try:
        subprocess.run([os.path.join(vcpkg_dir, "bootstrap-vcpkg.bat" if sys.platform == "win32" else "bootstrap-vcpkg.sh")], check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error initializing vcpkg: {e}")
        sys.exit(1)
else:
    print("vcpkg already exists.")

# 检查当前目录是否包含 "deps" 目录
if not os.path.exists(deps_dir):
    print("Creating 'deps' directory...")
    os.makedirs(deps_dir)
else:
    print("'deps' directory already exists.")

# 检查 "deps" 目录是否包含 "ciftl-cpp" 
ciftl_cpp_dir = os.path.join(deps_dir, "ciftl-cpp")
if not os.path.exists(ciftl_cpp_dir):
    print("Cloning ciftl-cpp from GitHub...")
    try:
        subprocess.run(["git", "clone", "https://github.com/sandinpool/ciftl-cpp.git", ciftl_cpp_dir], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error cloning ciftl-cpp: {e}")
        sys.exit(1)
else:
    print("ciftl-cpp already exists.")

print("All done!")