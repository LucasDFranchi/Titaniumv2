#!/bin/bash

DOCKER_IMAGE="titanium_env"

# Check if Docker image exists locally
if [[ "$(docker images -q $DOCKER_IMAGE 2> /dev/null)" == "" ]]; then
  echo "Docker image '$DOCKER_IMAGE' not found. Building..."
  docker build -t $DOCKER_IMAGE .
else
  echo "Using existing Docker image '$DOCKER_IMAGE'."
fi

# Run Docker container
docker run -it -v $(pwd)/../:/app $DOCKER_IMAGE
