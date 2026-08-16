#!/usr/bin/env python
import os
import subprocess
import shutil
import glob

# Use cache for faster build times
cache_dir = os.path.abspath('build/scons_cache')
if not os.path.exists(cache_dir):
    os.makedirs(cache_dir, exist_ok=True)
os.environ['SCONS_CACHE'] = cache_dir

base_env = SConscript('ext/godot-cpp/SConstruct')

def get_library_name(env):
    platform = env.get('platform', 'unknown')
    target = env.get('target', 'unknown')
    arch = env.get('arch', 'unknown')

    debug_or_release = 'release' if target == 'template_release' else 'debug'

    if platform == 'windows':
        return f'libgdyaml.windows.{debug_or_release}.{arch}.dll'
    elif platform in ['linux', 'android']:
        return f'libgdyaml.{platform}.{debug_or_release}.{arch}.so'
    elif platform == 'macos':
        return f'libgdyaml.{platform}.{debug_or_release}.dylib'
    elif platform == 'ios':
        return f'libgdyaml.ios.{debug_or_release}.xcframework'
    else:
        print(f'Unsupported platform: {platform}')
        return f'libgdyaml.{platform}.{debug_or_release}.{arch}'

def setup_build_env(base_env):
    env = base_env.Clone()

    platform = env.get('platform', '')
    is_debug = env.get('target', '') != 'template_release'
    arch = env.get('arch', 'x86_64')  # Default to 64-bit

    print(f"Building for {platform}, {'Debug' if is_debug else 'Release'}, {arch}")

    # ========== WINDOWS ==========
    if platform == 'windows':
        # C++ standard and exception handling
        env.Append(CCFLAGS=['/EHsc'])

        # Debug vs Release
        if is_debug:
            env.Append(CCFLAGS=['/Z7'])    # Debug info
        else:
            env.Append(CCFLAGS=[
                '/O2',    # Optimize for speed
                '/Oi',    # Intrinsic functions
            ])
            env.Append(CPPDEFINES=['NDEBUG'])

        # Architecture
        if arch == 'x86_32':
            env.Append(LINKFLAGS=['/MACHINE:X86'])
        else:
            env.Append(LINKFLAGS=['/MACHINE:X64'])

        env.Append(LINKFLAGS=['/IMPLIB:${TARGET.base}.lib'])

    # ========== UNIX-LIKE ==========
    else:
        if 'CXXFLAGS' in env and isinstance(env['CXXFLAGS'], list):
            env['CXXFLAGS'] = [f for f in env['CXXFLAGS'] if f != '-fno-exceptions']
        env.Append(CXXFLAGS=['-fexceptions'])
        env.Append(CCFLAGS=['-fexceptions'])

        if platform == 'macos':
            env.Append(CCFLAGS=['-mmacosx-version-min=10.15'])
            env.Append(LINKFLAGS=[
                '-mmacosx-version-min=10.15',
                '-Wl,-undefined,dynamic_lookup'
            ])

        if platform == 'linux':
          env.Append(CCFLAGS=['-fPIC'])
          env.Append(LINKFLAGS=["-Wl,-R,'$$ORIGIN'"])

        if is_debug:
            env.Append(CCFLAGS=['-O0', '-g'])
        else:
            env.Append(CCFLAGS=['-O3'])
            env.Append(CPPDEFINES=['NDEBUG'])

        # Architecture
        if arch == 'x86_32':
            env.Append(CCFLAGS=['-m32'])
            env.Append(LINKFLAGS=['-m32'])
        elif arch == 'x86_64':
            env.Append(CCFLAGS=['-m64'])
            env.Append(LINKFLAGS=['-m64'])

    # ========== COMMON ==========
    if is_debug:
        env.Append(CPPDEFINES=['GODOT_YAML_DEBUG', 'GODOT_SCHEMA_DEBUG'])

    env.Append(CPPPATH=['src'])
    return env

def get_build_path(env):
    """Create a unique build path based on platform, target, and architecture"""
    platform = env.get('platform', 'unknown')
    target = env.get('target', 'unknown')
    arch = env.get('arch', 'unknown')

    # Create a unique directory name
    build_path = os.path.join('build', f"{platform}_{target}_{arch}")
    return os.path.abspath(build_path)

