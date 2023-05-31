# image for deploy
FROM envoyproxy/envoy:v1.26.1 as dep-runner
# --build-arg PARTY_ID=<0,1,2>が必要
ARG party_id
ENV PARTY_ID=${party_id}
COPY Deploy/Application/config/Party${PARTY_ID}/manage_container/envoy.yaml /etc/envoy/envoy.yaml