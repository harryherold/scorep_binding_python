#!/usr/bin/sh

python_version=$(python -c 'import platform as p; print("{}.{}".format(p.sys.version_info.major,p.sys.version_info.minor))')

###########################
# Prepare Python bindings #
###########################

scorep_python_dir=
export PYTHONPATH=$scorep_python_dir/lib/python$python_version/site-packages/
export LD_LIBRARY_PATH=$PYTHONPATH/scorep:$LD_LIBRARY_PATH

###########################
#   Prepare preloading    #
###########################
# Clean existing preload libs
scorep-preload-init --clean $PWD
# Create preload library
preload=$(scorep-preload-init --user --nocompiler --io=runtime:posix --value-only)

###########################
#   Set Score-P Config    #
###########################
export SCOREP_EXPERIMENT_DIRECTORY=$PWD/trace
export SCOREP_TOTAL_MEMORY=300M
export SCOREP_ENABLE_TRACING=true

LD_PRELOAD="$preload" $@

