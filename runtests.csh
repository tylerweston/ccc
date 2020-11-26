#!/bin/csh
foreach f ( ./tests/* )
    valgrind ./build/src/ece467c 3 $f
    echo $f
    cat $f
    set input = "$<"
    set answer = `awk -v var="$input" 'BEGIN {print substr(var,1,1)}'`
    if ($answer == y || $answer == Y) echo ""
end

