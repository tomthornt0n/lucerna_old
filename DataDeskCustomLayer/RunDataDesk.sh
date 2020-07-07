#!/bin/bash

pushd ../Vendor/data_desk/linux
./data_desk --custom ../../../DataDeskCustomLayer/bin/linux/libDataDeskCustomLayer.so ../../../Client/Assets/Metafiles/Components.ds
popd
