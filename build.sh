#!/bin/bash
scons -j4 target=template_debug debug_symbols=yes
scons -j4 target=template_release
