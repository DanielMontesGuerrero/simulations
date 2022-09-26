import logging

import azure.functions as func
from azure.batch import BatchServiceClient
from azure.batch.batch_auth import SharedKeyCredentials
from start_orchestrator.utils import *
from start_orchestrator.models import Response
from start_orchestrator import config


def main(req: func.HttpRequest) -> func.HttpResponse:
    logging.info('Initiating request')

    try:
        logging.info('Parsing request')
        request_data = parse_request(req)

        logging.info('Creating batch client')
        credentials = SharedKeyCredentials(
            config.BATCH_ACCOUNT_NAME, config.BATCH_ACCOUNT_KEY)
        batch_client = BatchServiceClient(
            credentials, batch_url=config.BATCH_ACCOUNT_URL)

        logging.info('Configuring workers and orchestrator')
        num_nodes = get_number_of_nodes(request_data)
        workers_info = get_workers_info(request_data, num_nodes)
        orchestrator_info = OrchestratorInfo(
            request_data.rows, request_data.cols)

        logging.info('Creating workers')
        create_workers(batch_client, workers_info, orchestrator_info)

        logging.info('Creating orchestrator')
        orchestrator_task_info = create_orchestrator(batch_client, orchestrator_info)
        response_data = create_response(batch_client, orchestrator_task_info, orchestrator_info)
    except ValueError:
        logging.error('Error parsing request')
        response = Response(False, 'Bad request', None)
        return func.HttpResponse(response.to_json(), status_code=400)
    except Exception as error:
        logging.error('Error processing request: %s', error)
        response = Response(False, f'Error processing request {error}', None)
        return func.HttpResponse(response.to_json(), status_code=500)
    else:
        response = Response(True, 'Ok', response_data)
        return func.HttpResponse(response.to_json(), status_code=200)
