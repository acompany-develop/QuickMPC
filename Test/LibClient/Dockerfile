FROM python:3.7 as base


FROM base as builder
# copy
COPY QuickMPC-libClient-py /QuickMPC-libClient-py
RUN rm /QuickMPC-libClient-py/.git
COPY .git/modules/QuickMPC-libClient-py /QuickMPC-libClient-py/.git

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
