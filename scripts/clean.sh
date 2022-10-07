DIR="build/"
if [ -d "$DIR" ]; then
  rm -r ./build/
  echo "Build directory has been deleted!"
else 
    echo "Couldn't find build directory"
fi

