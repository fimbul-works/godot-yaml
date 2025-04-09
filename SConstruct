#!/usr/bin/env python
import os
import subprocess
import shutil

# Use cache for faster build times
os.environ['SCONS_CACHE'] = 'build/scons_cache'

base_env = SConscript('godot-cpp/SConstruct')

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
        return f"libgdyaml.{platform}.{debug_or_release}.framework/gdyaml.{platform}.{debug_or_release}"
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

    # Platform and release/debug flags
    if platform == 'windows':
        env.Append(CCFLAGS=['/std:c++17', '/EHsc'])
        if is_debug:
            env.Append(CCFLAGS=['/Z7'])

        # Add architecture-specific flags for Windows
        if arch == 'x86_32':
            env.Append(LINKFLAGS=['/MACHINE:X86'])
        else:  # x86_64
            env.Append(LINKFLAGS=['/MACHINE:X64'])
    else:
        env.Append(CCFLAGS=['-std=c++17'])
        # Add architecture-specific flags for other platforms
        if arch == 'x86_32':
            env.Append(CCFLAGS=['-m32'])
            env.Append(LINKFLAGS=['-m32'])
        else:  # x86_64
            env.Append(CCFLAGS=['-m64'])
            env.Append(LINKFLAGS=['-m64'])

    # Set debug flag
    if is_debug:
        env.Append(CPPDEFINES=['GODOT_YAML_DEBUG'])

    env.Append(CPPPATH=['src'])
    return env

def build_rapidyaml(env, variant_dir):
    platform = env.get('platform', '')
    target = env.get('target', '')

    # Configure CMake
    cmake_build_type = 'Release' if target == 'template_release' else 'Debug'

    # Separate build directories
    rapidyaml_build_dir = os.path.join(variant_dir, f'rapidyaml_build_{cmake_build_type.lower()}')
    rapidyaml_install_dir = os.path.join(variant_dir, f'rapidyaml_install_{cmake_build_type.lower()}')

    if not os.path.exists(rapidyaml_build_dir):
        os.makedirs(rapidyaml_build_dir)

    cmake_command = [
        'cmake',
        '-S', 'rapidyaml',
        '-B', rapidyaml_build_dir,
        f'-DCMAKE_INSTALL_PREFIX={rapidyaml_install_dir}',
        f'-DCMAKE_BUILD_TYPE={cmake_build_type}',
        '-DRYML_BUILD_TESTS=OFF',
        '-DRYML_BUILD_TOOLS=OFF',
        '-DRYML_DEFAULT_CALLBACK_USES_EXCEPTIONS=ON'
    ]

    # Godot uses the /MT runtime in both release and debug builds
    arch = env.get('arch', 'x86_64')
    if platform == 'windows':
        cmake_command.append('-DCMAKE_CXX_FLAGS_DEBUG=/MT')
        cmake_command.append('-DCMAKE_CXX_FLAGS_RELEASE=/MT')
        if arch == 'x86_32':
            cmake_command.append('-A Win32')
        else:
            cmake_command.append('-A x64')

    # Run CMake
    subprocess.run(cmake_command, check=True)
    subprocess.run(['cmake', '--build', rapidyaml_build_dir, '--config', cmake_build_type], check=True)
    subprocess.run(['cmake', '--install', rapidyaml_build_dir, '--config', cmake_build_type], check=True)

    # Return the paths to the built library and include directory
    lib_name = 'ryml.lib' if platform == 'windows' else 'libryml.a'

    return {
        'lib': os.path.join(rapidyaml_install_dir, 'lib', lib_name),
        'include': os.path.join(rapidyaml_install_dir, 'include')
    }

def clean_rapidyaml(env, variant_dir):
    rapidyaml_build_dir = os.path.join(variant_dir, 'rapidyaml_build')
    rapidyaml_install_dir = os.path.join(variant_dir, 'rapidyaml_install')

    # Remove build directory
    if os.path.exists(rapidyaml_build_dir):
        shutil.rmtree(rapidyaml_build_dir)

    # Remove install directory
    if os.path.exists(rapidyaml_install_dir):
        shutil.rmtree(rapidyaml_install_dir)

    print("Cleaned RapidYAML build directories")

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
    sources += Glob(os.path.join(variant_dir, 'src', 'variants', '*.cpp'))

    # Set up output directories
    output_lib_dir = os.path.join(variant_dir, 'lib')
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
    installed_lib = env.Install(bin_dir, library)
    env.Alias('install', installed_lib)

    return library, installed_lib

# Setup the build environment
env = setup_build_env(base_env)

# Establish the variant directory based on the target
variant_dir = os.path.abspath(os.path.join('build', env["target"]))
if not os.path.exists(variant_dir):
    os.makedirs(variant_dir)

# OBJPREFIX placees object files in the variant directory
env['OBJPREFIX'] = os.path.join(variant_dir, '')

target = build_config(env, variant_dir)

Default(target)
