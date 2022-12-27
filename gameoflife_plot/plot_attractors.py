import requests
import sys
import getopt
from typing import List
import time
import networkx as nx

import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation
from metadata import density_metadata, entropy_metadata, PlotMetadata


HOST = 'http://localhost'
PORT = 8000
TYPE = ''
REFRESH_INTERVAL = 60


def init():
    global PORT
    global HOST
    global TYPE
    argument_list = sys.argv[1:]
    options = 'pht:'
    long_options = ["port=", 'host=']
    try:
        arguments, _values = getopt.getopt(
            argument_list, options, long_options)
        for current_argument, current_value in arguments:
            if current_argument in ('-p', '--port'):
                PORT = int(current_value)
            elif current_argument in ('-h', '--host'):
                HOST = current_value
    except getopt.error as err:
        print(str(err))


def config():
    fig, axes = plt.subplots()
    graph = nx.DiGraph()
    axes.set_autoscaley_on(True)
    graph.add_nodes_from(range(2 ** 2))
    pos = nx.spring_layout(graph)
    return fig, axes, graph, pos


def fetch() -> List[List[int]]:
    res = requests.get(f'{HOST}:{PORT}/attractors', timeout=10)
    data = res.json()
    # print(data)
    if not data['result']:
        return []
    return data['edges']


def get_status():
    res = requests.get(f'{HOST}:{PORT}/status', timeout=10)
    data = res.json()
    print(data)
    if data['stop']:
        print('Stopped')
        # exit()


def animate():
    global fig
    global graph
    global pos
    axes.clear()
    # fixed_nodes = graph.nodes
    # print('before:', len(fixed_nodes), len(pos))
    edges = fetch()
    edges_list = [(edge[0], edge[1]) for edge in edges]
    graph.add_edges_from(edges_list)
    should_redraw = (int(time.time()) % REFRESH_INTERVAL) == 0
    if len(pos) != len(graph.nodes) or should_redraw:
        pos = nx.spring_layout(graph)
    # print('after:', len(fixed_nodes), len(pos))
    # pos = nx.spring_layout(graph)
    nx.draw(graph, ax=axes, pos=pos, with_labels=True)
    # nx.draw(graph, ax=axes)


def main(plot_type: str):
    global fig
    _ani = animation.FuncAnimation(
        fig, lambda _i: animate(), interval=2000, blit=False)
    plt.show()


if __name__ == '__main__':
    init()
    fig, axes, graph, pos = config()
    main(TYPE)
