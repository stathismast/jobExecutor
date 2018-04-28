logs="../log/*"
# Keep only the timestamps of each search command
timestamps="$(cat ${logs} | grep search | awk -F : '{ print $1 }' | sort | uniq)"

# echo "${timestamps}"
# echo

# For each different search command keep the keyword and the files of that
# keyword only if it is included in atleast one file
for i in ${timestamps}; do
    results[i]="$(grep ${i} ${logs} |
                  awk -F: -vOFS=: '{NF > 4; $1=$2=$3=""; print substr($0,4)}' |
                  sed 's/:/|||/2g' |
                  awk -F: 'NF>1{a[$1] = a[$1]":"$2};END{for(i in a)print i""a[i]}' |
                  sed 's/|||/:/1g')"
    # echo "${results[i]}"
    # echo ------------------------------------------
done

# Merge all the search command results
allSearchResults=$(echo "${results[*]}")
# echo "${allSearchResults}"

# Count how many different files each keyword is included in
# echo ------------------------------------------
counts="$(echo "${allSearchResults}" | sed 's/[^:]//g' | awk '{ print length }')"
# echo "${counts}"

# Keep the keyword with the highest number
# echo ------------------------------------------
sorted=$(paste -d: <(echo "${counts}") <(echo "${allSearchResults}") | cut -f1-2 -d: | sort -r | head -n1)
count=$(echo "${sorted}" | awk -F: '{ print $1 }')
keyword=$(echo "${sorted}" | awk -F: '{ print $2 }')
echo ${keyword} [totalNumFilesFound: ${count}]
