#!/bin/sh
android create project \
 --target 1 \
 --name netput_source_android \
 --path ./ns_android \
 --activity NSAndroidActivity \
 --package org.netput.sources.android