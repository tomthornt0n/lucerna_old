#!/bin/bash

pushd ../vendor/bin/data_desk/linux
./data_desk --custom ../../../../DataDeskCustomLayer/bin/linux/libDataDeskCustomLayer.so ../../../../components.ds
popd
