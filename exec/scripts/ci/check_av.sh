#!/bin/bash
#!!!!!!!!!!!!!!!!!!!!! Don't ferget to set CRLF to LF !!!!!!!!!!!!!!!!!!!!!
echo 'Start shell script '$0

function getRequiredModules()
{
    # get branchname
    cat $1
}

function checkAvailabilityViaCommits()
{
    local IS_PART_OF=0
    local RESULT='FAILURE'
    local LIST_TO_CHECK
    if [ $1 = "OFF" ]
    then
        LIST_TO_CHECK=$(git log --pretty=format:"%h %s" | grep "Module(off)" | sed -n 's/.*Module(off): \([a-zA-Z0-9]*\).*/\1/p')
    else
        LIST_TO_CHECK=$(git log --pretty=format:"%h %s" | grep "Module(off)" | sed -n 's/.*Module(on): \([a-zA-Z0-9]*\).*/\1/p')
    fi
    echo $LIST_TO_CHECK
    for ELEMENT_M in ${@:2}
    do
        for ELEMENT_N in "${LIST_TO_CHECK[@]}"
        do
            echo "Check: <$ELEMENT_M | $ELEMENT_N>"
            if [ "$ELEMENT_N" = "$ELEMENT_M" ] || [ -n "$ELEMENT_N" ]
            then 
                echo "Match: Yes"
                IS_PART_OF=1
                break
            else 
                echo "Match: No"
            fi
        done
        echo "IS_PART_OF: $IS_PART_OF"
        echo "LIST_TO_CHECK: $LIST_TO_CHECK"
        if [ "$IS_PART_OF" -eq 0 ]
        then
            echo "$1 Target:"
            echo "Test of $ELEMENT_N not available"
            exit 1
        else
            IS_PART_OF=0
        fi
    done 
    echo "All Module Tests are available"
    return 0
}

BRANCH_NAME=$(git branch --show-current)
# Get List of Requirde OffTarget Tests
OFF_TAR_LIST=$(getRequiredModules ./docs/dsgn/ci_specs/$BRANCH_NAME.spec)

# Get List of Requirde OnTarget Tests
ON_TAR_LIST=$(getRequiredModules ./docs/dsgn/ci_specs/onTargetMTests/$BRANCH_NAME.spec)

# Check for availability of Tests
checkAvailabilityViaCommits OFF $OFF_TAR_LIST && checkAvailabilityViaCommits ON $ON_TAR_LIST && exit 0 || exit 1
#EOF