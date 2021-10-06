#!/usr/bin/env python

EnsureSConsVersion(3, 0, 0)
EnsurePythonVersion(3, 5)

import atexit
import glob
import os
import pickle
import sys
import time
from collections import OrderedDict

import methods
import glsl_builders

platform_list = []
platform_opts = {}
platform_flags = {}

active_platforms = []
active_platform_ids = []
platform_exporters = []
platform_apis = []

time_at_start = time.time()

for x in sorted(glob.glob("platform/*")):
    if not os.path.isdir(x) or not os.path.exists(x + "/detect.py"):
        continue
    tmppath = "./" + x
    
    sys.path.insert(0, tmppath)
    import detect
    
    if os.path.exists(x + "/export/export.cpp"):
        platform_exporters.append(x[9:])
    if os.path.exists(x + "/api/api.cpp"):
        platform_apis.append(x[9:])
    if detect.is_active():
        active_platforms.append(detect.get_name())
        active_platform_ids.append(x)
    if detect.can_build():
        x = x.replace("platform/", "")
        x = x.replace("platform\\", "")
        platform_list += [x]
        platform_opts[x] = detect.get_opts()
        platform_flags[x] = detect.get_flags()
    sys.path.remove(tmppath)
    sys.modules.pop("detect")
    
methods.save_active_platforms(active_platforms, active_platform_ids)

custom_tools = ["default"]

platform_arg = ARGUMENTS.get("platform", ARGUMENTS.get("p", False))

if os.name == "nt" and (platform_arg == "android" or methods.get_cmdline_bool("use_mingw", False)):
    custom_tools = ["mingw"]
elif platform_arg == "javascript":
    custom_tools = ["cc", "c++", "ar", "link", "textfile", "zip"]
    
env_base = Environment(tools=custom_tools)
env_base.PrependENVPath("PATH", os.getenv("PATH"))
env_base.PrependENVPath("PKG_CONFIG_PATH", os.getenv("PKG_CONFIG_PATH"))
if "TERM" in os.environ:
    env_base["ENV"]["TERM"] = os.environ["TERM"]
    
env_base.disabled_modules = []
env_base.module_version_string = ""
env_base.msvc = False

env_base.__class__.disable_module = methods.disable_module

env_base.__class__.add_module_version_string = methods.add_module_version_string

env_base.__class__.add_source_files = methods.add_source_files
env_base.__class__.use_windows_spawn_fix = methods.use_windows_spawn_fix

env_base.__class__.add_shared_library = methods.add_shared_library
env_base.__class__.add_library = methods.add_library
env_base.__class__.add_program = methods.add_program
env_base.__class__.CommandNoCache = methods.CommandNoCache
env_base.__class__.Run = methods.Run
env_base.__class__.disable_warnings = methods.disable_warnings
env_base.__class__.module_check_dependencies = methods.module_check_dependencies

env_base["x86_libtheora_opt_gcc"] = False
env_base["x86_libtheora_opt_vc"] = False

env_base.SConsignFile(".sconsign{0}.dblite".format(pickle.HIGHEST_PROTOCOL))

customs = ["custom.py"]

profile = ARGUMENTS.get("profile", "")
if profile:
    if os.path.isfile(profile):
        customs.append(profile)
    elif os.path.isfile(profile + ".py"):
        customs.append(profile + ".py")
        
opts = Variables(customs, ARGUMENTS)

opts.Add("p", "Platform (alias for 'platform')", "")
opts.Add("platform", "Target platform (%s)" % ("|".join(platform_list),), "")
opts.Add(BoolVariable("tools", "Build the tools (a.k.a. the Godot editor)", True))
opts.Add(EnumVariable("target", "Compilation target", "debug", ("debug", "release_debug", "release")))
opts.Add("arch", "Platform-dependent architecture (arm/arm64/x86/x64/mips/...)", "")
opts.Add(EnumVariable("bits", "Target platform bits", "default", ("default", "32", "64")))
opts.Add(EnumVariable("float", "Floating-point precision", "default", ("debug", "32", "64")))
opts.Add(EnumVariable("optimize", "Optimization type", "speed", ("speed", "size", "none")))
opts.Add(BoolVariable("production", "Set defaults to build Godot for use in production", False))
opts.Add(BoolVariable("use_lto", "Use link-time optimization", False))

opts.Add(BoolVariable("deprecated", "Enable deprecated features", True))
opts.Add(BoolVariable("minizip", "Enable ZIP archive support using minizip", True))
opts.Add(BoolVariable("xaudio2", "Enable the XAudio2 audio driver", False))
opts.Add(BoolVariable("vulkan", "Enable the vulkan video driver", True))
opts.Add("custom_modules", "A list of comma-seperated directory paths containing custom modules to build.", "")
opts.Add(BoolVariable("custom_modules_recursive", "Detect custom modules recursively for each specified path.", True))
opts.Add(BoolVariable("use_volk", "Use the volk library to load the Vulkan loader dynamically", True))

