logs="../log/*"

echo Long:
# Keep only the timestamps of each search command
timestamps="$(cat ${logs} | grep search | awk -F : '{ print $1 }' | sort | uniq)"

# echo "${timestamps}"
# echo

# For each different search command keep the keyword and the files of that
# keyword only if it is included in atleast one file
for i in ${timestamps}; do
    results[i]="$(grep ${i} ${logs} | cut -d: -f4-5 | grep : | cut -d: -f1)"
    # echo "${results[i]}"
done

# echo -----
allResults=$(printf "\n%s" "${results[@]}")
allResults=${allResults:1}
# allResults=$(echo "${results[@]}" | sort | uniq)
# echo "${allResults}"
# echo ------------------------------------------
uniqueResults=$(echo "${allResults}" | sed '/^\s*$/d' | sort | uniq)
echo "${uniqueResults}" | wc -l


echo Short:
grep ':search:' ../log/* |
cut -d: -f4-5            |
grep :                   |
cut -d: -f1              |
sort                     |
uniq                     |
wc -l
