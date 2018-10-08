sed '1d' $1
sed -i --  "s/fs/`tail -n 1 $1 | cut -c7-50`/" $1