opts.Add(BoolVariable("dev", "If yes, alias for verbose=yes warnings=extra werror=yes", False))
opts.Add(BoolVariable("progress", "Show a progress indicator during compilation", True))
opts.Add(BoolVariable("tests", "Build the unit tests", False))
opts.Add(BoolVariable("verbose", "Enable verbose output for the compilation", False))
opts.Add(EnumVariable("warnings", "Level of compilation warnings", "all", ("extra", "all", "moderate", "no")))
opts.Add(BoolVariable("werror", "Treat compiler warnings as errors", False))
opts.Add("extra_suffix", "Custom extra suffix added to the base filename or all generated binary files", "")
opts.Add(BoolVariable("vsproj", "Generate a Visual Studio soluation", False))
opts.Add(BoolVariable("disable_3d", "Disable 3D nodes for a smaller executable", False))
opts.Add(BoolVariable("disable_advanced_gui", "Disable advanced GUI nodes and behaviors", False))
opts.Add("disable_classes", "Disable given classes (comma seperated)", "")
opts.Add(BoolVariable("modules_enabled_by_default", "If no, disable all modules except ones explicitly enabled", True))
opts.Add(BoolVariable("no_editor_splash", "Don't use the custom splash screen for the editor", False))
opts.Add("system_certs_path", "Use this path as SSL certificates default for editor (for package maintainers)", "")
opts.Add(BoolVariable("use_precise_math_checks", "Math checks use very precise epsilon (debug option)", False))

opts.Add(BoolVariable("builtin_bullet", "Use the built-in Bullet library", True))
opts.Add(BoolVariable("builtin_certs", "Use the built-in SSL certificates bundles", True))
opts.Add(BoolVariable("builtin_embree", "Use the built-in Embree library", True))
opts.Add(BoolVariable("builtin_enet", "Use the built-in ENet library", True))
opts.Add(BoolVariable("builtin_freetype", "Use the built-in FreeType library", True))
opts.Add(BoolVariable("builtin_msdfgen", "Use the built-in MSDFgen library", True))
opts.Add(BoolVariable("builtin_glslang", "Use the built-in glslang library", True))
opts.Add(BoolVariable("builtin_graphite", "Use the built-in Graphite library", True))
opts.Add(BoolVariable("builtin_harfbuzz", "Use the built-in HarfBuzz library", True))
opts.Add(BoolVariable("builtin_icu", "Use the built-in ICU library", True))
opts.Add(BoolVariable("builtin_libogg", "Use the built-in libogg library", True))
opts.Add(BoolVariable("builtin_libtheora", "Use the built-in libtheora library", True))
opts.Add(BoolVariable("builtin_libvorbis", "Use the built-in libvorbis library", True))
opts.Add(BoolVariable("builtin_libvpx", "Use the built-in libvpx library", True))
opts.Add(BoolVariable("builtin_libwebp", "Use the built-in libwebp library", True))
opts.Add(BoolVariable("builtin_wslay", "Use the built-in wslay library", True))
opts.Add(BoolVariable("builtin_mbedtls", "Use the built-in mbedTLS library", True))
opts.Add(BoolVariable("builtin_miniupnpc", "Use the built-in miniupnpc library", True))
opts.Add(BoolVariable("builtin_opus", "Use the built-in Opus library", True))
opts.Add(BoolVariable("builtin_pcre2", "Use the built-in PCRE2 library", True))
opts.Add(BoolVariable("builtin_pcre2_with_jit", "Use JIT compiler for the built-in PCRE2 library", True))
opts.Add(BoolVariable("builtin_recast", "Use the built-in Recast library", True))
opts.Add(BoolVariable("builtin_rvo2", "Use the built-in RVO2 library", True))
opts.Add(BoolVariable("builtin_squish", "Use the built-in squish library", True))
opts.Add(BoolVariable("builtin_xatlas", "Use the built-in xatlas library", True))
opts.Add(BoolVariable("builtin_zlib", "Use the built-in zlib library", True))
opts.Add(BoolVariable("builtin_zstd", "Use the built-in Zstd library", True))

opts.Add("CXX", "C++ compiler")
opts.Add("CC", "C compiler")
opts.Add("LINK", "Linker")
opts.Add("CCFLAGS", "Custom flags for both the C and C++ compilers")
opts.Add("CFLAGS", "Custom flags for the C compiler")
opts.Add("CXXFLAGS", "Custom flags for the C++ compiler")
opts.Add("LINKFLAGS", "Custom flags for the linker")

