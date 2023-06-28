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
  region        = var.region
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

resource "google_compute_firewall" "qmpc_allow_tcp_from_parties_fw" {
  count        = var.instance_count
  name         = "${var.instance_name}-allow-tcp-from-parties-fw-${count.index}"

  network = google_compute_network.qmpc_k8s_vpc.*.self_link[count.index]

  direction = "INGRESS"

  # 全てのtcp通信を受け入れる
  allow {
    protocol = "tcp"
  }

  source_ranges = concat(["${local.myip}/32"], google_compute_address.qmpc_k8s_static_ip.*.address)
}