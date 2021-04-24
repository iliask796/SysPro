#!/bin/bash
if [ "$#" -ne 3 ]
	then
		echo "Incorrect number of arguements."
		exit 1
elif [ ! -f "$1" ]
	then
		echo "Input does not contain the required file."
		exit 2
elif [ -d "$2" ]
	then
		echo "Given directory already exists."
		exit 3
elif ! [[ "$3" =~ ^[0-9]+$ ]]
	then
		echo "Input does not contain integer number of files to be created."
		exit 4
fi
mkdir "$2"
echo "Created Parent Directory."
INPUT="$1"
countries=()
while read -r line
	do
	country=$(echo $line | cut -d' ' -f4)
	if [[ ! " ${countries[@]} " =~ " ${country} " ]] 
		then
			countries+=( "$country" )
	fi
done < "$INPUT"
a=0
counters=()
cd "$2"
while [ "$a" -lt "${#countries[@]}" ]
	do
	b=1
	mkdir "${countries[$a]}"
	cd "${countries[$a]}"
	while [ "$b" -le "$3" ]
		do
		FILENAME="${countries[$a]}-${b}.txt"
		echo -n "" > "$FILENAME"
		let "b = b + 1"
	done
	cd ..
	counters+=( 1 )
	let "a = a + 1"
done
echo "Created Sub-Directories."
cd ..
while read -r line
	do
	country=$(echo $line | cut -d' ' -f4)
	i=0
	while [ "$i" -lt "${#counters[@]}" ]
	do
		if [[ "${countries[$i]}" = "${country}" ]]
			then
       				pos="${i}"

  		fi
		let "i = i + 1"
	done
	cd "$2"/"$country"
	echo $line >> "$country-${counters[$pos]}.txt"
	if [ ${counters[$pos]} -lt 3 ]
		then
			((counters[$pos]++))
	else
		counters[$pos]=1
	fi
	cd ../..
done < "$INPUT"
echo "Successfully Parsed Data to Sub-Directories."
exit 0
