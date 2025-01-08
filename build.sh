clear
echo Compiling project....
cd build
cmake -G "Unix Makefiles" .
make && 
(
    cd ../
    ./Clue
) ||
(
    echo Ooops, something failed. See above.
)