#!/bin/bash
#!!!!!!!!!!!!!!!!!!!!! Don't ferget to set CRLF to LF !!!!!!!!!!!!!!!!!!!!!
echo 'Start shell script '$0

function getRequiredModules()
{
    # get branchname
    BRANCH_NAME=$(git branch --show-current)
    cat docs/dsgn/$BRANCH_NAME.spec
}

function clearWDir()
{
    rm -fr $1 
    mkdir  $1

}
function analyseAll()
{
    for module in "${@}"
    do
        mkdir ./output/CPAchecker/$module
        ./exec/tools/cpachecker/scripts/cpa.sh \
        -default \
        -outputpath ./output/CPAchecker/$module \
        -setprop analysis.checkCounterexamples=false \
        -setprop solver.solver=SMTInterpol \
        -setprop cpa.predicate.encodeBitvectorAs=INTEGER \
        -setprop cpa.predicate.encodeFloatAs=RATIONAL \
        ./output/prebuild/$module.p.c
    done
}


# Get List of Required Modules 
MODULE_LIST=$(getRequiredModules)

# Clear Working Directory
clearWDir output/CPAchecker

# Check Every Module
analyseAll $MODULE_LIST

exit 0
#EOF