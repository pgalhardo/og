#!/bin/sh

src_dir=og
testfolder=auto-tests
score=0

# simulate delivery state
#make clean -C $src_dir 2>&1 > /dev/null

echo "#################### PROJECT STRUCTURE ####################"
echo
echo
echo "====================     N O D E S     ===================="
echo

if [ -d $src_dir/ast ]
then
	echo "-- Directory 'ast' found."
	echo
	nodes=$(ls -l $src_dir/ast | grep .h | wc -l)
	echo "Number of nodes: $nodes"
	echo
	
else
	echo "-- Directory 'ast' not found."
	exit 1
fi

echo "====================     SEMANTICS     ===================="
echo

if [ -d $src_dir/targets ]
then
	echo "-- Directory 'targets' found."
	echo
else
	echo "-- Directory 'targets' not found."
	exit 1
fi

echo "====================     SCANNER       ===================="
echo

if [ -f $src_dir/og_scanner.l ]
then
	echo "-- Scanner specification found."
	echo
else
	echo "-- Scanner specification not found."
	exit 1
fi

echo "====================      PARSER       ===================="
echo

if [ -f $src_dir/og_parser.y ]
then
	echo "-- Parser specification found."
	yacc --xml $src_dir/og_parser.y > yacc.out &> /dev/null
	states=$(cat y.xml | grep "<state " | wc -l )
	rules=$(cat y.xml | grep "<rule " | wc -l )
	terminals=$(cat y.xml | grep "<terminal " | wc -l )
	nonterminals=$(cat y.xml | grep "<nonterminal " | wc -l )
	
	echo "$terminals terminals, $nonterminals nonterminals"
	echo "$rules grammar rules, $states states"
	
	if [[ $(cat yacc.out) ]]; then
    	score=1
		echo "!! GRAMMAR HAS CONFLICTS"
	else
		echo "-- grammar has no conflics"
	fi
	
	rm yacc.out y.xml y.tab.c	
	echo
else
	echo "-- Parser specification not found."
	exit 1
fi

echo "====================     COMPILING     ===================="
echo

if ! [ -r $src_dir/Makefile ]
then
  echo "$0: that dir has no Makefile!"
  exit 1
fi

# compiling: if you want to show the compilation shenanigans,
# remove everything from 2>&1 onwards onwards
make -C $src_dir  2>&1 > /dev/null
if [ "$?" -ne "0" ]
then
  echo
  printf '%b' "$0: ${red}build failed${reset}\n"
  printf '%b' "$0: ${red}aborting${reset}\n"
  exit 1
fi

if [ -f $src_dir/og ]
then
	echo "-- successfully generated 'og'."
	echo
else
	echo "!! Makefile could not produce 'og'."
	exit 1
fi

echo "##################      TEST RESULTS      ###################"

red="\033[31;1m"
green="\033[32;1m"
reset="\033[0m"

printf '%b' "     ${green}OK${reset} - compiler runs; ${red}KO${reset} - compiler (partially) fails  \n"
echo "   1 - compiler produces ASM file from og program             " 
echo "   2 - yasm is able to produce object file (.o) from ASM file "
echo "   3 - linker is able to produce executable program           "
echo "   4 - program runs and returns 0 on sucessful exit           "
echo "   5 - program output matches expected output                 "
echo "       (tab, space, and newline chars are ignored)            "
echo 

if ! [ -d $testfolder/results ]
then
  mkdir $testfolder/results
fi

total=0
succ=0

input=$1
raw=$(basename $input | cut -f 1 -d'.')
expected=$testfolder/expected/$raw.out
out=$testfolder/results/$raw.outhyp
if ! [ -r $input ]
then
  printf '%b' "$0: cannot read file "$input"${red}aborting$reset\n"
  exit 1
fi
if ! [ -r $expected ]
then
  printf '%b' "$0: cannot read file $expected${red}aborting$reset\n"
  exit 1
fi

lenght=$( echo -n $raw | wc -c )
dots=$(( 30-$lenght ))
printf "$raw"
printf ".................."
total=$(echo "$total + 1" | bc)

if [ -z $2 ]; then 
  $src_dir/og $input 2>&1
else
  valgrind $src_dir/og $input 2>&1
fi
yasm -felf32 $testfolder/$raw.asm -o $testfolder/$raw.o 2>&1
# ld -m elf_i386 -o $testfolder/$raw $testfolder/$raw.o -lrts 2>&1
ld -m elf_i386 -o $testfolder/$raw $testfolder/$raw.o -L$HOME/ist/com/root/usr/lib -lrts 2>&1
$testfolder/$raw &> $out

retVal=$?
if [ $retVal -eq 139 ]; then
  printf '%b' "${red}KO${reset}\n"
else
  tr -d '\t \n' < $out > temp.outhyp && mv temp.outhyp $out
  tr -d '\t \n' < $testfolder/expected/$raw.out > temp.outhyp && mv temp.outhyp $testfolder/expected/$raw.out

  if [[ $(diff -b $expected $out) ]]; then
    printf '%b' "${red}KO${reset}\n"
  else
    printf '%b' "${green}OK${reset}\n"
    succ=$(echo "$succ + 1" | bc)
  fi
fi

rm $testfolder/*.asm
rm $testfolder/*.o
rm $testfolder/$raw

echo
echo "#####################     SUMMARY     #####################"
echo

if [ $succ -eq 1 ]
then
	printf '%b' "Number of at least \"OK\" tests:${green} 1/1 (100%)\n"
else
	printf '%b' "Number of at least \"OK\" tests:${red} 0/1 (0%)\n"
fi
printf '%b' "${reset}"
