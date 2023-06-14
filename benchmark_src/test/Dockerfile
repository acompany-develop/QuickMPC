FROM python:3.7 as base


FROM base as builder

ARG QMPC_VERSION
# install pipenv and modules
WORKDIR /packages/client/libclient-py
WORKDIR /libclient
# pypiから最新のquickmpcをinstall
RUN pip install -U pip && \
    pip install \
    pytest \
    tabulate \
    quickmpc==$QMPC_VERSION


FROM builder as medium
