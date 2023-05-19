locals {
  party_num = var.instance_count - 1
  bts_num = var.instance_count - 1
  ip1 = google_compute_instance.qmpc_k8s_vm[0].network_interface.0.access_config.0.nat_ip
  ip2 = google_compute_instance.qmpc_k8s_vm[1].network_interface.0.access_config.0.nat_ip
  ip3 = google_compute_instance.qmpc_k8s_vm[2].network_interface.0.access_config.0.nat_ip
}

# ---------------------------
# set deploy directory
# ---------------------------
resource "null_resource" "copy_file" {
    count        = var.instance_count

    provisioner "file" {
        source      = "../../deploy"
        destination = "/home/ubuntu/QuickMPC"
        connection {
            host        = google_compute_instance.qmpc_k8s_vm[count.index].network_interface.0.access_config.0.nat_ip
            type        = "ssh"
            user        = "${var.gce_ssh_user}"
            private_key = file("${local_file.private_key_pem.filename}")
        }
    }

    depends_on = [google_compute_instance.qmpc_k8s_vm]
}


# ---------------------------
# setup
# ---------------------------
resource "null_resource" "setup" {
    count = var.instance_count
    provisioner "remote-exec" {
        connection {
            host        = google_compute_instance.qmpc_k8s_vm[count.index].network_interface.0.access_config.0.nat_ip
            type        = "ssh"
            user        = "${var.gce_ssh_user}"
            private_key = file("${local_file.private_key_pem.filename}")
        }

        inline = [
            "cd /home/ubuntu/QuickMPC",
            "sh setup.sh",
        ]
    }

    depends_on = [null_resource.copy_file]
}

# ---------------------------
# create JWT
# ---------------------------
resource "null_resource" "create_jwt" {
    provisioner "remote-exec" {
        connection {
            host        = google_compute_instance.qmpc_k8s_vm[local.bts_num].network_interface.0.access_config.0.nat_ip
            type        = "ssh"
            user        = "${var.gce_ssh_user}"
            private_key = file("${local_file.private_key_pem.filename}")
        }
        inline = [
            "cd /home/ubuntu/QuickMPC && chmod 777 ./prepare_deploy.sh && ./prepare_deploy.sh ${local.bts_num} ${local.ip1} ${local.ip2} ${local.ip3} ${var.docker_image_tag}"
        ]
    }
    depends_on = [null_resource.setup]
}

# TODO
# ---------------------------
# get jwt
# ---------------------------
# resource "null_resource" "get_jwt" {
#     count        = local.party_num
#     provisioner "local-exec" {
#         command = "scp -i ${local_file.private_key_pem.filename} ${var.gce_ssh_user}@${local.ip3}:/home/ubuntu/QuickMPC/config/beaver_triple_service/client${count.index}.sample.env ./"
#         interpreter = ["bash", "-c"]
#     }
#     depends_on = [null_resource.create_jwt]
# }
#
# # ---------------------------
# # sent jwt
# # ---------------------------
# resource "null_resource" "sent_jwt_to_1" {
#     count        = local.party_num
#     provisioner "local-exec" {
#         command = "scp -i ${local_file.private_key_pem.filename} ./client0.sample.env ${var.gce_ssh_user}@${local.ip1}:/home/ubuntu/QuickMPC/config/beaver_triple_service/client.sample.env"
#         interpreter = ["bash", "-c"]
#     }
#     depends_on = [null_resource.get_jwt]
# }
#
# resource "null_resource" "sent_jwt_to_2" {
#     count        = local.party_num
#     provisioner "local-exec" {
#         command = "scp -i ${local_file.private_key_pem.filename} ./client1.sample.env ${var.gce_ssh_user}@${local.ip2}:/home/ubuntu/QuickMPC/config/beaver_triple_service/client.sample.env"
#         interpreter = ["bash", "-c"]
#     }
#     # depends_on = [null_resource.get_jwt]
# }

# ---------------------------
# set config
# ---------------------------

# TODO jwt持ってくる
locals {
    jwt_list = ["eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiI3NjQ0OThiYy00YjExLTRiMGUtOWM5MC0yN2RiZDExYzA0OTUiLCJwYXJ0eV9pZCI6MSwicGFydHlfaW5mbyI6W3siaWQiOjEsImFkZHJlc3MiOiIzNC45Mi4yMjMuMTcxIn0seyJpZCI6MiwiYWRkcmVzcyI6IjM0LjE1MC40OS4yOCJ9XSwic3ViIjoiMGRhMjI4MWQtODc0NS00MTdlLTlhYTgtMDE5ZGY4MzVkNWExIiwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.7lzisgpFfGHA9vJEvH2VdgEOm3voYvT8G8alIWxKipI","eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiJlOWI0ZDhjNi05NjUzLTRlNWYtYTQ3OC05ZjBlN2MzMmQxYzgiLCJwYXJ0eV9pZCI6MiwicGFydHlfaW5mbyI6W3siaWQiOjEsImFkZHJlc3MiOiIzNC45Mi4yMjMuMTcxIn0seyJpZCI6MiwiYWRkcmVzcyI6IjM0LjE1MC40OS4yOCJ9XSwic3ViIjoiNmVlZmNlYzUtMzk0NC00M2I4LThmZjAtNzlmNTdkMWMxYTM2IiwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.p5Rn-ojOmaPg36TP6mlnf4CiQNxAjGHEtiEJifsgKog"]
}

resource "null_resource" "set_config" {
    count = local.party_num

    provisioner "remote-exec" {
        connection {
            host        = google_compute_instance.qmpc_k8s_vm[count.index].network_interface.0.access_config.0.nat_ip
            type        = "ssh"
            user        = "${var.gce_ssh_user}"
            private_key = file("${local_file.private_key_pem.filename}")
        }

        inline = [
            "cd /home/ubuntu/QuickMPC && chmod 777 ./prepare_deploy.sh && ./prepare_deploy.sh ${count.index} ${local.ip1} ${local.ip2} ${local.ip3} ${var.docker_image_tag} ${local.jwt_list[count.index]}"
        ]
    }
    # depends_on = [null_resource.deploy_bts]
    depends_on = [null_resource.create_jwt]
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
            "cd /home/ubuntu/QuickMPC",
            "make upd"
        ]
    }
    depends_on = [null_resource.set_config]
}