opts.Update(env_base)

selected_platform = ""

if env_base["platform"] != "":
    selected_platform = env_base["platform"]
elif env_base["p"] != "":
    selected_platform = env_base["p"]
else:
    if (
        sys.platform.startswith("linux")
        or sys.platform.startswith("dragonfly")
        or sys.platform.startswith("freebsd")
        or sys.platform.startswith("netbsd")
        or sys.platform.startswith("openbsd")
    ):
        selected_platform = "linuxbsd"
    elif sys.platform == "darwin":
        selected_platform = "osx"
    elif sys.platform == "win32":
        selected_platform = "windows"
    else:
        print("Could not detect platform automatically. Supported platforms:")
        for x in platform_list:
            print("\t" + x)
        print("\nPlease run SCons again and select a valid platform: platform=<string>")
        
    if selected_platform != "":
        print("Automatically detected platform: " + selected_platform)
        
if selected_platform in ["linux", "bsd", "x11"]:
    if selected_platform == "x11":
        print('Platform "x11" has been renamed to "linuxbsd" in Godot 4.0. Building for platform "linuxbsd".')
    selected_platform = "linuxbsd"
    
env_base["platform"] = selected_platform

if selected_platform in platform_opts:
    for opt in platform_opts[selected_platform]:
        opts.Add(opt)
        
opts.Update(env_base)
env_base["platform"] = selected_platform

modules_detected = OrderedDict()
module_search_paths = ["modules"]

if env_base["custom_modules"]:
    paths = env_base["custom_modules"].split(",")
    for p in paths:
        try:
            module_search_paths.append(methods.convert_custom_modules_path(p))
        except ValueError as e:
            print(e)
            Exit(255)
            
for path in module_search_paths:
    if path == "modules":
        modules = methods.detect_modules(path, recursive=False)
    else:
        modules = methods.detect_modules(path, recursive=True)
        
        env_base.Prepend(CPPPATH=[path, os.path.dirname(path)])
        
    modules_detected.update(modules)
    
for name, path in modules_detected.items():
    if env_base["modules_enabled_by_default"]:
        enabled = True
        
        sys.path.insert(0, path)
        import config
        
        try:
            enabled = config.is_enabled()
        except AttributeError:
            pass
        sys.path.remove(path)
        sys.modules.pop("config")
    else:
        enabled = False
        
    opts.Add(BoolVariable("module_" + name + "_enabled", "Enable module '%s'" % (name,), enabled))
    
methods.write_modules(modules_detected)

opts.Update(env_base)
env_base["platform"] = selected_platform
Help(opts.GenerateHelpText(env_base))

env_base.Prepend(CPPPATH=["#"])

env_base.platform_exporters = platform_exporters
env_base.platform_apis = platform_apis

if env_base["use_precise_math_checks"]:
    env_base.Append(CPPDEFINES=["PRECISE_MATH_CHECKS"])
    
if env_base["target"] == "debug":
    env_base.Append(CPPDEFINES=["DEBUG_MEMORY_ALLOC", "DISABLE_FORCED_INLINE"])
    
    env_base.Decider("MD5-timestamp")
    
    env_base.SetOption("implicit_cache", 1)
    
if env_base["no_editor_splash"]:
    env_base.Append(CPPDEFINES=["NO_EDITOR_SPLASH"])
    
if not env_base["deprecated"]:
    env_base.Append(CPPDEFINES=["DISABLE_DEPRECATED"])
    
if env_base["float"] == "64":
    env_base.Append(CPPDEFINES=["REAL_T_IS_DOUBLE"])
    
