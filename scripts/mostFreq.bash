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
