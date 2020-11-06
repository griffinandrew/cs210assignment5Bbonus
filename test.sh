#!/bin/bash
#set -x

REFERENCE=./reference_assignment-5
PGM=./assignment-5_memtrace

# To add a test you can add an element the REQUIRED arrays 
# or BONUS Arrays
# Each test can have a descripttion, input and optionally a num
# num is passed as the argument on the command line to set
# the default max schedule size

# Required tests
REQUIRED_DESC[0]="Simple test of pushing on number on the stack"
REQUIRED_INPUT[0]="10"

REQUIRED_DESC[1]="Add two numbers"
REQUIRED_INPUT[1]="1 + 1"

REQUIRED_DESC[2]="Subtract"
REQUIRED_INPUT[2]="1 - 1"

REQUIRED_DESC[3]="Multiply"
REQUIRED_INPUT[3]="3 * 2"

REQUIRED_DESC[4]="Divide"
REQUIRED_INPUT[4]="6 / 2"

REQUIRED_DESC[5]="Two commands"
REQUIRED_INPUT[5]="1 + 1
3 * 2"

REQUIRED_DESC[6]="Subtraction is ordered"
REQUIRED_INPUT[6]="3 - 2"

REQUIRED_DESC[7]="Division is ordered"
REQUIRED_INPUT[7]="10 / 2"

REQUIRED_DESC[8]="Divide by 0"
REQUIRED_INPUT[8]="5 / 0"

REQUIRED_DESC[9]="Basic parens"
REQUIRED_INPUT[9]="( 3 + 2 )"

REQUIRED_DESC[10]="In and out of parens"
REQUIRED_INPUT[10]="( 1 + 2 ) + 3"

REQUIRED_DESC[11]="Parens with priority"
REQUIRED_INPUT[11]="( 1 + 2 ) * 3"

REQUIRED_DESC[12]="Divide by zero with parens"
REQUIRED_INPUT[12]="8 / ( 2 - 2 )"

REQUIRED_DESC[13]="Priority weave"
REQUIRED_INPUT[13]="1 + 2 * 3 + 4 * 5 - 6 / 7 - 8 * 9 + 10"

REQUIRED_DESC[14]="Not divide by zero"
REQUIRED_INPUT[14]="0 / 2"

REQUIRED_DESC[15]="Continue after failed command"
REQUIRED_INPUT[15]="1 / 0
1 + 1"

REQUIRED_DESC[16]="Bad operator format"
REQUIRED_INPUT[16]="1 + + 2
2 * 2 *"

REQUIRED_DESC[17]="Unmatched left paren"
REQUIRED_INPUT[17]="( 5 * 6"

REQUIRED_DESC[18]="Unmatched right paren"
REQUIRED_INPUT[18]="9 / 3 )"

REQUIRED_DESC[19]="Extra data"
REQUIRED_INPUT[19]="5 + 5 / 3 9 67 13 12"

REQUIRED_DESC[20]="No data"
REQUIRED_INPUT[20]="+ * - /"

REQUIRED_DESC[21]="Bonus 1: A small test "
REQUIRED_INPUT[21]="$(cat smalldata.txt)"
REQUIRED_VALUE[21]=2

# Bonus tests

BONUS_DESC[0]="Bonus 2: Medium sized test"
BONUS_INPUT[0]="$(cat mediumdata.txt)"
BONUS_VALUE[0]=3

BONUS_DESC[1]="Bonus 3: Large sized test"
BONUS_INPUT[1]="$(cat largedata.txt)"
BONUS_VALUE[1]=5

