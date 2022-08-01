# image for deploy
FROM envoyproxy/envoy:v1.17.1 as dep-runner
# --build-arg PARTY_ID=<0,1,2>が必要
ARG party_id
ENV PARTY_ID=${party_id}
COPY Deploy/Application/Config/Party${PARTY_ID}/ManageContainer/envoy.yaml /etc/envoy/envoy.yaml