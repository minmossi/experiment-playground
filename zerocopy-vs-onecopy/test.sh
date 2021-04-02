#!/bin/bash
echo ""
echo "<One-Copy Result>"
sudo perf stat -B ./onecopy $1 $2 $3
sudo turbostat --quiet --Summary --Joules --show Pkg_J --show RAM_J --show IRQ \
    --show Pkg%pc2 --show Pkg%pc3 --show Pkg%pc6 --show Pkg%pc7 --show Busy% --show Avg_MHz --show PKG_% --show RAM_% ./onecopy $1 $2 $3
echo ""
echo "-----------------------------------------------------------------------------------------------------------------------------------"
echo ""
echo "<Zero-Copy Result>"
sudo perf stat -B ./zerocopy $1 $2 $3
sudo turbostat --quiet --Summary --Joules --show Pkg_J --show RAM_J --show IRQ \
    --show Pkg%pc2 --show Pkg%pc3 --show Pkg%pc6 --show Pkg%pc7 --show Busy% --show Avg_MHz --show PKG_% --show RAM_% ./zerocopy $1 $2 $3
echo ""
echo "-----------------------------------------------------------------------------------------------------------------------------------"