import requests
import sys
import getopt
from typing import Tuple
import math

import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation
from metadata import density_metadata, entropy_metadata, PlotMetadata


HOST = 'http://localhost'
PORT = 8000
TYPE = ''


def init():
    global PORT
    global HOST
    global TYPE
    argument_list = sys.argv[1:]
    options = 'pht:'
    long_options = ["port=", 'host=', 'type=']
    try:
        arguments, _values = getopt.getopt(
            argument_list, options, long_options)
        for current_argument, current_value in arguments:
            if current_argument in ('-p', '--port'):
                PORT = int(current_value)
            elif current_argument in ('-h', '--host'):
                HOST = current_value
            elif current_argument in ('-t', '--type'):
                TYPE = current_value
    except getopt.error as err:
        print(str(err))
    if len(TYPE) == 0:
        print('missing required argument: type')
        sys.exit()


def config(metadata: PlotMetadata):
    fig, axes = plt.subplots()

    # set the basic properties
    axes.set_xlabel(metadata.xlabel)
    axes.set_ylabel(metadata.ylabel)
    axes.set_title(metadata.title)

    # set the grid on
    axes.grid('on')

    # change the color of the top and right spines to opaque gray
    axes.spines['right'].set_color((.8, .8, .8))
    axes.spines['top'].set_color((.8, .8, .8))

    # tweak the axis labels
    xlab = axes.xaxis.get_label()
    ylab = axes.yaxis.get_label()
    xlab.set_style('italic')
    xlab.set_size(10)
    ylab.set_style('italic')
    ylab.set_size(10)

    # tweak the title
    ttl = axes.title
    ttl.set_weight('bold')

    axes.set_autoscaley_on(True)
    return fig, axes


def fetch(plot_type: str) -> Tuple[int, int]:
    res = requests.get(f'{HOST}:{PORT}/{plot_type}', timeout=10)
    data = res.json()
    print(data)
    if not data['result']:
        return -1, -1
    return data['iteration'], data['value']


def get_status():
    res = requests.get(f'{HOST}:{PORT}/status', timeout=10)
    data = res.json()
    print(data)
    if data['stop']:
        print('Stopped')
        # exit()


def animate(axes, lines, plot_type, metadata):
    iteration, value = fetch(plot_type)
    if iteration == -1 or value == -1:
        return lines
    for i in range(len(lines)):
        lines[i].set_xdata(np.append(lines[i].get_xdata(), iteration))
        lines[i].set_ydata(np.append(lines[i].get_ydata(), metadata.transform_funcs[i](value)))
    axes.relim()
    axes.autoscale_view(True, True, True)
    return lines


def init_animation(lines):
    return lines


def main(plot_type: str):
    metadata = density_metadata if plot_type == 'density' else entropy_metadata
    fig, axes = config(metadata)

    lines = []
    for i in range(metadata.num_lines):
        line, = axes.plot([], [], metadata.formats[i], label=metadata.labels[i])
        lines.append(line)

    axes.legend()

    _ani = animation.FuncAnimation(fig, lambda _i: animate(axes, lines, plot_type, metadata),
                                   init_func=lambda: init_animation(lines), interval=10, blit=False)
    plt.show()


if __name__ == '__main__':
    init()
    main(TYPE)
