#!/bin/bash

cd Build-Skripts
pytest --html=pytest_report.html

google-chrome pytest_report.html