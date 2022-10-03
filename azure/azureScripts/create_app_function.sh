#!/bin/bash

resourcegroup="RESOURCE_GRUOP"
storageaccount="STORAGE_ACCOUNT"
appname="simulationsApp"
location="eastus"

# Iniciar sesión
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

# Crear function app
az functionapp create \
    --resource-group $resourcegroup \
    --consumption-plan-location "$location" \
    --runtime python \
    --functions-version 3 \
    --name $appname \
    --storage-account $storageaccount

# Desplegar app
cd azure/simulationsApp
func azure functionapp publish $appname
