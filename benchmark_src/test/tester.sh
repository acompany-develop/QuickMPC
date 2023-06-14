#!/bin/bash
status=0

# benchmarkを実行
docker-compose up benchmark-libc
status=$?
# NOTE: `docker-compose up`はCMDやENTRYPOINTで異常終了してもexitステータスが`0`になってしまうので別途exitステータスを集積する
run_status=$(docker-compose ps -aq | tr -d '[ ]' | xargs docker inspect -f '{{ .State.ExitCode }}' | grep -v 0 | wc -l | tr -d '[ ]')
status=$(($status + $run_status))

if [ $status -ne 0 ]; then
  echo "docker-compose up failed with exit code $status"
  exit $status
fi

exit 0
