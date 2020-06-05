REPO="https://github.com/PiotrJTomaszewski/PRozp_projekt"
BASEDIR="/home/piotr/Documents/studia/rok_III/sem_6/PRozp/"
REPODIR="$BASEDIR/PRozp_projekt"
SRCDIR="$REPODIR/code_cpp/src"
if [ ! -d "$BASEDIR" ]
then
	echo "Creating directory"
	mkdir -p "$BASEDIR"
fi
if [ ! -d "$REPODIR" ]
then
	echo "Downloading repo"
	cd "$BASEDIR" && git clone "$REPO" 
	cd "$REPODIR" && git checkout dev && git pull
	cd "$SRCDIR" && chmod u+x compile.sh
else
	cd "$SRCDIR" && git pull && (make clean || true)
fi
./compile.sh