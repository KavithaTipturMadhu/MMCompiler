sed -i.orig '1d' $1
darwin=false;
case "$(uname)" in
    Darwin*) darwin=true ;;
esac
if $darwin; then
    sed -i -- "s/[[:<:]]fs[[:>:]]/`tail -n 1 $1 | cut -c7-50`/" $1
else
    sed -i -- "s/\bfs\b/`tail -n 1 $1 | cut -c7-50`/" $1
fi
