"""
WAF Tool to select the correct toolchain based on the target archtecture.

This tool loads compiler_c and compiler_cxx, so you don't need to load them
(and you must not load them before this tool). Use the environment variable
TOOLCHAIN to define the toolchain.

Example::

    def configure(cfg):
        cfg.env.TOOLCHAIN = 'arm-linux-gnueabihf'
        cfg.load('toolchain')
"""

from waflib import Errors, Context, Utils
from waflib.Tools import clang, clangxx, gcc, gxx

import os

def find_realexec_path(cfg, filename, path_list=[]):
    if not filename:
        return ''

    if not path_list:
        path_list = cfg.environ.get('PATH','').split(os.pathsep)

    for dir in path_list:
        path = os.path.abspath(os.path.expanduser(os.path.join(dir, filename)))

        if os.path.isfile(path):
            if os.path.islink(path):
                realpath = os.path.realpath(path)

                if filename not in os.path.basename(realpath):
                    continue
                else:
                    return os.path.dirname(realpath)

            else:
                return os.path.dirname(path)

    cfg.fatal('Could not find real exec path to %s in path_list %s:' % (filename, path_list))

def _set_toolchain_prefix_wrapper(tool_module, var, compiler_names):
    original_configure = tool_module.configure
    def new_configure(cfg):
        if cfg.env.TOOLCHAIN == 'native':
            original_configure(cfg)
            return

        last_exception = None
        for name in compiler_names:
            cfg.env.stash()
            try:
                cfg.env[var] = '%s-%s' % (cfg.env.TOOLCHAIN, name)
                original_configure(cfg)
            except Errors.ConfigurationError as e:
                cfg.env.revert()
                last_exception = e
            else:
                cfg.env.commit()
                return
        raise last_exception
    tool_module.configure = new_configure

_set_toolchain_prefix_wrapper(gxx, 'CXX', ('g++', 'c++'))
_set_toolchain_prefix_wrapper(gcc, 'CC', ('gcc', 'cc'))

def _clang_cross_support(cfg):
    if _clang_cross_support.called:
        return

    prefix = cfg.env.TOOLCHAIN + '-'

    cfg.find_program(prefix + 'gcc', var='CROSS_GCC')
    toolchain_path = os.path.join(find_realexec_path(cfg, prefix + 'ar'),
                                  '..')
    toolchain_path = os.path.abspath(toolchain_path)

    cfg.msg('Using toolchain path for clang', toolchain_path)

    sysroot = cfg.cmd_and_log(
        [cfg.env.CROSS_GCC[0], '--print-sysroot'],
        quiet=Context.BOTH,
    ).strip()

    cfg.env.CLANG_FLAGS = [
        '--target=' + cfg.env.TOOLCHAIN,
        '--gcc-toolchain=' + toolchain_path,
        '--sysroot=' + sysroot,
        '-B' + os.path.join(toolchain_path, 'bin')
    ]

_clang_cross_support.called = False

def _set_clang_crosscompilation_wrapper(tool_module):
    original_configure = tool_module.configure
    def new_configure(cfg):
        if cfg.env.TOOLCHAIN == 'native':
            original_configure(cfg)
            return

        cfg.env.stash()
        try:
            _clang_cross_support(cfg)
            original_configure(cfg)
        except Errors.ConfigurationError as e:
            cfg.env.revert()
            raise
        else:
            cfg.env.commit()
    tool_module.configure = new_configure

_set_clang_crosscompilation_wrapper(clang)
_set_clang_crosscompilation_wrapper(clangxx)

def configure(cfg):
    if cfg.env.TOOLCHAIN == 'native':
        cfg.load('compiler_cxx compiler_c')
        return

    cfg.env.AR = cfg.env.TOOLCHAIN + '-ar'
    cfg.msg('Using toolchain', cfg.env.TOOLCHAIN)
    cfg.load('compiler_cxx compiler_c')

    if cfg.env.COMPILER_CC == 'clang':
        cfg.env.CFLAGS += cfg.env.CLANG_FLAGS
        cfg.env.LINKFLAGS_cprogram += cfg.env.CLANG_FLAGS

    if cfg.env.COMPILER_CXX == 'clang++':
        cfg.env.CXXFLAGS += cfg.env.CLANG_FLAGS
        cfg.env.LINKFLAGS_cxxprogram += cfg.env.CLANG_FLAGS
