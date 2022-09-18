#!/bin/sh

# Run first node
echo First node starting...
buck run //gameoflifeServer:worker -- -port=8080 &

# Run second node
echo Second node starting...
buck run //gameoflifeServer:worker -- -port=8081 &

# Run third node
echo Third node starting...
buck run //gameoflifeServer:worker -- -port=8082 &

# Run fourth node
echo Fourth node starting...
buck run //gameoflifeServer:worker -- -port=8083 &

# Run orchestrator
echo Orchestrator starting...
buck run //gameoflifeServer:orchestrator 