def build_rapidyaml(env, variant_dir):
    """ Build RapidYAML """
    platform = env.get('platform', '')
    target = env.get('target', '')
    arch = env.get('arch', 'x86_64')

    # Configure CMake
    cmake_build_type = 'Release' if target == 'template_release' else 'Debug'

    # Separate build directories using the variant_dir which now includes platform and architecture
    rapidyaml_build_dir = os.path.join(variant_dir, 'rapidyaml_build')
    rapidyaml_install_dir = os.path.join(variant_dir, 'rapidyaml_install')

    if not os.path.exists(rapidyaml_build_dir):
        os.makedirs(rapidyaml_build_dir)

    cmake_command = [
        'cmake',
        '-S', 'ext/rapidyaml',
        '-B', rapidyaml_build_dir,
        f'-DCMAKE_INSTALL_PREFIX={rapidyaml_install_dir}',
        f'-DCMAKE_BUILD_TYPE={cmake_build_type}',
        '-DRYML_BUILD_TESTS=OFF',
        '-DRYML_BUILD_TOOLS=OFF',
        '-DRYML_DEFAULT_CALLBACK_USES_EXCEPTIONS=ON',
        '-DCMAKE_POSITION_INDEPENDENT_CODE=ON'
    ]

    # Godot uses the /MT runtime in both release and debug builds
    if platform == 'windows':
        cmake_command.append('-DCMAKE_CXX_FLAGS_DEBUG=/MT')
        cmake_command.append('-DCMAKE_CXX_FLAGS_RELEASE=/MT')

    # Android NDK configuration
    elif platform == 'android':
        # Use Ninja generator for Android to avoid MSBuild issues on Windows
        cmake_command.insert(1, 'Ninja')
        cmake_command.insert(1, '-G')

        # Get Android NDK path from environment
        android_ndk = env.get('ANDROID_NDK_ROOT', os.environ.get('ANDROID_NDK_ROOT', ''))
        if not android_ndk:
            raise ValueError("ANDROID_NDK_ROOT must be set for Android builds")

        # Map Godot arch to Android ABI
        android_abi_map = {
            'arm32': 'armeabi-v7a',
            'arm64': 'arm64-v8a',
            'x86_32': 'x86',
            'x86_64': 'x86_64'
        }
        android_abi = android_abi_map.get(arch, 'arm64-v8a')

        # Android NDK CMake toolchain
        toolchain_file = os.path.join(android_ndk, 'build', 'cmake', 'android.toolchain.cmake')
        cmake_command.extend([
            f'-DCMAKE_TOOLCHAIN_FILE={toolchain_file}',
            f'-DANDROID_ABI={android_abi}',
            '-DANDROID_PLATFORM=android-21',  # Minimum API level
            '-DANDROID_STL=c++_shared',
            '-DCMAKE_SYSTEM_NAME=Android'
        ])
    else:
        cmake_command.append('-DCMAKE_CXX_FLAGS=-fexceptions')

    # Prepare environment for subprocess to include SCons-detected paths/compilers
    sub_env = os.environ.copy()
    if 'ENV' in env:
        sub_env.update(env['ENV'])

    # Run CMake
    subprocess.run(cmake_command, check=True, env=sub_env)
    subprocess.run(['cmake', '--build', rapidyaml_build_dir, '--config', cmake_build_type], check=True, env=sub_env)
    subprocess.run(['cmake', '--install', rapidyaml_build_dir, '--config', cmake_build_type], check=True, env=sub_env)

    # Return the paths to the built library and include directory
    lib_name = 'ryml.lib' if platform == 'windows' else 'libryml.a'

    return {
        'lib': os.path.join(rapidyaml_install_dir, 'lib', lib_name),
        'include': os.path.join(rapidyaml_install_dir, 'include')
    }

def clean_rapidyaml(env, variant_dir):
    """ Clean RapidYAML build folders """
    rapidyaml_build_dir = os.path.join(variant_dir, 'rapidyaml_build')
    rapidyaml_install_dir = os.path.join(variant_dir, 'rapidyaml_install')

    # Remove build directory
    if os.path.exists(rapidyaml_build_dir):
        shutil.rmtree(rapidyaml_build_dir)

    # Remove install directory
    if os.path.exists(rapidyaml_install_dir):
        shutil.rmtree(rapidyaml_install_dir)

    print(f"Cleaned RapidYAML build directories for {variant_dir}")

def copy_gdschema_files():
    """Copy GDSchema documentation and icons to the project"""
    import shutil
    import glob

    # Copy documentation XML files
    doc_source = 'ext/gdschema/doc_classes'
    doc_dest = 'doc_classes'

    if os.path.exists(doc_source):
        if not os.path.exists(doc_dest):
            os.makedirs(doc_dest)

        xml_files = glob.glob(os.path.join(doc_source, '*.xml'))
        for xml_file in xml_files:
            dest_file = os.path.join(doc_dest, os.path.basename(xml_file))
            shutil.copy2(xml_file, dest_file)
            print(f"Copied documentation: {os.path.basename(xml_file)}")

    # Copy schema icon
    icon_source = 'ext/gdschema/project/addons/GDSchema/icon.svg'
    icon_dest = 'project/addons/yaml/icon-schema.svg'

    if os.path.exists(icon_source):
        dest_dir = os.path.dirname(icon_dest)
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
        shutil.copy2(icon_source, icon_dest)
        print(f"Copied icon: icon-schema.svg")

