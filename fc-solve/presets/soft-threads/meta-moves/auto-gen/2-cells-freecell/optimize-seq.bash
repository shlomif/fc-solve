#!/bin/bash
export FCS_DATA_DIR="$(pwd)"
(cd ../optimize-seq ; make results) | tee find_optimal_quotas-300.dump
