# ---------------------------
# Key pair
# ---------------------------
locals {
  public_key_file = "./.key_pair/${var.key_name}.id_rsa.pub"
  private_key_file = "./.key_pair/${var.key_name}.id_rsa"
}

resource "tls_private_key" "qmpc_k8s_keygen" {
  algorithm = "RSA"
  rsa_bits = 4096
}

resource "local_file" "private_key_pem" {
  filename = local.private_key_file
  content = tls_private_key.qmpc_k8s_keygen.private_key_pem
  file_permission = "0600"
}

resource "local_file" "public_key_pem" {
  filename = local.public_key_file
  content = tls_private_key.qmpc_k8s_keygen.public_key_openssh
  file_permission = "0600"
}

# ---------------------------
# static ip address
# ---------------------------
resource "google_compute_address" "qmpc_k8s_static_ip" {
  count      = var.instance_count
  name       = "${var.instance_name}-static-ip-${count.index}"
  project    = var.project_id
  region     = var.region
}

# ---------------------------
# Vm instance
# ---------------------------
locals {
  bts_index = var.contain_client ? var.instance_count - 2 : var.instance_count -1
  client_index = var.contain_client ? var.instance_count -1 : -1
}
resource "google_compute_instance" "qmpc_k8s_vm" {
  count        = var.instance_count
  name         = "${var.instance_name}-vm-${count.index}"
  machine_type = count.index == local.client_index ? var.client_machine_type : count.index == local.bts_index ? var.bts_machine_type : var.mechine_type
  zone         = "${var.region}-${var.zone}"

  tags = [var.instance_name]

  boot_disk {
    initialize_params {
      image = var.disk_image
      type  = var.disk_type
      size  = var.disk_size
    }
  }

  network_interface {
    network    = google_compute_network.qmpc_k8s_vpc.*.self_link[count.index]
    subnetwork = google_compute_subnetwork.qmpc_k8s_sn.*.self_link[count.index]
    access_config {
      nat_ip = google_compute_address.qmpc_k8s_static_ip.*.address[count.index]
    }
  }

  scheduling {
    provisioning_model = "STANDARD"
    # provisioning_model = "SPOT"
    # preemptible        = true
    # automatic_restart  = false
  }

    metadata = {
      # NOTE: 公開鍵を<protocol> <key-blob> google-sshの形式に変更している
      ssh-keys = "${var.gce_ssh_user}:${trim(tls_private_key.qmpc_k8s_keygen.public_key_openssh, "\n")} google-ssh"
  }
}
