#!/bin/bash

# Prompt input
read -p "Enter 6-digit North America phone number prefix: " prefix

# Check if the input is 6 digits
if [[ ! "$prefix" =~ ^[0-9]{6}$ ]]
then
    echo "Not a valid 6-digit North America phone number prefix."
    exit 1
fi

# Search for the 6 digits and format the result
result=$(grep "$prefix" nanpa | sed -e 's/[0-9]\{6\}//' -e 's/ *$//')

# Check if there was a successful match or not
if [ -z "$result" ]
then
    echo "That 6-digit North America phone number prefix could not be found."
    exit 1
fi

# Return the match
echo "$result"