import time
import datetime
import sys
import logging
import re
from typing import List, Tuple

from azure.batch import BatchServiceClient
import azure.functions as func
import azure.batch.models as batchmodels
from start_orchestrator.models import (ComputeNodeInfo,
                                       RequestData,
                                       RespponseData,
                                       WorkerInfo,
                                       OrchestratorInfo)
from start_orchestrator import config

NUM_NODES_BREAKPOINT_1 = 1000 * 1000
NUM_NODES_BREAKPOINT_2 = 5000 * 5000


def add_task(batch_service_client: BatchServiceClient, job_id: str, command: str,
             task_type: str) -> str:
    """
    Adds a task for each input file in the collection to the specified job.
    :param batch_service_client: A Batch service client.
    :param str job_id: The ID of the job to which to add the tasks.
    :param list resource_input_files: A collection of input files. One task will be
     created for each input file.
    """
    task_id = f'task-{task_type}-{int(time.time()*1000)}'
    logging.info('Adding task [%s] to job [%s]...', task_id, job_id)
    batch_service_client.task.add(job_id, batchmodels.TaskAddParameter(
        id=task_id,
        command_line=command,
    ))
    return task_id


def wait_for_tasks_to_complete(batch_service_client: BatchServiceClient, job_id: str,
                               timeout: datetime.timedelta):
    """
    Returns when all tasks in the specified job reach the Completed state.
    :param batch_service_client: A Batch service client.
    :param job_id: The id of the job whose tasks should be to monitored.
    :param timeout: The duration to wait for task completion. If all
    tasks in the specified job do not reach Completed state within this time
    period, an exception will be raised.
    """
    timeout_expiration = datetime.datetime.now() + timeout

    logging.info(
        'Monitoring all tasks for \'Completed\' state, timeout in {%s}...', timeout)

    while datetime.datetime.now() < timeout_expiration:
        print('.', end='')
        sys.stdout.flush()
        tasks = batch_service_client.task.list(job_id)

        incomplete_tasks = [task for task in tasks if
                            task.state != batchmodels.TaskState.completed]
        if not incomplete_tasks:
            print()
            return True

        time.sleep(1)

    print()
    message = ('ERROR: Tasks did not reach \'Completed\' state within timeout period of ' +
               str(timeout))
    logging.error(message)
    raise RuntimeError(message)


def get_task_info(batch_service_client: BatchServiceClient, job_id: str,
                  task_id: str) -> batchmodels.CloudTask:
    logging.info(
        'Waiting to get task info. job_id [%s] task_id [%s]', job_id, task_id)
    while True:
        print('.', end='')
        sys.stdout.flush()
        cloud_task = batch_service_client.task.get(
            job_id, task_id, batchmodels.TaskGetOptions(select='nodeInfo'))
        if cloud_task is not None and cloud_task.node_info is not None:
            break
    print()
    return cloud_task

def get_compute_node_info(batch_service_client: BatchServiceClient, pool_id: str,
                          node_id: str) -> batchmodels.ComputeNode:
    logging.info(
        'Waiting to get compute node info. pool_id [%s] node_id [%s]', pool_id, node_id)
    while True:
        print('.', end='')
        sys.stdout.flush()
        compute_node_info = batch_service_client.compute_node.get(
            pool_id,
            node_id,
            batchmodels.ComputeNodeGetOptions(select='endpointConfiguration'),
        )
        if compute_node_info is not None and compute_node_info.endpoint_configuration is not None:
            break
    print()
    return compute_node_info


def parse_request(req: func.HttpRequest) -> RequestData:
    rows = req.params.get('rows')
    cols = req.params.get('cols')
    if not rows:
        try:
            req_body = req.get_json()
        except ValueError:
            pass
        else:
            if not rows:
                rows = req_body.get('rows')
            if not cols:
                cols = req_body.get('cols')
    if not rows or not cols:
        raise ValueError()
    return RequestData(int(rows), int(cols))


