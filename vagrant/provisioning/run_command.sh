#!/bin/sh
BASEDIR=$(pwd)
echo "$BASEDIR"
docker run -d --name artifact-eval --volume $BASEDIR/results/:/results taptipalit/temporal-specialization-artifacts:1.0
docker exec -it artifact-eval ./run.sh $1
docker rm -f artifact-eval
