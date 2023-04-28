#!/bin/bash

# This script echoes the latest tag name,
# tars and gzips everything in a specified folder,
# includes a specified file, appends a tag name to the output file name,
# commits the changes, pushes the changes to the remote repository,
# and creates a new tag with a user-specified name

# Echo the latest tag name
LATEST_TAG=$(git describe --abbrev=0 --tags)
echo "Latest tag name is: $LATEST_TAG"

# Set the folder to be archived
FOLDER=homeConfig


if [ ! -z $1 ]; then
    TAG_NAME=$1
else
    # Prompt the user to enter a tag name
    echo "Enter a tag name:"
    read TAG_NAME
fi

# use -f to force past this
if [ -z $2 ] || [ ! $2 == "-f" ]; then
    # Check if the new tag name is greater than the latest tag name
    if [[ $(git tag -l "$TAG_NAME") ]]; then
        echo "Error: Tag '$TAG_NAME' already exists."
        exit 1
    elif [[ $(printf "%s\n%s" "$LATEST_TAG" "$TAG_NAME" | sort -V | head -n1) != "$LATEST_TAG" ]]; then
        echo "Error: New tag version must be greater than the latest tag version ('$LATEST_TAG')."
        exit 1
    fi
fi

# Set the name of the output file
OUTPUT_FILE=homeThing_${TAG_NAME}.tar.gz

# Include the example.yaml file
INCLUDE_FILE=example-homeThing.yaml

rm homeThing_*.tar.gz

# Tar and gzip the folder, including the example.yaml file
tar -czvf $OUTPUT_FILE --exclude=secrets.yaml $FOLDER fonts --add=$INCLUDE_FILE

# Commit the changes
git add .
git commit -m "New Release $TAG_NAME"
git push

# # Create a new tag with the entered tag name
git tag $TAG_NAME
git push origin $TAG_NAME