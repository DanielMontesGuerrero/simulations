#!/bin/sh

fuser -k 8000/tcp
docker stop $(docker ps -a -q)