if selected_platform in platform_list:
    tmppath = "./platform/" + selected_platform
    sys.path.insert(0, tmppath)
    import detect
    
    if "create" in dir(detect):
        env = detect.create(env_base)
    else:
        env = env_base.Clone()
        
    from SCons import __version__ as scons_raw_version
    
    scons_ver = env._get_major_minor_revision(scons_raw_version)
    
    if scons_ver >= (4, 0, 0):
        env.Tool("compilation_db")
        env.Alias("compiledb", env.CompilationDatabase())
        
    if env["dev"]:
        env["verbose"] = methods.get_cmdline_bool("verbose", True)
        env["warnings"] = ARGUMENTS.get("warnings", "extra")
        env["werror"] = methods.get_cmdline_bool("werror", True)
        if env["tools"]:
            env["tests"] = methods.get_cmdline_bool("tests", True)
    if env["production"]:
        env["use_static_cpp"] = methods.get_cmdline_bool("use_static_cpp", True)
        env["use_lto"] = methods.get_cmdline_bool("use_lto", True)
        env["debug_symbols"] = methods.get_cmdline_bool("debug_symbols", False)
        if not env["tools"] and env["target"] == "debug":
            print(
                "WARNING: Requested `production` build with `tools=no target=debug`, "
                "this will give you a full debug template (use `target=release_debug` "
                "for an optimized template with debug features)."
            )
        if env.msvc:
            print(
                "WARNING: For `production` Windows builds, you should use MinGW with GCC "
                "or Clang instead of Visual Studio, as they can better optimize the "
                "GDScript VM in a very significant way. MSVC LTO also doesn't work "
                "reliably for our use case."
                "If you want to use MSVC nevertheless for production builds, set "
                "`debug_symbols=no use_lto=no` instead of the `production=yes` option."
            )
            Exit(255)
    
    env.extra_suffix = ""
    
    if env["extra_suffix"] != "":
        env.extra_suffix += "." + env["extra_suffix"]
        
    CCFLAGS = env.get("CCFLAGS", "")
    env["CCFLAGS"] = ""
    env.Append(CCFLAGS=str(CCFLAGS).split())
    
    CFLAGS = env.get("CFLAGS", "")
    env["CFLAGS"] = ""
    env.Append(CFLAGS=str(CFLAGS).split())
    
    CXXFLAGS = env.get("CXXFLAGS", "")
    env["CXXFLAGS"] = ""
    env.Append(CXXFLAGS=str(CXXFLAGS).split())
    
    LINKFLAGS = env.get("LINKFLAGS", "")
    env["LINKFLAGS"] = ""
    env.Append(LINKFLAGS=str(LINKFLAGS).split())
    
    flag_list = platform_flags[selected_platform]
    for f in flag_list:
        if not (f[0] in ARGUMENTS):
            env[f[0]] = f[1]
            
    detect.configure(env)
    
    if not env.msvc:
        env.Prepend(CFLAGS=["-std=gnu11"])
        env.Prepend(CXXFLAGS=["-std=gnu++17"])
    else:
        env.Prepend(CCFLAGS=["/std:c++17"])
        
    cc_version = methods.get_compiler_version(env) or {
        "major": None,
        "minor": None,
        "patch": None,
        "metadata1": None,
        "metadata2": None,
        "date": None
    }
    cc_version_major = int(cc_version["major"] or -1)
    cc_version_minor = int(cc_version["minor"] or -1)
    cc_version_metadata1 = cc_version["metadata1"] or ""
    
    if methods.using_gcc(env):
        if cc_version_major == -1:
            print(
                "Couldn't detect compiler version, skipping version checks. "
                "Build may fail if the compiler doesn't support C++17 fully."
            )
        elif cc_version_major == 8 and cc_version_minor < 4:
            print(
                "Detected GCC 8 version < 8.4, which is not supported due to a "
                "regression in its C++17 guaranteed copy elision support. Use a "
                'newer GCC version, or Clang 6 or later by passing "use_llvm=yes" '
                "to the SCons command line."
            )
            Exit(255)
        elif cc_version_major < 7:
            print(
                "Detected GCC version older than 7, which does not fully support "
                "C++17. Supported versions are GCC 7, 9 and later. Use a newer GCC "
                'version, or Clang 6 or later by passing "use_llvm=yes" to the '
                "SCons command line."
            )
            Exit(255)
        elif cc_version_metadata1 == "win32":
            print(
                "Detected mingw version is not using posix threads. Only posix "
                "version of mingw is supported. "
                'Use "update-alternatives --config <platform>-w64-mingw32-[gcc|g++]" '
                "to switch to posix threads."
            )
            Exit(255)
    elif methods.using_clang(env):
        if cc_version_major == -1:
            print(
                "Couldn't detect compiler version, skipping version checks. "
                "Build may fail if the compiler doesn't support C++17 fully."
            )
        elif env["platform"] == "osx" or env["platform"] == "iphone":
            vanilla = methods.is_vanilla_clang(env)
            if vanilla and cc_version_major < 6:
                print(
                    "Detected Clang version older than 6, which does not fully support "
                    "C++17. Supported versions are Clang 6 and later."
                )
                Exit(255)
            elif not vanilla and cc_version_major < 10:
                print(
                    "Detected Apple Clang version older than 10, which does not fully "
                    "support C++17. Supported versions are Apple Clang 10 and later."
                )
                Exit(255)
        elif cc_version_major < 6:
            print(
                "Detected Clang version older than 6, which does not fully support "
                "C++17. Supported versions are Clang 6 and later."
            )
            Exit(255)
            
    if env.msvc:
        disable_nonessential_warnings = ["/wd4267", "/wd4244", "/wd4305", "/wd4018", "/wd4800"]
        if env["warnings"] == "extra":
            env.Append(CCFLAGS=["/Wall"])
        
            
