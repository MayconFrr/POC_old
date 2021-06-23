#!/bin/bash
for file in data_raw/*/*; do
  mkdir -pv "${file//data_raw/data_processed}"
done

for old in data_raw/*/*/SM.dat; do
  new="${old//data_raw/data_processed}" \
    && sed '1d' "$old" | cut -d ' ' -f 1 --complement > "./temp.dat" \
    && awk -F ' ' 'END {printf "%s %s\n", NR, NF}' "./temp.dat" > "$new" \
    && cat "./temp.dat" >> "$new" \
    && echo "saved matrix from $old to $new"
done

for old in data_raw/*/*/WM.dat; do
  new="${old//data_raw/data_processed}" \
    && sed '1d' "$old" | cut -d ' ' -f 1 --complement > "./temp.dat" \
    && awk -F ' ' 'END {printf "%s %s\n", NR, NF}' "./temp.dat" > "$new" \
    && cat "./temp.dat" >> "$new" \
    && echo "saved matrix from $old to $new"
done

rm ./temp.dat
