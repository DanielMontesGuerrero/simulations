#!/bin/bash

resourcegroup="RESOURCE_GRUOP"
storageaccount="STORAGE_ACCOUNT"
batchaccount="BATCH_ACCOUNT"
location="eastus"

# Inicar sesión
az login

# Crear grupo de recursos
az group create \
	--name $resourcegroup \
	--location "$location"

# Crear cuenta de almacenamiento
az storage account create \
    --resource-group $resourcegroup \
    --name $storageaccount \
    --location "$location" \
    --sku Standard_LRS

# Crear cuenta batch
az batch account create \
    --name $batchaccount \
    --storage-account $storageaccount \
    --resource-group $resourcegroup \
    --location "$location"

# Iniciar sesión en la cuenta batch
az batch account login \
    --name $batchaccount \
    --resource-group $resourcegroup \
    --shared-key-auth

# Crear pool
az batch pool create pool-configuration.json

# Crear job
az batch job create \
    --id gameoflife_job \
    --pool-id gameoflife_pool

# Seguir los pasos en: https://learn.microsoft.com/en-us/azure/batch/batch-application-packages#upload-and-manage-applications para añadir un paquete de aplicación que contenga los binarios del orquestrador y el worker
