FROM grafana/grafana:9.4.3-ubuntu

COPY ./config/grafana/dashboard/ /var/lib/grafana/dashboards/
COPY ./config/grafana/dashboard.yml /etc/grafana/provisioning/dashboards/dashboard.yml
COPY ./config/grafana/datasource.yml /etc/grafana/provisioning/datasources/datasource.yml

RUN grafana-cli plugins install grafana-piechart-panel