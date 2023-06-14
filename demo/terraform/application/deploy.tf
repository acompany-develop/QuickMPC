locals {
    # party_sizeがdefaultのままであればlocal.instance_countを使う
    n_parties = var.party_size == -1 ? local.instance_count : var.party_size

    # btsを除いたパーティ数
    party_num = local.n_parties - 1

    # btsサーバのindex
    bts_id = local.n_parties - 1

    # btsを含むサーバのipのタプル
    party_ip = slice(local.qmpc_instance_gips, 0, local.n_parties)

    # サーバのipを,で区切った文字列
    ip_str = join(",", local.party_ip)
}

# ---------------------------
# set deploy directory
# ---------------------------
resource "null_resource" "setup" {
    count        = local.instance_count

    provisioner "file" {
        source      = "../../src"
        destination = "/home/${local.gce_ssh_user}/QuickMPC"
        connection {
            host        = local.qmpc_instance_gips[count.index]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }
    }

    provisioner "remote-exec" {
        connection {
            host        = local.qmpc_instance_gips[count.index]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }

        inline = [
            "cd /home/${local.gce_ssh_user}/QuickMPC",
            "sh setup.sh",
        ]
    }
}

resource "null_resource" "site24x7" {
    count        = local.instance_count

    provisioner "remote-exec" {
        connection {
            host        = local.qmpc_instance_gips[count.index]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }

        inline = [
            "wget https://staticdownloads.site24x7.com/server/Site24x7InstallScript.sh",
            "bash Site24x7InstallScript.sh -i -key=${{ secrets.SITE24X7 }} -automation=true"
        ]
    }
}

# ---------------------------
# Prepare deploy
# ---------------------------
resource "null_resource" "prepare_deploy" {
    triggers = {
        image_tag = "${var.docker_image_tag}"
    }
    count        = local.n_parties
    provisioner "remote-exec" {
        connection {
            host        = local.qmpc_instance_gips[count.index]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }
        inline = [
            "cd /home/${local.gce_ssh_user}/QuickMPC && chmod +x ./prepare_deploy.sh && ./prepare_deploy.sh ${count.index} ${local.ip_str} ${var.docker_image_tag}"
        ]
    }
    depends_on = [null_resource.setup, null_resource.site24x7]
}

# ---------------------------
# get jwt
# ---------------------------
resource "null_resource" "get_jwt" {
    provisioner "local-exec" {
        command = "scp -oStrictHostKeyChecking=no -i ${local.private_key_path} ${local.gce_ssh_user}@${local.qmpc_instance_gips[local.bts_id]}:/home/${local.gce_ssh_user}/QuickMPC/config/beaver_triple_service/client?*.sample.env ./"
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
        command = "scp -oStrictHostKeyChecking=no -i ${local.private_key_path} ./client${count.index}.sample.env ${local.gce_ssh_user}@${local.qmpc_instance_gips[count.index]}:/home/${local.gce_ssh_user}/QuickMPC/config/beaver_triple_service/client.sample.env"
        interpreter = ["bash", "-c"]
    }
    depends_on = [null_resource.get_jwt]
}

# ---------------------------
# deploy QuickMPC
# ---------------------------
resource "null_resource" "deploy_quickmpc" {
    count = local.n_parties
    provisioner "remote-exec" {
        connection {
            host        = local.qmpc_instance_gips[count.index]
            type        = "ssh"
            user        = "${local.gce_ssh_user}"
            private_key = file("${local.private_key_path}")
        }
        inline = [
            "cd /home/${local.gce_ssh_user}/QuickMPC",
            "make upd"
        ]
    }
    depends_on = [null_resource.sent_jwt]
}
