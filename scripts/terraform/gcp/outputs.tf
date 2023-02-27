output "qmpc_instance_gip" {
  value = google_compute_instance.qmpc_k8s_vm.network_interface.0.access_config.0.nat_ip
}
