#!/bin/bash

gcc client.c -o client -pthread -lwebsockets
gcc server.c -o server -pthread -lwebsockets
