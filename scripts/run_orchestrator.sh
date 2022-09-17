#!/bin/sh

# Run first node
echo First node starting...
buck run //gameoflifeServer:worker -- -port=8080 >/dev/null &

# Run second node
echo Second node starting...
buck run //gameoflifeServer:worker -- -port=8081 >/dev/null &

# Run third node
echo Third node starting...
buck run //gameoflifeServer:worker -- -port=8082 >/dev/null &

# Run fourth node
echo Fourth node starting...
buck run //gameoflifeServer:worker -- -port=8083 >/dev/null &

# Run orchestrator
echo Orchestrator starting...
buck run //gameoflifeServer:orchestrator >/dev/null &