function run_test()
{
   description="$1"
   input="$2"
   num="$3"
   type=$4

   echo -n "$description : "
   [[ -n $VERBOSE ]] &&  echo "Input:"
   [[ -n $VERBOSE ]] &&  echo "--------------------------"
   [[ -n $VERBOSE ]] &&  echo "$input"
   [[ -n $VERBOSE ]] &&  echo "--------------------------"

   [[ -a ${type}_${num}_ref.test.out ]] && rm ${type}_${num}_ref.test.out
   [[ -a ${type}_${num}_ans.test.out ]] && rm ${type}_${num}_ans.test.out
   [[ -a ${type}_${num}_ans.calc.out ]] && rm ${type}_${num}_ans.calc.out
   [[ -a ${type}_${num}_ans.mem.out ]] && rm ${type}_${num}_ans.mem.out
   [[ -a ${type}_${num}_ref.calc.out ]] && rm ${type}_${num}_ref.calc.out

   if [[ -x $REFERENCE ]]; then
       $REFERENCE $num > ${type}_${num}_ref.test.out 2>&1 <<EOF
$input
EOF
      
   else 
       touch ${type}_${num}_ref.test.out
   fi
   if [[ -x $PGM ]]; then
     $PGM $num > ${type}_${num}_ans.test.out 2>&1 <<EOF
$input
EOF

   else
       touch ${type}_${num}_ans.test.out
   fi

   [[ -a ${type}_${num}_ans.test.out ]] && grep -v memtrace ${type}_${num}_ans.test.out > ${type}_${num}_ans.calc.out
   [[ -a ${type}_${num}_ans.test.out ]] && grep memtrace ${type}_${num}_ans.test.out > ${type}_${num}_ans.mem.out
   [[ -a ${type}_${num}_ref.test.out ]] && grep -v memtrace ${type}_${num}_ref.test.out > ${type}_${num}_ref.calc.out

   calcoutdiffcnt="$(diff ${type}_${num}_ans.calc.out ${type}_${num}_ref.calc.out | wc -l)"
   memerrcnt="$(grep ERROR ${type}_${num}_ans.mem.out | wc -l)"

  if [[ -n $DEBUG ]]; then
      echo "$solution"
  else
    if (( calcoutdiffcnt == 0  &&  memerrcnt == 0 )); then
       echo "PASS"
       [[ -a ${type}_${num}_ans.calc.out ]] && rm ${type}_${num}_ans.calc.out 
       [[ -a ${type}_${num}_ans.mem.out ]] && rm ${type}_${num}_ans.mem.out 
       [[ -a ${type}_${num}_ref.calc.out ]] && rm ${type}_${num}_ref.calc.out 
       [[ -a ${type}_${num}_ref.test.out ]] && rm ${type}_${num}_ref.test.out 
       [[ -a ${type}_${num}_ans.test.out ]] && rm ${type}_${num}_ans.test.out
       return 0
    fi
    #echo "FAIL: your program produced: '$answer' output should have been: '$solution'"
    echo "FAIL"
    [[ -n $VERBOSE ]] && [[ -a ${type}_${num}_ans.calc.out ]] && echo "Your program produced:"
    [[ -n $VERBOSE ]] && [[ -a ${type}_${num}_ans.calc.out ]] && echo "--------------------------"
    [[ -n $VERBOSE ]] && [[ -a ${type}_${num}_ans.calc.out ]] && cat ${type}_${num}_ans.calc.out
    [[ -n $VERBOSE ]] && [[ -a ${type}_${num}_ans.calc.out ]] && echo "--------------------------"
    [[ -n $VERBOSE ]] && [[ -a ${type}_${num}_ref.calc.out ]] && echo "Output should have been:"
    [[ -n $VERBOSE ]] && [[ -a ${type}_${num}_ref.calc.out ]] && echo "--------------------------"
    [[ -n $VERBOSE ]] && [[ -a ${type}_${num}_ref.calc.out ]] && cat ${type}_${num}_ref.calc.out
    [[ -n $VERBOSE ]] && [[ -a ${type}_${num}_ref.calc.out ]] && echo "--------------------------"
    if [[ -n $VERBOSE &&  -a ${type}_${num}_ans.mem.out && $memerrcnt != 0 ]]; then
	echo "Your program seems the have memory trace errors"
	grep ERROR ${type}_${num}_ans.mem.out
        echo "--------------------------"
    fi
    [[ -n $VERBOSE ]] && echo ""
  fi
  return 1
}

testnum=$1
type=$2

total=0
correct=0

if [[ -z $testnum ]]; then
    for ((i=0; i<${#REQUIRED_INPUT[@]}; i++)); do
	value="${REQUIRED_VALUE[$i]}"
	[[ -z $value ]] && value=1
	if run_test "${REQUIRED_DESC[$i]}" "${REQUIRED_INPUT[$i]}" "$((i+1))" test ; then
	    (( correct += $value ))
	fi
	(( total += value ))
    done
    
    
    for ((i=0; i<${#BONUS_INPUT[@]}; i++)); do
	value="${BONUS_VALUE[$i]}"
	[[ -z $value ]] && value=1
	if run_test "${BONUS_DESC[$i]}" "${BONUS_INPUT[$i]}" "$((i+1))" bonus; then
	    (( correct += $value ))
	fi
    done
else
    VERBOSE=1
    (( i=$testnum - 1 ))
    if [[ -z "$type" ]]; then
	if (( $i < 0 || $i >= ${#REQUIRED_INPUT[@]} )); then
	    echo -n "There are ${#REQUIRED_INPUT[@]} required tests.  Please specify a test number"
	    echo " between 1 and ${#REQUIRED_INPUT[@]} inclusively"
	    exit -1
	fi
	value="${REQUIRED_VALUE[$i]}"
	[[ -z $value ]] && value=1
	if run_test "${REQUIRED_DESC[$i]}" "${REQUIRED_INPUT[$i]}" "$((i+1))" test; then
	    (( correct += $value ))
	fi
	(( total += value ))
    else
	if (( $i < 0 || $i >= ${#BONUS_INPUT[@]} )); then
	    echo -n "There are ${#BONUS_INPUT[@]} bonus tests.  Please specify a test number"
	    echo " between 1 and ${#BONUS_INPUT[@]} inclusively"
	    exit -1
	fi	
	value="${BONUS_VALUE[$i]}"
	[[ -z $value ]] && value=1
	if run_test "${BONUS_DESC[$i]}" "${BONUS_INPUT[$i]}" "$((i+1))" bonus; then
         (( correct += $value ))
	fi
    fi
fi
echo "score: ${correct}/${total}"
