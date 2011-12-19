#!/bin/sh
#
# Author: Ben Porter <benjamin.porter@gmail.com>
# Hacked from the example at:
#   http://www.vtk.org/Wiki/CMake:CPackPackageGenerators

FUGU_BUNDLE="`echo "$0" | sed -e 's/\/Contents\/MacOS\/FuguOSX//'`"
FUGU_RESOURCES="$FUGU_BUNDLE/Contents/Resources"

echo "running $0"
echo "FUGU_BUNDLE: $FUGU_BUNDLE"

# Setup temporary runtime files
export "DYLD_LIBRARY_PATH=$FUGU_RESOURCES/lib"
export "PATH=$FUGU_RESOURCES:$PATH"

# export
exec "$FUGU_RESOURCES/fugu" 
# TODO: may need extra command line args ... 
# "--log-level=debug" "--plugins=$K3D_RESOURCES/lib/k3d/plugins" "--share=$K3D_RESOURCES/share/k3d" "--ui=$K3D_RESOURCES/lib/k3d/uiplugins/k3d-ngui.module"

