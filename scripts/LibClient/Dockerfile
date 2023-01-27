FROM python:3.7 as base


FROM base as builder
# copy
COPY packages/client/libclient-py /QuickMPC-libClient-py

# install pipenv and modules
WORKDIR /LibClient
RUN pip install -U pip && \
  pip install \
  pytest \
  seaborn \
  scikit-learn \
  tabulate \
  /QuickMPC-libClient-py


FROM builder as medium

FROM builder as large
