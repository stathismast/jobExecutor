logs="../log/*"

echo Long:
# Keep only the timestamps of each search command
timestamps="$(cat ${logs} | grep search | awk -F : '{ print $1 }' | sort | uniq)"

# echo "${timestamps}"
# echo

# For each different search command keep the keyword and the files of that
# keyword only if it is included in atleast one file
for i in ${timestamps}; do
    results[i]="$(grep ${i} ${logs} |
                  awk -F: -vOFS=: '{NF > 4; $1=$2=$3=""; print substr($0,4)}' |
                  sort | uniq |
                  sed 's/:/|||/2g' |
                  awk -F: 'NF>1{a[$1] = a[$1]":"$2};END{for(i in a)print i""a[i]}' |
                  sed 's/|||/:/1g')"
    # echo "${results[i]}"
    # echo ------------------------------------------
done

# Merge all the search command results
allSearchResults=$(printf "\n%s" "${results[@]}")
allSearchResults=${allSearchResults:1}
# allSearchResults=$(echo "${results[@]}" | sort | uniq)
# echo "${allSearchResults}"
# echo ------------------------------------------
uniqueResults=$(echo "${allSearchResults}" | sed '/^\s*$/d' | sort | uniq)
# echo "${uniqueResults}"

# Count how many different files each keyword is included in
# echo ------------------------------------------
counts="$(echo "${uniqueResults}" | sed 's/[^:]//g' | awk '{ print length }')"
# echo "${counts}"

# Keep the keyword with the highest number
mostFreqFound=$(paste -d: <(echo "${counts}") <(echo "${uniqueResults}") | cut -f1-2 -d: | sort -k1 -n -r | head -n1)
# echo ------------------------------------------
count=$(echo "${mostFreqFound}" | awk -F: '{ print $1 }')
keyword=$(echo "${mostFreqFound}" | awk -F: '{ print $2 }')
echo ${keyword} [totalNumFilesFound: ${count}]



###############################################################################

echo Short:
uniqueSearches=$(grep ':search:' ../log/* |
				cut -d: -f4- |
				grep : | sort | uniq |
				sed 's/:/|||/2g' |
				awk -F: 'NF>1{a[$1] = a[$1]":"$2};END{for(i in a)print i""a[i]}' |
				sed 's/|||/:/1g')
# echo "${uniqueSearches}"
# echo ---------
counts=$(echo "${uniqueSearches}" | sed 's/[^:]//g' | awk '{ print length }')
# echo ---------
# echo "${counts}"

# Keep the keyword with the highest number
mostFreqFound=$(paste -d: <(echo "${counts}") <(echo "${uniqueSearches}") |
				cut -f1-2 -d: | sort -k1 -n -r | head -n1)
# echo ------------------------------------------
count=$(echo "${mostFreqFound}" | awk -F: '{ print $1 }')
keyword=$(echo "${mostFreqFound}" | awk -F: '{ print $2 }')
echo ${keyword} [totalNumFilesFound: ${count}]
