"""
Configure Batch and Storage Account credentials
"""
import os

BATCH_ACCOUNT_NAME = os.getenv('BATCH_ACCOUNT_NAME', '')
BATCH_ACCOUNT_KEY = os.getenv('BATCH_ACCOUNT_KEY', '')
BATCH_ACCOUNT_URL = os.getenv('BATCH_ACCOUNT_URL', '')

POOL_ID = os.getenv('POOL_ID', '')
JOB_ID = os.getenv('JOB_ID', '')
INBOUND_ENDPOINT_NAME = os.getenv('INBOUND_ENDPOINT_NAME', '')
BACK_END_PORT = int(os.getenv('BACK_END_PORT', '3000'))
APP_PACKAGE_VERSION = os.getenv('APP_PACKAGE_VERSION', '').replace('.', '_')

WORKER_COMMAND = ('/bin/bash -c \"cd $AZ_BATCH_APP_PACKAGE_orchestrator_' + APP_PACKAGE_VERSION
                  + ' && ./worker -rows={} -cols={} -host={} -port={} -protocol={}\"')

ORCH_COMMAND = ('/bin/bash -c \"cd $AZ_BATCH_APP_PACKAGE_orchestrator_' + APP_PACKAGE_VERSION
                + ' && ./orchestrator -rows={} -cols={} -host={} -port={} -protocol={} '
                '-hosts={} -ports={}\"')
