grep ':search:' ../log/* |
cut -d: -f4-5            |
grep :                   |
cut -d: -f1              |
sort                     |
uniq                     |
wc -l
