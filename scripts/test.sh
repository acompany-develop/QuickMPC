fail = 0
for i in {1..50}
do
  $(make test t=./computation_container p=medium m=run)
done