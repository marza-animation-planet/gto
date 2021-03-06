import os
import sys
import glob
import excons
import excons.tools.python
import excons.tools.zlib
import excons.tools.houdini


python_prefix = excons.tools.python.ModulePrefix() + "/" + excons.tools.python.Version()

lib_defs = []
lib_cppflags = ""
lib_incdirs = ["lib"]
lib_extrasrcs = []

cmn_custom = []


env = excons.MakeBaseEnv()

if sys.platform == "win32":
    lib_defs.append("REGEX_STATIC")
    lib_defs.append("_SCL_SECURE_NO_WARNINGS")
    lib_defs.append("_CRT_SECURE_NO_WARNINGS")
    lib_incdirs.append("regex/src")
    lib_extrasrcs = ["regex/src/regex.c"]
    # Shutup a bunch of annoying warnings
    if excons.warnl != "all":
        lib_cppflags += " -wd4267 -wd4244 -wd4018 -wd4065 -wd4251"

if excons.GetArgument("gto-use-zlib", 0, int) != 0:
    lib_defs.append("GTO_SUPPORT_ZIP")
    cmn_custom.append(excons.tools.zlib.Require)
    if sys.platform == "win32":
        zlib_inc, zlib_lib = excons.GetDirs("zlib", silent=True)
        if zlib_inc is None and zlib_lib is None:
            zlib_dir = os.path.abspath("zlib-1.2.3")
            if not os.path.isdir(zlib_dir):
                print("Extracting 'zlib-1.2.3.zip' to 'zlib-1.2.3'...")
                import zipfile
                zf = zipfile.ZipFile("zlib-1.2.3.zip")
                zf.extractall()
            print("Use self-provided zlib (1.2.3)")
            excons.SetArgument("with-zlib-inc", "%s/include" % zlib_dir)
            excons.SetArgument("with-zlib-lib", "%s/lib/%s" % (zlib_dir, excons.arch_dir))
            excons.SetArgument("zlib-static", 1)

gto_headers = env.Install(excons.OutputBaseDirectory() + "/include/Gto", glob.glob("lib/Gto/*.h"))

prjs = [
    {
        "name": "gtoLib_s",
        "alias": "lib",
        "type": "staticlib",
        "desc": "GTO static library",
        "defs": lib_defs + ["GTO_STATIC"],
        "cppflags": lib_cppflags,
        "incdirs": lib_incdirs,
        "srcs": glob.glob("lib/Gto/*.cpp") + lib_extrasrcs,
        "custom": cmn_custom
    },
    {
        "name": "gtoLib",
        "alias": "lib",
        "type": "sharedlib",
        "desc": "GTO shared library",
        "defs": lib_defs + ["GTO_EXPORT"],
        "cppflags": lib_cppflags,
        "incdirs": lib_incdirs,
        "srcs": glob.glob("lib/Gto/*.cpp") + lib_extrasrcs,
        "custom": cmn_custom
    },
    {
        "name": "_gto",
        "alias": "python",
        "type": "dynamicmodule",
        "desc": "GTO python module",
        "prefix": python_prefix,
        "ext": excons.tools.python.ModuleExtension(),
        "defs": ["GTO_STATIC"],
        "incdirs": ["plugins/python/src/gto"],
        "srcs": glob.glob("plugins/python/src/gto/*.cpp"),
        "libs": ["gtoLib_s"],
        "custom": [excons.tools.python.SoftRequire] + cmn_custom,
        "install": {python_prefix: ["python/gto/gto.py",
                                    "python/gtoContainer/gtoContainer.py"]}
    },
    {
        "name": "gtoinfo",
        "alias": "tools",
        "type": "program",
        "desc": "GTO command line tool",
        "defs": ["GTO_STATIC"],
        "srcs": ["bin/gtoinfo/main.cpp"],
        "libs": ["gtoLib_s"],
        "custom": cmn_custom
    },
    {
        "name": "SOP_GtoImport",
        "alias": "houdini",
        "type": "dynamicmodule",
        "prefix": "houdini/%s" % excons.tools.houdini.Version(),
        "ext": excons.tools.houdini.PluginExt(),
        "defs": ["GTO_STATIC"],
        "incdirs": lib_incdirs + ["plugins/houdini/sop"],
        "srcs": glob.glob("plugins/houdini/sop/*.cpp"),
        "libs": ["gtoLib_s"],
        "custom": [excons.tools.houdini.Require, excons.tools.houdini.Plugin] + cmn_custom
    },
    {
        "name": "ROP_GtoExport",
        "alias": "houdini",
        "type": "dynamicmodule",
        "prefix": "houdini/%s" % excons.tools.houdini.Version(),
        "ext": excons.tools.houdini.PluginExt(),
        "defs": ["GTO_STATIC"],
        "incdirs": lib_incdirs + ["plugins/houdini/rop"],
        "srcs": glob.glob("plugins/houdini/rop/*.cpp"),
        "libs": ["gtoLib_s"],
        "custom": [excons.tools.houdini.Require, excons.tools.houdini.Plugin] + cmn_custom,
        "install": {"houdini/otls/%s" % excons.tools.houdini.Version(full=False): glob.glob("plugins/houdini/rop/*.otl")}
    }
]

build_opts = "GTO OPTIONS\n  gto-use-zlib=0|1 : Enable zlib compression. [0]"
excons.AddHelpOptions(gto=build_opts)
if sys.platform != "win32":
    excons.AddHelpOptions(zlib=excons.tools.zlib.GetOptionsString())

targets = excons.DeclareTargets(env, prjs)

env.Depends(targets["gtoLib_s"], gto_headers)
env.Depends(targets["gtoLib"], gto_headers)

def RequireGto(static=False):
   def _RealRequire(env):
      env.Append(LIBS=["gtoLib%s" % ("_s" if static else "")])
      if static:
         env.Append(CPPDEFINES=["GTO_STATIC"])
         if excons.GetArgument("gto-use-zlib", 0, int) != 0:
            excons.tools.zlib.Require(env)
      excons.AddHelpOptions(gto=build_opts)

   return _RealRequire

Export("RequireGto")
