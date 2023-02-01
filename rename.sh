find . -depth -type d -name '*[A-Z]*' ! -path "./.git/*" ! -path "./.github/*" | while read dir; do
  base=$(basename "$dir")
  new_dir=$(echo "$base" | sed -r 's/([A-Z][a-z0-9]+)/_\1/g' | tr '[:upper:]' '[:lower:]')
#   echo mv "$dir" "$(dirname "$dir")/${new_dir}"
  mv "$dir" "$(dirname "$dir")/${new_dir}"
done