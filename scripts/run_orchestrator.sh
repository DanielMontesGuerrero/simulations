#!/bin/sh

# Runs orchestrator and four worker nodes locally
# Flags:
# 	-r: number of rows
# 	-c: number of cols
# 	-d: run on debug mode
# 	-m: module indexes when updating

ROWS=0
COLS=0
MODULE=0
DEBUG=0

while getopts c:r:m:d: flag; do
	case "${flag}" in
	c) COLS=${OPTARG} ;;
	r) ROWS=${OPTARG} ;;
	m) MODULE=${OPTARG} ;;
	d) DEBUG=${OPTARG} ;;
	esac
done

if [ "$COLS" = "0" ]; then
	echo "Expected -c"
	exit 1
fi

if [ "$ROWS" = "0" ]; then
	echo "Expected -r"
	exit 1
fi

ROWS_1=$(expr $ROWS / 2)
MOD_ROWS=$(expr $ROWS % 2)

if [ "$MOD_ROWS" -gt "0" ]; then
	ROWS_2=$(expr $ROWS_1 + 1)
else
	ROWS_2=$ROWS_1
fi

COLS_1=$(expr $COLS / 2)
MOD_COLS=$(expr $COLS % 2)

if [ "$MOD_COLS" -gt "0" ]; then
	COLS_2=$(expr $COLS_1 + 1)
else
	COLS_2=$COLS_1
fi

# Run first node
echo First node starting...
buck run //gameoflifeServer:worker -- -port=8080 -rows=$ROWS_1 -cols=$COLS_1 -debug=$DEBUG &

# Run second node
echo Second node starting...
buck run //gameoflifeServer:worker -- -port=8081 -rows=$ROWS_1 -cols=$COLS_2 -debug=$DEBUG &

# Run third node
echo Third node starting...
buck run //gameoflifeServer:worker -- -port=8082 -rows=$ROWS_2 -cols=$COLS_1 -debug=$DEBUG &

# Run fourth node
echo Fourth node starting...
buck run //gameoflifeServer:worker -- -port=8083 -rows=$ROWS_2 -cols=$COLS_2 -debug=$DEBUG &

# Run orchestrator
echo Orchestrator starting...
buck run //gameoflifeServer:orchestrator -- -rows=$ROWS -cols=$COLS -shouldModule=$MODULE -debug=$DEBUG
