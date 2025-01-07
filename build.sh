clear
echo Compiling project....
cd build
cmake -G "Unix Makefiles" . --config Release --parallel
make && 
(
    cd ../
    ./Clue
) ||
(
    echo Ooops, something failed. See above.
)