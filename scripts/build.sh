DIR="build/"
if [ -d "$DIR" ]; then
  # Take action if $DIR exists. #
  echo "Compiling..."
else 
    mkdir build
    echo "Creating build directory...."
    echo "Compiling..."
fi
gcc main.c -o build/main -lncurses
echo "Project has been successfuly built in  ${DIR} directory"