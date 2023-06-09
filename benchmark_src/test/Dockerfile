FROM python:3.7 as base


FROM base as builder

# install pipenv and modules
WORKDIR /packages/client/libclient-py
WORKDIR /libclient
# pypiから最新のquickmpcをinstall
RUN pip install -U pip && \
    pip install \
    pytest \
    tabulate \
    quickmpc


FROM builder as medium
