clear
echo Compiling project....
cd build
cmake -G "Unix Makefiles" .
make && 
(
    cd ../
    ./the_butler_didnt_do_it
) ||
(
    echo Ooops, something failed. See above.
)