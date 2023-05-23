output "qmpc_instance_gips" {
  value = google_compute_instance.qmpc_k8s_vm.*.network_interface.0.access_config.0.nat_ip
}

output "instance_count" {
  value = var.instance_count
}

output "gce_ssh_user" {
  value = var.gce_ssh_user
}

output "private_key_path" {
  value = "../gcp/${local_file.private_key_pem.filename}"
}
