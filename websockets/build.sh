#!/bin/bash

gcc client.c -o ../bin/client -pthread -lwebsockets
gcc server.c -o ../bin/server -pthread -lwebsockets
