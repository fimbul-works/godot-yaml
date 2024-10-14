#!/usr/bin/env python
import os
import sys

# Use cache for faster build times
os.environ['SCONS_CACHE'] = 'build/scons_cache'

base_env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# Determine build target
AddOption(
    '--target',
    dest='target',
    type='string',
    nargs=1,
    action='store',
    metavar='TARGET',
    help='Specify the build target (debug or release)',
    default='debug'
)

def setup_build_env(base_env):
    env = base_env.Clone()

    # Get the target from command line option
    target = GetOption('target')
    env['target'] = f'template_{target}' if target != 'test' else 'test'

    is_release = target == "release"

    if env.get('platform', '') == 'windows':
        os.environ['godot_cpp_windows_use_dynamic_runtime'] = 'yes'
        env.Append(CCFLAGS=['/EHsc', '/std:c++17'])
    else:
        env.Append(CCFLAGS=['-std=c++17'])

    if is_release:
        if env.get('platform', '') == 'windows':
            env.Append(CCFLAGS=['/O2', '/GL'])
            env.Append(LINKFLAGS=['/LTCG'])
        else:
            env.Append(CCFLAGS=['-O3', '-fomit-frame-pointer'])
    else:
        if env.get('platform', '') == 'windows':
            env.Append(CCFLAGS=['/Z7', '/Od'])
        else:
            env.Append(CCFLAGS=['-g', '-O0'])
        env.Append(CPPDEFINES=['GODOT_YAML_DEBUG'])

    env.Append(CPPPATH=["src"])

    return env

def build_config(env, variant_dir):
    # Set up variant dir for our sources
    env.VariantDir(os.path.join(variant_dir, 'src'), 'src', duplicate=0)

    # Gather source files
    sources = Glob(os.path.join(variant_dir, 'src', '*.cpp'))
#    sources += Glob(os.path.join(variant_dir, 'src', 'variants', '*.cpp'))

    # Set up output directories
    output_lib_dir = os.path.join(variant_dir, 'lib')
    env.Append(LIBPATH=[output_lib_dir])

    # Build the library
    library = env.SharedLibrary(
        target=os.path.join(output_lib_dir, get_library_name(env)),
        source=sources
    )

    # Copy the library to the demo directory
    demo_dir = os.path.join('demo', 'addons', 'yaml', 'bin')
    installed_lib = env.Install(demo_dir, library)

    # Create an alias for the install target
    env.Alias('install', installed_lib)

    # Return both the library and the installed library
    return library, installed_lib

def get_library_name(env):
    platform = env.get("platform", "unknown")
    target = env.get("target", "unknown")
    arch = env.get("arch", "unknown")

    if platform == "windows":
        return f"libgdyaml.windows.{target}.{arch}.dll"
    elif platform in ["linux", "android"]:
        return f"libgdyaml.{platform}.{target}.{arch}.so"
    elif platform == "macos":
        return f"libgdyaml.macos.{target}.framework"
    elif platform == "ios":
        return f"libgdyaml.ios.{target}.xcframework"
    else:
        print(f"Unsupported platform: {platform}")
        return f"libgdyaml.{platform}.{target}.{arch}"

# Setup the build environment
env = setup_build_env(base_env)

# Determine the variant directory based on the target
variant_dir = os.path.abspath(os.path.join('build', GetOption("target")))

# Ensure the variant directory exists
if not os.path.exists(variant_dir):
    os.makedirs(variant_dir)

# Set OBJPREFIX to place object files in the variant directory
env['OBJPREFIX'] = os.path.join(variant_dir, '')

target = build_config(env, variant_dir)

Default(target)
