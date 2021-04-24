#!/bin/bash
a=0
FILENAME="inputFile"
if [ ! -f "$1" ] || [ ! -f "$2" ]
	then
		echo "Input does not contain the two required files."
		exit 1
elif ! [[ "$3" =~ ^[0-9]+$ ]]
	then
		echo "Input does not contain integer number of lines."
		exit 2
elif ! [[ "$4" =~ ^[0-9]+$ ]]
	then
		echo "Input does not set duplicates correctly."
		exit 3
fi
if [ "$4" -eq 0 ] && [ ! "$3" -le 10000 ] 
		then
			echo "Cannot generate unique IDs using 4 digits for more than 10000 citizens."
			exit 4
		fi
readarray -t viruses < "$1"
readarray -t countries < "$2"
virusNum="${#viruses[@]}"
countryNum="${#countries[@]}"
if [ -f "$FILENAME" ]
then
        echo "File: $FILENAME already exists."
        sleep 0.75
        echo "Type YES to override current file or NO to cancel."
        read b
        if [ "$b" == "YES" ]
        then
                echo "Attempting to override."
                > "$FILENAME"
        elif [ "$b" == "NO" ]
        then
                echo "Exit from user."
                exit 5
        else
                echo "Wrong input. Rerun to retry."
                exit 6
        fi
fi
while [ "$a" -lt "$3" ]
	do
	if [ "$4" -eq 0 ]
	then
		if [ "$a" -eq 0 ]
		then
			ids=($(shuf -i 0-9999 -n "$3"))	
		fi
		id="${ids[$a]}"
	else
		flag=$(( RANDOM % 10 ))
		if [ "$a" -ne 1 ]
		then
			if [ "$flag" -le 8 ] || [ "$a" -eq 0 ]
			then
				id=$(( ( RANDOM % 9999 ) + 1 ))
			fi
		fi
	fi
	length1=$(( ( RANDOM % 10 ) + 2 ))
	length2=$(( ( RANDOM % 10 ) + 2 ))
	fullname="$( tr -dc A-Z </dev/urandom | head -c 1 )$( tr -dc a-z </dev/urandom | head -c "$length1" ) $( tr -dc A-Z </dev/urandom | head -c 1 )$( tr -dc a-z </dev/urandom | head -c "$length2" )"
	country=$(( RANDOM % $countryNum ))
	age=$(( ( RANDOM % 120 ) + 1 ))
	virus=$(( RANDOM % $virusNum ))
	vaccinated=$(( RANDOM % 2 ))
	if [ "$vaccinated" -eq 0 ]
	then
		echo "$id" "$fullname" "${countries[$country]}" "$age" "${viruses[$virus]}" "NO" >> inputFile
	else
		day=$(( ( RANDOM % 30 ) + 1 ))
		month=$(( ( RANDOM % 12 ) + 1 ))
		year=$(( 2020 - ( RANDOM % $age ) ))
		echo "$id" "$fullname" "${countries[$country]}" "$age" "${viruses[$virus]}" "YES" "$day"-"$month"-"$year" >> inputFile
	fi
	let "a = a + 1"
	done
exit 0
