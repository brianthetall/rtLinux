#!/bin/bash

gcc rtInfinite.c -o ../bin/rtInfinite_100us -pthread
gcc rtInfinite_1ms.c -o ../bin/rtInfinite_1ms -pthread
