#!/bin/bash

if [[ $CB_SERVICES == *"n1ql"* ]]; then
  # Wait for the query service to be up and running
  for attempt in $(seq 10)
  do
    curl -s http://127.0.0.1:8093/admin/ping > /dev/null \
      && break

    echo "Waiting for query service..."
    sleep 1
  done

  # We're seeing sporadic issues with "Operation not supported" creating indexes
  # As painful as it is, an extra sleep is called for to make sure N1QL is fully up and running
  sleep 5
fi

if [[ $CB_SERVICES == *"fts"* ]]; then
  # Wait for the FTS service to be up and running
  for attempt in $(seq 10)
  do
    curl -s -u $CB_USERNAME:$CB_PASSWORD http://127.0.0.1:8094/api/index > /dev/null \
      && break

    echo "Waiting for FTS service..."
    sleep 1
  done
fi

if [[ $CB_SERVICES == *"cbas"* ]]; then
  # Wait for the analytics service to be up and running
  for attempt in $(seq 10)
  do
    curl -s -u $CB_USERNAME:$CB_PASSWORD http://localhost:8095/analytics/config/service > /dev/null \
      && break

    echo "Waiting for analytics service..."
    sleep 1
  done
fi

if [[ $CB_SERVICES == *"eventing"* ]]; then
  # Wait for the eventing service to be up and running
  for attempt in $(seq 10)
  do
    curl -s -u $CB_USERNAME:$CB_PASSWORD http://127.0.0.1:8096/api/v1/functions > /dev/null \
      && break

    echo "Waiting for eventing service..."
    sleep 1
  done
fi
