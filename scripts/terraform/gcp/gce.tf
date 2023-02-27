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
# Vm instance
# ---------------------------
resource "google_compute_instance" "qmpc_k8s_vm" {
  name         = var.instance_name
  machine_type = var.mechine_type
  zone         = var.zone

  tags = [var.instance_name]

  boot_disk {
    initialize_params {
      image = var.disk_image
      type  = var.disk_type
    }
  }

  network_interface {
    network    = google_compute_network.qmpc_k8s_vpc.name
    subnetwork = google_compute_subnetwork.qmpc_k8s_sn.name
    access_config {}
  }

  scheduling {
    provisioning_model = "STANDARD"
    # preemptible        = true
    # automatic_restart  = false
  }

    metadata = {
      ssh-keys = "${var.gce_ssh_user}:${tls_private_key.qmpc_k8s_keygen.public_key_openssh}"
  }
}