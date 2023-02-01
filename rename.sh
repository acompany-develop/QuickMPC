# find . -depth -type d -name '*[A-Z]*' ! -path "./.git/*" ! -path "./.github/*"
find . -depth -type d -name '*[A-Z]*' ! -path "./.git/*" ! -path "./.github/*" | while read file; do
  dir=$(dirname "$file")
  base=$(basename "$file")
  mv "$file" "$dir/$(echo ${base}_ | tr '[:upper:]' '[:lower:]')"
done