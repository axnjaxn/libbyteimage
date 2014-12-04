function help {
    echo "Usage: byteimage-config [options]"
    echo "Options:"
    echo "  --cflags Compilation flags (such as includes)"
    echo "  --libs   Library flags (including dependencies)"
    echo "  --inc    Show the list of *.h files that can be included" 
}

if [ "$#" -eq "0" ]; then
    help
    exit 0
fi

for i in $*
do
    if [ $i = "--cflags" ]; then
	echo $CFLAGS
    elif [ $i = "--libs" ]; then
	echo $LIBS
    elif [ $i = "--inc" ]; then
	echo $INC
    else
	echo "Invalid parameters."
	help
	exit 1
    fi
done
exit 0
