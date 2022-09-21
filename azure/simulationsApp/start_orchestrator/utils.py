from typing import List, Tuple
from start_orchestrator.models import RequestData, WorkerInfo, OrchestratorInfo
from azure.batch import BatchServiceClient
import start_orchestrator.config as config
import azure.functions as func
from azure.batch import BatchServiceClient
import azure.batch.models as batchmodels
import time
import datetime
import sys
import logging

NUM_NODES_BREAKPOINT_1 = 1000 * 1000
NUM_NODES_BREAKPOINT_2 = 5000 * 5000

def add_task(batch_service_client: BatchServiceClient, job_id: str, command: str) -> str:
    """
    Adds a task for each input file in the collection to the specified job.
    :param batch_service_client: A Batch service client.
    :param str job_id: The ID of the job to which to add the tasks.
    :param list resource_input_files: A collection of input files. One task will be
     created for each input file.
    """
    id = f'task-{int(time.time()*1000)}'
    logging.info(f'Adding task [{id}] to job [{job_id}]...')
    batch_service_client.task.add(job_id, batchmodels.TaskAddParameter(
        id=id,
        command_line=command,
    ))
    return id

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

    logging.info(f"Monitoring all tasks for 'Completed' state, timeout in {timeout}...", end='')

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
    raise RuntimeError("ERROR: Tasks did not reach 'Completed' state within "
                       "timeout period of " + str(timeout))

def get_task_info(batch_service_client: BatchServiceClient, job_id: str, task_id: str):
    logging.info(f'Waiting to get task info. job_id [{job_id}] task_id [{task_id}]')
    while True:
        print('.', end='')
        sys.stdout.flush()
        cloud_task = batch_service_client.task.get(job_id, task_id, batchmodels.TaskGetOptions(select='nodeInfo'))
        if cloud_task != None and cloud_task.node_info != None:
            break
    print()
    return cloud_task

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

def get_last_divisors(num: int) -> Tuple[int,int]:
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
    command = f'echo \"-rows={worker.rows} -cols={worker.cols} -port={worker.port} -protocol={worker.protocol}\"'
    task_id = add_task(batch_client, config.JOB_ID, command)
    return get_task_info(batch_client, config.JOB_ID, task_id)

def create_workers(batch_client: BatchServiceClient, workers_info: List[WorkerInfo], orchestrator_info: OrchestratorInfo):
    for worker in workers_info:
        task_info = create_worker(batch_client, worker)
        orchestrator_info.hosts.append(task_info.node_info.node_url)
        orchestrator_info.ports.append(worker.port)

def create_orchestrator(batch_client: BatchServiceClient, orchestrator_info: OrchestratorInfo):
    hosts = ','.join(orchestrator_info.hosts)
    ports = ','.join([str(port) for port in orchestrator_info.ports])
    command = f'echo \"-rows={orchestrator_info.rows} -cols={orchestrator_info.cols} -port={orchestrator_info.port} -protocol={orchestrator_info.protocol} -hosts={hosts} -ports={ports}\"'
    task_id = add_task(batch_client, config.JOB_ID, command)
    return get_task_info(batch_client, config.JOB_ID, task_id)
