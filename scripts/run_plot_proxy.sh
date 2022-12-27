#!/bin/sh

docker run -p 6379:6379 redislabs/rejson:latest &
python3 gameoflife_plot/plot_proxy.py
