find . -depth -type d -name '_*[a-z]*' ! -path "./.git/*" ! -path "./.github/*" | while read dir; do
  base=$(basename "$dir")
  # new_dir=$(echo "$base" | sed -r 's/([A-Z][a-z0-9]+)/_\1/g' | tr '[:upper:]' '[:lower:]')
  # echo mv "$dir" "$(dirname "$dir")/${base#*_}"
  mv "$dir" "$(dirname "$dir")/${base#*_}"
done