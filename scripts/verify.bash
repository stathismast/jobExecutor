grep search:$1: ../log/* | cut -d: -f3-30 | sort | uniq