def get_number_of_nodes(request_data: RequestData) -> int:
    total_num_cells = request_data.cols * request_data.rows
    if total_num_cells <= NUM_NODES_BREAKPOINT_1:
        return 1
    if total_num_cells <= NUM_NODES_BREAKPOINT_2:
        return 4
    return 9


def get_last_divisors(num: int) -> Tuple[int, int]:
    i = 1
    div1 = 1
    div2 = 2
    while i * i <= num:
        if num % i:
            continue
        div1 = i
        div2 = num // i
        i += 1
    return div1, div2


def get_workers_info(request_data: RequestData, num_nodes: int) -> List[WorkerInfo]:
    num_divs_rows, num_divs_cols = get_last_divisors(num_nodes)
    workers = []
    for i in range(num_divs_rows):
        for j in range(num_divs_cols):
            rows = request_data.rows // num_divs_rows
            cols = request_data.cols // num_divs_cols
            if j == num_divs_cols - 1:
                cols = request_data.cols - cols * j
            if i == num_divs_rows - 1:
                rows = request_data.rows - rows * i
            workers.append(WorkerInfo(rows, cols))
    return workers


def create_worker(batch_client: BatchServiceClient, worker: WorkerInfo):
    command = (f'echo \"-rows={worker.rows} -cols={worker.cols} ' +
               f'-port={worker.port} -protocol={worker.protocol}\"')
    task_id = add_task(batch_client, config.JOB_ID, command, 'worker')
    return get_task_info(batch_client, config.JOB_ID, task_id)

def get_inbound_endpoint(name: str, node_info: batchmodels.ComputeNode) -> Tuple[str,int]:
    for inbound_endpoint in node_info.endpoint_configuration.inbound_endpoints:
        print(inbound_endpoint)
        if re.search(f'{name}.*', inbound_endpoint.name):
            node_ip = inbound_endpoint.public_ip_address
            node_port = inbound_endpoint.frontend_port
            return node_ip, node_port
    logging.error('Did not find inbound endpoint [%s] for compute node', name)
    return '', -1


def create_workers(batch_client: BatchServiceClient,
                   workers_info: List[WorkerInfo],
                   orchestrator_info: OrchestratorInfo):
    for worker in workers_info:
        task_info = create_worker(batch_client, worker)
        node_info = get_compute_node_info(batch_client, config.POOL_ID, task_info.node_info.node_id)
        node_ip, node_port = get_inbound_endpoint(config.INBOUND_ENDPOINT_NAME, node_info)
        orchestrator_info.hosts.append(node_ip)
        orchestrator_info.ports.append(node_port)


def create_orchestrator(batch_client: BatchServiceClient, orchestrator_info: OrchestratorInfo):
    hosts = ','.join(orchestrator_info.hosts)
    ports = ','.join([str(port) for port in orchestrator_info.ports])
    command = (f'echo \"-rows={orchestrator_info.rows} -cols={orchestrator_info.cols} ' +
               f'-port={orchestrator_info.port} -protocol={orchestrator_info.protocol} ' +
               f'-hosts={hosts} -ports={ports}\"')
    task_id = add_task(batch_client, config.JOB_ID, command, 'orchestrator')
    return get_task_info(batch_client, config.JOB_ID, task_id)

def create_response(batch_client: BatchServiceClient, orchestrator_task_info: batchmodels.CloudTask,
                    orchestrator_info: OrchestratorInfo) -> RespponseData:
    orchestrator_ip, orchestrator_port = get_inbound_endpoint(
        config.INBOUND_ENDPOINT_NAME,
        get_compute_node_info(
            batch_client,
            config.POOL_ID,
            orchestrator_task_info.node_info.node_id,
        ),
    )
    orchestrator_response = ComputeNodeInfo(orchestrator_ip, orchestrator_port)
    worker_responses = []
    for i, _ in enumerate(orchestrator_info.hosts):
        worker_response = ComputeNodeInfo(orchestrator_info.hosts[i], orchestrator_info.ports[i])
        worker_responses.append(worker_response)
    return RespponseData(orchestrator_response, worker_responses)
