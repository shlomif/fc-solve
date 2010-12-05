#!/bin/bash
export FCS_DATA_DIR="$(pwd)"
(cd optimize-seq ; make more_results) | tee -a find_optimal_quotas-300.dump
