sed -i.orig '1d' $1
sed -i --  "s/\bfs\b/`tail -n 1 $1 | cut -c7-50`/" $1
