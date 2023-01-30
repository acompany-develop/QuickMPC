FROM python:3.7 as base


FROM base as builder
# setuptools_scmが_version.pyを作る時にgitのversionが必要なので.git/をcopy
COPY .git/ /.git/
COPY packages/client/libclient-py /packages/client/libclient-py

# install pipenv and modules
WORKDIR /packages/client/libclient-py
WORKDIR /LibClient
RUN pip install -U pip && \
  pip install \
    pytest \
    scikit-learn \
    /packages/client/libclient-py


FROM builder as medium

FROM builder as large