def build_config(env, variant_dir):
    # Set up variant dir for our sources
    env.VariantDir(os.path.join(variant_dir, 'src'), 'src', duplicate=0)

    # Handle rapidyaml
    if env.GetOption('clean'):
        clean_rapidyaml(env, variant_dir)
        rapidyaml = None
    else:
        rapidyaml = build_rapidyaml(env, variant_dir)
        # Add rapidyaml to the environment
        env.Append(CPPPATH=[rapidyaml['include']])
        env.Append(LIBPATH=[os.path.dirname(rapidyaml['lib'])])
        env.Append(LIBS=['ryml'])

    # Gather source files
    sources = Glob(os.path.join(variant_dir, 'src', '*.cpp'))
    sources += Glob(os.path.join(variant_dir, 'src', 'variant_converters', '*.cpp'))
    sources += Glob(os.path.join(variant_dir, 'src', 'emitter', '*.cpp'))
    sources += Glob(os.path.join(variant_dir, 'src', 'extension', '*.cpp'))
    sources += Glob(os.path.join(variant_dir, 'src', 'parser', '*.cpp'))
    sources += Glob(os.path.join(variant_dir, 'src', 'style', '*.cpp'))
    sources += Glob(os.path.join(variant_dir, 'src', 'util', '*.cpp'))
    sources += Glob(os.path.join(variant_dir, 'src', 'validator', '*.cpp'))

    # Add GDSchema sources
    sources += Glob(os.path.join('ext', 'gdschema', 'src', '*.cpp'))
    sources += Glob(os.path.join('ext', 'gdschema', 'src', 'selector', '*.cpp'))
    sources += Glob(os.path.join('ext', 'gdschema', 'src', 'rule', '*.cpp'))
    sources += Glob(os.path.join('ext', 'gdschema', 'src', 'rule_factory', '*.cpp'))
    env.Append(CPPPATH=["ext/gdschema/src"])

    # Embed documentation
    if env["target"] in ["editor", "template_debug"]:
        try:
            doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
            sources.append(doc_data)
        except AttributeError:
            print("Not including class reference as we're targeting a pre-4.3 baseline.")

    # Set up output directories
    output_lib_dir = os.path.join(variant_dir, 'lib')
    if not os.path.exists(output_lib_dir):
        os.makedirs(output_lib_dir)

    env.Append(LIBPATH=[output_lib_dir])

    # Build the library
    library = env.SharedLibrary(
        target=os.path.join(output_lib_dir, get_library_name(env)),
        source=sources
    )

    # Add dependency on rapidyaml
    if not env.GetOption('clean') and rapidyaml:
        env.Depends(library, rapidyaml['lib'])

    # Install the built library to the bin directory
    bin_dir = os.path.join('project', 'addons', 'yaml', 'bin')
    if not os.path.exists(bin_dir):
        os.makedirs(bin_dir)

    # Handle installation differently for Windows vs other platforms
    platform = env.get('platform', '')
    if platform == 'windows':
        # For Windows, install all generated files that match our library name pattern
        lib_base_name = get_library_name(env).replace('.dll', '')

        # Create a custom install action that uses glob at build time
        def install_windows_files(target, source, env):
            # Convert SCons File objects to strings and grab DLL
            lib_pattern_str = os.path.join(output_lib_dir, f"{lib_base_name}.dll")
            generated_files = glob.glob(lib_pattern_str)

            installed_files = []
            for file_path in generated_files:
                if os.path.exists(file_path):
                    dest_path = os.path.join(bin_dir, os.path.basename(file_path))
                    shutil.copy2(file_path, dest_path)
                    installed_files.append(dest_path)
                    print(f"Installed: {dest_path}")
                else:
                    print(f"Warning: {file_path} not found, skipping")
            return None

        # Create a dummy target for the install action
        install_target = os.path.join(bin_dir, get_library_name(env))
        installed_lib = env.Command(install_target, library, install_windows_files)
    else:
        # For non-Windows platforms, use the standard install
        installed_lib = env.Install(bin_dir, library)

    env.Alias('install', installed_lib)
    return library, installed_lib

# Setup the build environment
env = setup_build_env(base_env)

# Copy GDSchema files before building
if not env.GetOption('clean'):
    copy_gdschema_files()

# Establish the variant directory based on platform, target, and architecture
variant_dir = get_build_path(env)
if not os.path.exists(variant_dir):
    os.makedirs(variant_dir)

# OBJPREFIX places object files in the variant directory
env['OBJPREFIX'] = os.path.join(variant_dir, '')

target = build_config(env, variant_dir)

Default(target)
