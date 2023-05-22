locals {
  party_num = var.instance_count - 1
  bts_num = var.instance_count - 1
  ip_str = join(",", google_compute_instance.qmpc_k8s_vm.*.network_interface.0.access_config.0.nat_ip)
}

# ---------------------------
# set deploy directory
# ---------------------------
resource "null_resource" "setup" {
    count        = var.instance_count

    provisioner "file" {
        source      = "../../deploy"
        destination = "/home/${var.gce_ssh_user}/QuickMPC"
        connection {
            host        = google_compute_instance.qmpc_k8s_vm[count.index].network_interface.0.access_config.0.nat_ip
            type        = "ssh"
            user        = "${var.gce_ssh_user}"
            private_key = file("${local_file.private_key_pem.filename}")
        }
    }

    provisioner "remote-exec" {
        connection {
            host        = google_compute_instance.qmpc_k8s_vm[count.index].network_interface.0.access_config.0.nat_ip
            type        = "ssh"
            user        = "${var.gce_ssh_user}"
            private_key = file("${local_file.private_key_pem.filename}")
        }

        inline = [
            "cd /home/${var.gce_ssh_user}/QuickMPC",
            "sh setup.sh",
        ]
    }

    depends_on = [google_compute_firewall.qmpc_k8s_fw]
}

# ---------------------------
# Prepare deploy
# ---------------------------
resource "null_resource" "prepare_deploy" {
    count        = var.instance_count
    provisioner "remote-exec" {
        connection {
            host        = google_compute_instance.qmpc_k8s_vm[count.index].network_interface.0.access_config.0.nat_ip
            type        = "ssh"
            user        = "${var.gce_ssh_user}"
            private_key = file("${local_file.private_key_pem.filename}")
        }
        inline = [
            "cd /home/${var.gce_ssh_user}/QuickMPC && chmod +x ./prepare_deploy.sh && ./prepare_deploy.sh ${count.index} ${local.ip_str} ${var.docker_image_tag}"
        ]
    }
    depends_on = [null_resource.setup]
}

# ---------------------------
# get jwt
# ---------------------------
resource "null_resource" "get_jwt" {
    provisioner "local-exec" {
        command = "scp -oStrictHostKeyChecking=no -i ${local_file.private_key_pem.filename} ${var.gce_ssh_user}@${google_compute_instance.qmpc_k8s_vm[local.bts_num].network_interface.0.access_config.0.nat_ip}:/home/${var.gce_ssh_user}/QuickMPC/config/beaver_triple_service/client?*.sample.env ./"
        interpreter = ["bash", "-c"]
    }
    depends_on = [null_resource.prepare_deploy]
}

# ---------------------------
# sent jwt
# ---------------------------
resource "null_resource" "sent_jwt" {
    count        = local.party_num
    provisioner "local-exec" {
        command = "scp -oStrictHostKeyChecking=no -i ${local_file.private_key_pem.filename} ./client${count.index}.sample.env ${var.gce_ssh_user}@${google_compute_instance.qmpc_k8s_vm[count.index].network_interface.0.access_config.0.nat_ip}:/home/${var.gce_ssh_user}/QuickMPC/config/beaver_triple_service/client.sample.env"
        interpreter = ["bash", "-c"]
    }
    depends_on = [null_resource.get_jwt]
}

# ---------------------------
# deploy QuickMPC
# ---------------------------
resource "null_resource" "deploy_quickmpc" {
    count = var.instance_count
    provisioner "remote-exec" {
        connection {
            host        = google_compute_instance.qmpc_k8s_vm[count.index].network_interface.0.access_config.0.nat_ip
            type        = "ssh"
            user        = "${var.gce_ssh_user}"
            private_key = file("${local_file.private_key_pem.filename}")
        }
        inline = [
            "cd /home/${var.gce_ssh_user}/QuickMPC",
            "make upd"
        ]
    }
    depends_on = [null_resource.sent_jwt]
}
