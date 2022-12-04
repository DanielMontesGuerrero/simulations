import os
import random
import redis
import sys
import getopt

from flask import Flask, request
from redis.commands.json.path import Path


PORT = 8000
app = Flask(__name__)
redis_client = redis.Redis('localhost')


def init():
    global PORT
    argument_list = sys.argv[1:]
    options = "p:"
    long_options = ["port="]
    try:
        arguments, _values = getopt.getopt(
            argument_list, options, long_options)
        for current_argument, current_value in arguments:
            if current_argument in ("-p", "--port"):
                PORT = int(current_value)
    except getopt.error as err:
        print(str(err))


def update_data(data_type: str, iteration: int, value: float):
    data = redis_client.json().get(data_type)
    data['data'].append([iteration, value])
    redis_client.json().set(data_type, Path.root_path(), data)
    print(data)


def pop_data(data_type: str):
    data = redis_client.json().get(data_type)
    if len(data['data']) == 0:
        return []
    value = data['data'][0]
    data['data'] = data['data'][1:]
    redis_client.json().set(data_type, Path.root_path(), data)
    print(data)
    return value


def handle_post(data_type: str, args):
    iteration = args.get('iteration', type=int)
    density = args.get(data_type, type=float)
    if iteration is None or density is None:
        return {'result': False}
    update_data(data_type, iteration, density)
    return {'result': True}


def handle_get(data_type: str):
    value = pop_data(data_type)
    if len(value) == 0:
        return {'result': False}
    return {'result': True, 'iteration': value[0], 'value': value[1]}


@app.route('/')
def serve():
    return 'OK'


@app.get('/density')
def get_density():
    return handle_get('density')


@app.post('/density')
def post_density():
    return handle_post('density', request.args)


@app.get('/entropy')
def get_entropy():
    return handle_get('entropy')


@app.post('/entropy')
def post_entropy():
    return handle_post('entropy', request.args)


@app.get('/status')
def status():
    should_stop = random.choice([True, False])
    response = {'stop': should_stop}
    return response


if __name__ == '__main__':
    init()
    while True:
        try:
            redis_client.ping()
            break
        except redis.ConnectionError:
            print('waiting connection to redis')
    port = int(os.environ.get('PORT', PORT))
    initial_data = {'data': []}
    redis_client.json().set('density', Path.root_path(), initial_data)
    redis_client.json().set('entropy', Path.root_path(), initial_data)
    app.run(host='0.0.0.0', port=port)
