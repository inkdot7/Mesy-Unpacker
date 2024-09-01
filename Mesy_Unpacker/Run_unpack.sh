################
# Run_unpack.sh 
# Simple bash script to run the IS690 unpacker
# 1-> Reads one by one the names of the files set in: FileList.tx
# 2-> For each name searches for the file in the zip folder.
# 3-> Un zips the selected file 
# 4-> Uses the Interpreter.py script to create a configuration file from the .mvlclst (mesytech file)
# 5-> Uses the Unpacker and the configuration file to translate .mvlclst to .root (mesytech file)
# 6-> Save .root and clear the redundant files
# 7-> Open root
# DFR
################

#!/bin/bash

# Define the name of the ZIP and root path
#ZIP="./Datos_Mesytec"  # ZIP file path
ZIP="./Zip"   #Original directory
ROOT="./Root" #Target directory

# Define the file 
FileList="FileList.txt"

echo "Hiiii, I am unpacking :)"

# Check if the file exists
if [ ! -e "$FileList" ]; then
    echo "File list not found: $FileList"
    exit 1
fi

# Compile the unpacker
g++ Unpack_mvme.cpp `root-config --cflags --libs` -o Unpack_mvme.out

# Read each line from the file list
while IFS= read -r line; do
    
    # Get name file
    file_name=$(basename "$line")

    # Extract the .txt file from the ZIP archive
    unzip -o -j "$ZIP/$file_name.zip" '*.mvlclst' -d "$ZIP/"

    # Construct input configuration and output file paths
    input_file="$ZIP/$file_name.mvlclst"
    confi_file="$ZIP/$file_name.txt"
    output_file="$ROOT/$file_name.root"

    # Execute Interpreter.py with the input and output file paths from the current line
    python3 Interpreter.py "$input_file" "$confi_file" "$output_file"
    
    # Add the output file path to the array
    output_files+=("$file_name.root")

    # Remove the .mvlclst file
    rm "$input_file"
    rm "$confi_file"
    
done < "$FileList"

root_command=""
for file in "${output_files[@]}"; do
    root_command+=" $ROOT/$file"
done

# Execute the root command
root -l$root_command



