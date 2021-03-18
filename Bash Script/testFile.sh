# !/ bin / bash
a=0
FILENAME="inputFile.txt"
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
                exit 4
        else
                echo "Wrong input. Rerun to retry."
                exit 5
        fi
fi
while [ "$a" -lt "$3" ]
	do
	country=$(( RANDOM % "$countryNum" ))
	age=$(( ( RANDOM % 120 ) + 1 ))
	virus=$(( RANDOM % "$virusNum" ))
	vaccinated=$(( RANDOM % 2 ))
	if [ "$vaccinated" == 0 ]
	then
		echo "${countries[$country]}" "$age" "${viruses[$virus]}" "NO" >> inputFile.txt
	else
		day=$(( ( RANDOM % 30 ) + 1 ))
		month=$(( ( RANDOM % 12 ) + 1 ))
		year=$(( 2020 - ( RANDOM % "$age" ) ))
		echo "${countries[$country]}" "$age" "${viruses[$virus]}" "YES" "$day"-"$month"-"$year" >> inputFile.txt
	fi
	let "a = a + 1"
	done
exit 0
