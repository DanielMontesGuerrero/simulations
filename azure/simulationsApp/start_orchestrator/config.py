"""
Configure Batch and Storage Account credentials
"""
import os

BATCH_ACCOUNT_NAME = os.getenv('BATCH_ACCOUNT_NAME')
BATCH_ACCOUNT_KEY = os.getenv('BATCH_ACCOUNT_KEY')
BATCH_ACCOUNT_URL = os.getenv('BATCH_ACCOUNT_URL')

POOL_ID = os.getenv('POOL_ID')
JOB_ID = os.getenv('JOB_ID')
