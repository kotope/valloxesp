#!/bin/bash
# Script that installs vallox2mqtt (latest from master) into Home Assistant
git clone https://github.com/kotope/valloxesp.git
mv valloxesp/custom_components/vallox2mqtt .
rm -rf valloxesp
