# ---------------------------
# Vpc
# ---------------------------
resource "google_compute_network" "qmpc_k8s_vpc" {
  count        = var.instance_count
  name         = "${var.instance_name}-vpc-${count.index}"
  auto_create_subnetworks = false
}

resource "google_compute_subnetwork" "qmpc_k8s_sn" {
  count        = var.instance_count
  name         = "${var.instance_name}-sn-${count.index}"
  ip_cidr_range = var.subnet_cidr_range
  network       = google_compute_network.qmpc_k8s_vpc.*.self_link[count.index]
}


data "http" "ifconfig" {
  url = "http://ipv4.icanhazip.com/"
}

locals {
  myip = chomp(data.http.ifconfig.body)
}

# ---------------------------
# Firewall
# ---------------------------
resource "google_compute_firewall" "qmpc_k8s_fw" {
  count        = var.instance_count
  name         = "${var.instance_name}-fw-${count.index}"
  network = google_compute_network.qmpc_k8s_vpc.*.self_link[count.index]

  direction = "INGRESS"

  allow {
    protocol = "tcp"
    ports    = ["22"]
  }

  target_tags = [var.instance_name]
  source_ranges = ["${local.myip}/32"]
}

resource "google_compute_firewall" "qmpc_allow_tcp_from_parties_fw" {
  count        = var.instance_count
  name         = "${var.instance_name}-allow-tcp-from-parties-fw-${count.index}"

  network = google_compute_network.qmpc_k8s_vpc.*.self_link[count.index]

  direction = "INGRESS"

  # 全てのtcp通信を受け入れる
  allow {
    protocol = "tcp"
  }

  target_tags = [var.instance_name]
  source_ranges = concat(["${local.myip}/32"], google_compute_instance.qmpc_k8s_vm.*.network_interface.0.access_config.0.nat_ip)
  # source_ranges = concat(["${local.myip}/32"], google_compute_instance.qmpc_k8s_vm.*.network_interface.0.access_config.0.nat_ip, ["{ここにP1のGIPを設定}", "{ここにP2のGIPを設定}", ...]) # NOTE: 他のPartyのIPを設定してterraform applyしてください

  # 対象のNetworkInterfaceにNatIPがあるかどうかを判定するために、
  # google_compute_instanceから参照できるように `depends_on` を指定
  # depends_on = [
  #   google_compute_instance.qmpc_k8s_vm
  # ]
}
