locals {
  json_data = jsondecode(file("./output.json"))
  party_size = local.json_data["party_size"]["value"]
  qmpc_instance_gips = local.json_data["qmpc_instance_gips"]["value"]
  private_key_path = "../../demo/terraform/application/${local.json_data["private_key_path"]["value"]}"
  gce_ssh_user =  local.json_data["gce_ssh_user"]["value"]
  party_num = local.party_size - 1
  client_id = length(local.qmpc_instance_gips) - 1
  party_ip = slice(local.qmpc_instance_gips, 0, local.party_num)
  ip_str = join(",", local.party_ip)
}

# ---------------------------
# benchmark
# ---------------------------
resource "null_resource" "benchmark" {
    provisioner "file" {
        source      = "../test"
        destination = "/home/${local.gce_ssh_user}/test"
        connection {
            host        = local.qmpc_instance_gips[local.client_id]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }
    }

    provisioner "file" {
        source      = "../../scripts/libclient/src"
        destination = "/home/${local.gce_ssh_user}/test/src"
        connection {
            host        = local.qmpc_instance_gips[local.client_id]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }
    }

    provisioner "file" {
        source      = "../../demo/terraform/gcp/json/service_account.json"
        destination = "/home/${local.gce_ssh_user}/test/service_account.json"
        connection {
            host        = local.qmpc_instance_gips[local.client_id]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }
    }

    provisioner "remote-exec" {
        connection {
            host        = local.qmpc_instance_gips[local.client_id]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }

        inline = [
            "cd /home/${local.gce_ssh_user}/test",
            "chmod +x ./prepare.sh && ./prepare.sh ${local.ip_str}",
            "chmod +x ./tester.sh && ./tester.sh",
        ]
    }
}
