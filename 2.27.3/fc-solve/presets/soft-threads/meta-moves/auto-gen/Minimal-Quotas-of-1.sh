#!/bin/bash

# This script calculates a preset where the quotas are each 1-space.
perl process.pl -o quota-of-1s.dump.txt --quotas-expr='sub { if (shift() >= 10_000) { return; } else { return 1; } }' --quotas-are-cb
